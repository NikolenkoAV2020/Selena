// SelenaClientScan.cpp
// Walter Artyomenko, August 2019
//
// Implementation of Scaning - function for scanning pointed targets 
// in according users command. This function execute in separate thread.
// And in fact it's a worker bee who do main job pool about scanning network. 
// In this part of source code calling masscan and nmap API, 
// combine them results. 
//-----------------------------------------------------------------------------
#include "SelenaClient.h"
#include "SelenaClientGlobal.h"

//-----------------------------------------------------------------------------
// RU. Функция рабочего потока сканирования
// EN. Scan workflow function
void* Scaning(void* P)
{
	SelenaJob J;
	clientJob->V().Get(J);

	std::string dirName;
	clientJobsStorage->V().StorageDir(dirName);

	std::string jobResFileName, fingPrintFileName, mergeResFileName;
	jobResFileName    = dirName + "/" + J.storageName + ".jobr";
	fingPrintFileName = dirName + "/" + J.storageName + "fingerprint.txt";
	mergeResFileName  = dirName + "/" + J.storageName + "s.jobr";

	J.completCode = 0;
	J.stage       = 3;
	clientJob->V().Set(J);

	J.SpellOutCommand();
	int i, r = 0, n = J.CountCommand();
	std::string command;

	for (i = 0; i < n; i++) {
		if (J[i].tool < 0 || !J(i).length()) {
			// Если работы никакой не задано, или неверный формат,
			// то немедленно завершаем поток сканирования.
			J.errorMessage = "Bad command";
			J.completCode  = -1;
			J.stage = 0;
			clientJob->V().Set(J);
			return nullptr;
		}
	}

	MessageToClient("Begin job: " + J.storageName + "...", (int)smtMessage);

	//-------------------------------------------------------------------------
	for (i = 0; i < n; i++) {
		std::string toolName = "";
		switch(J[i].tool) {
			case 0: toolName = "Masscan"; break;
			case 1: toolName = "Nmap"; break;
		}
		command = J(i);
		MessageToClient( "\nLaunch " + toolName + 
			"... Command: " + command, (int)smtMessage);

		//---------------------------------------------------------------------
		// Magicality input-output files give us possibility 
		// concatenation several scan procedure

		// name of input file in command 
		std::string inputFileName;
		CommandFindInFileName(command, inputFileName);
		// name of output file in command 
		std::string outputFileName;
		CommandFindOutFileName(command, outputFileName);

		// Unique output file name
		std::string outputFileNameNew;
		outputFileNameNew = dirName + "/" + J.storageName + ".jobr";
		// Unique input file name
		std::string inputFileNameNew;
		inputFileNameNew  = dirName + "/" + J.storageName + "ip.jobr";

		if (i < 1) {
			if (outputFileName.length()) {
				// replace output file name to unique name
				ReplaceStdSubstr(command, outputFileName, outputFileNameNew);
			}
			else {
				// if output file is absent in command then 
				// we simple adding json otput file 
				command += " -oJ " + outputFileNameNew;
			}
		}

		if (i > 0) {
			if (inputFileName.length()) {
				// replace output file name to unique name ...
				// Substitute target file name with name that 
				// correspond to storage name.
				ReplaceStdSubstr(command, inputFileName, inputFileNameNew);
			} else {
				// if output file is absent in command then 
				// we simple adding json otput file 
				command += " -iL " + inputFileNameNew;
			}
		}

		//---------------------------------------------------------------------
		// Scan tool selection
		if (J[i].tool == 0) {
			struct MasScanReport MSR;
			r = LaunchMasScan(
				command.c_str(),
				dirName.c_str(),
				J.storageName.c_str(),
				&MSR);

			// What masscan finish?
			if (r || MSR.errCode) {
				J.errorMessage = "Masscan terminated with an error.";
				J.completCode = 1;
				J.stage = 101;

				clientJob->V().Set(J);
				MessageToClient(J.errorMessage, (int)smtError);

				return nullptr;
			}

			if (!MSR.countHostOpen) {
				J.stage = 5;
				clientJob->V().Set(J);

				J.errorMessage = "No open ports found.";
				J.completCode = 0;
				J.stage = 100;

				clientJobsStorage->V().OutputOfWork(J);
				clientJob->V().Set(J);
				MessageToClient(J.errorMessage, (int)smtWarning);

				return nullptr;
			}

			// Write job to current job keeper.
			J.errorMessage = "Masscan terminated successfully";
			J.completCode = 0;
			J.stage       = 4;
			clientJob->V().Set(J);
			MessageToClient(J.errorMessage, (int)smtMessage);
		}
		else {
			if (J[i].tool == 1) {
				//
				r = LaunchNmap(
					command.c_str(),
					J.storageName.c_str(),
					dirName.c_str());

				// What Nmap finish?
				if (r) {
					J.errorMessage = "Nmap terminated with an error.";
					J.completCode = 2;
					J.stage = 102;

					clientJob->V().Set(J);
					MessageToClient(J.errorMessage, (int)smtError);
					return nullptr;
				}
				// Nmap finish
				J.errorMessage = "Nmap scan completed successfully";
				J.completCode  = 0;
				J.stage        = 5;
				clientJob->V().Set(J);
				MessageToClient(J.errorMessage, (int)smtMessage);
			}
		}

		// Build list of founded ip-address	
		// Extracting ip-address from given file
		r = ExtractIPfromFile(outputFileNameNew, inputFileNameNew);
		if (r) {
			break;
		}

		//---------------------------------------------------------------------
		// Сonsolidation of results
		if (i > 0) {
			// Merge current result json file whith rusult of scaning 
			// open host fingerprints
			r = MergeIpJsonFiles(
				jobResFileName, 
				fingPrintFileName, 
				mergeResFileName);
			if (r) {
				break;
			}
		}
	}

	// Remame merge file to job result file
	remove(jobResFileName.c_str());
	rename(mergeResFileName.c_str(), jobResFileName.c_str());

	// Scanning finish
	J.errorMessage = "Scan completed successfully";
	J.completCode  = 0;
	J.stage        = 100;

	clientJobsStorage->V().OutputOfWork(J);
	clientJob->V().Set(J);
	MessageToClient("Job " + J.storageName + " completed successfully", (int)smtMessage);

	return nullptr;
}

//-----------------------------------------------------------------------------
