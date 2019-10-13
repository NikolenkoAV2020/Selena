// SelenaJob.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_JOB_HEADER
#define SELENA_JOB_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <SelenaSafeMessage.h>
#include <SelenaSafeObject.h>

#define SELENA_SIZE_KEY_WORD	32
#define SELENA_KEY_READYB		"IamReadToJob"
#define SELENA_KEY_BEGJOB		"BeginJob"
#define SELENA_KEY_ENDJOB		"EndJob"
#define SELENA_KEY_ERRORJOB		"ErrorJob"
#define SELENA_KEY_STAGEJOB		"StageJob"

//-----------------------------------------------------------------------------
// Job description of the client. This structure sent to the client. 
// After this structure, a string of commands for scanning 
// and a file with an indication of scanning targets are also 
// transmitted over the network (if the fileSize field is not zero).
// If fileSize field equally zero the target file not transmiting.
#pragma pack (push, 1)
struct SelenaJobHeader{
	pthread_t      jID;			        // Unique identifier of the task
	unsigned short sizeCommad     = 0;	// Size of scan command string
	unsigned int   targetCount    = 0;	// Count of targer for scaning
	unsigned int   fileInputSize  = 0;	// Aattached file size
	unsigned int   fileOutputSize = 0;	// Output file size
	char           varScan = 1;			// Кind of scaning
										// 0 - NMap
										// 1 - MasScan
	SelenaJobHeader() {
		memset(&jID, 0, sizeof(jID));
	}
};
#pragma pack (pop)

struct SelenaScanCommand {
	// number (name) of scan tool
	//		0 -	masscan
	//		1 - nmap
	char			tool;
	// string that contain list of command parametrs
	std::string		params;
};

class SelenaJob {
public:
	SelenaJobHeader	h;
	// scan commands chain
	std::vector<SelenaScanCommand> scanChain;
	std::string		command        = "";
	std::string		fileInputName  = "";
	std::string		fileOutputName = "";
	std::string		errorMessage   = "";
	std::string		storageName    = "";
	// Meaning value of the "completCode" variable 
	//	0 - all is Ok.
	//	1 - 
	//	2 - 
	int				completCode = 0;
	// Means value of stage:
	// Meaning value of the "stage" variable 
	//	0   - job has not yet begun
	//	1   - begining job
	//	2   - first part of job execut
	//	3   - first part of job complited
	//	4   - first part of job completed
	//	5   - second part of job completed
	//	100 - the work is completed full and redy to send to server
	char			stage       = 0;

	SelenaJob() {}
	~SelenaJob() {}

	int  SpellOutCommand();
	int  CountCommand();

	SelenaScanCommand& operator[] (int   i) { return scanChain[i];}
	SelenaScanCommand& operator[] (short i) { return scanChain[i];}
	SelenaScanCommand& operator[] (long  i) { return scanChain[i];}
	std::string& operator() (int   i) { return scanChain[i].params;}
	std::string& operator() (short i) { return scanChain[i].params;}
	std::string& operator() (long  i) { return scanChain[i].params;}
	
	void Set(const SelenaJob& j) { *this = j; };
	void Get(SelenaJob& j) const { j = *this; };

protected:
};

//-----------------------------------------------------------------------------
class SelenaJobsManager {
public:
	SelenaJob	J;
protected:
	std::string	dirName = "./SJobs";
	
private:

public:
	SelenaJobsManager() {}
	~SelenaJobsManager() {}
	// Get count results in storage
	int Count() {

		return 0;
	}

	int StorageDir(std::string& d);
	int FullStorageName(std::string& fileName, std::string& fullName);
	int PutCommandToJobStorage(std::string& command, std::string& comFile);

	// Create a unique file name
	int MakeUniqueName(
		const char*  rootName,	// Корень имени создаваемого файла
		std::string& jobName);	// Имя файла с заданием на выполнение работы

	// Take to work
	int TakeToWork     (SelenaJob& J);
	int TakeToWorkFirst(SelenaJob& J);
	// Output of work
	int OutputOfWork   (SelenaJob& J);

	// Storage to file & restory
	int StorageJ(SelenaJob& J);
	int RestoryJ(SelenaJob& J, char stage = 0);
	// Remove job & result
	int RemoveJ (SelenaJob& J);
	// First job in list
	int GetFirstJob(SelenaJob& J);
	// First result in list
	int GetFirstRes(SelenaJob& J);
	// Job is completed and need create result file 
	// and mark job as performed.
	int CreateResultFile(SelenaJob& J);

	int GetJobsCount();
	int GetResultsCount();
	int GetJobsList(std::vector<std::string>& JL, bool forRes = false);
	int GetFileList(std::vector<std::string>& FL, const char* masc, bool excludeExt = true);

private:
	void WriteNameString(
		std::ofstream& f, 
		std::string& name);
	void ReadNameString(
		std::ifstream& f, 
		char** buf, int& lenbuf, std::string& name);

	bool IsStrEqualMask(char* s, char* p);
};

//-----------------------------------------------------------------------------
typedef ZSelenaSafeObject<SelenaJob>			ThrSfSelenaJob;
typedef ZSelenaSafeObject<SelenaJobsManager>	ThrSfSelenaJobsMng;


#endif // #ifndef SELENA_JOB_HEADER
