// SelenaClientSendResToServer.cpp
// Walter Artyomenko, September 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
// Send result to server.
int SendScanResultToServer(SelenaJob J, ZSelenaSocket* pClient)
{
	if (!pClient || !pClient->IsReady()) return-1;

	std::string s;
	s += "Job: " + J.storageName + "\n";
	s += "Command: " + J.command + "\n";
	s += "File: " + J.fileOutputName + "\n";
	s += "Solution of the problem completed successfully.";

	int r = 0;
	std::string d = "./SJobs/";
	std::string f = d + J.storageName + ".jobr";

	ZSelenaMessage M;
	M.msg = s;
	M.type = smtScanResult;
	M.msgFileName  = f;
	M.storFileName = J.fileOutputName;

	r = pClient->WriteOutgoing(M);
	if (r) {
		ZSelenaMessage ME;
		ME.msg  = "Can't send results for Job " + J.storageName + "\n";
		ME.msg += "Command: " + J.command + "\n";
		ME.type = smtWarning;
		clientMsg->V().Push(ME);
		//pClient->WriteOutgoing(M);
		return r;
	}

	// Remove Job & all results from local disk...
	std::vector<std::string> FL;
	std::string templ = "*" + J.storageName + "*.*";
	clientJobsStorage->V().GetFileList(FL, templ.c_str(), false);
	for (auto &F : FL) {
		F = "./SJobs/" + F;
		remove(F.c_str());
	}

	return r;
}

//-----------------------------------------------------------------------------