// SelenaJob.cpp
// Walter Artyomenko, September 2019
//

#include <string>
#include <iostream>
#include <algorithm>

#include <NetWinUnixCompat.h>
#include <SelenaStrings.h>
#include <SelenaJob.h>

//-----------------------------------------------------------------------------
int SelenaJob::SpellOutCommand()
{
	scanChain.clear();

	SelenaScanCommand C;
	std::string       s, w;

	int i, n = command.length();
	for (i = 0; i < n; ++i) {
		char ch = command[i];
		if (ch == ';' || i == n - 1) {
			if (s.length()) {
				if (ch != ';') s += ch;
				C.params = s;
				if (CommandGetFirstWord(s, w) > 0) {
					if (w == "masscan")	C.tool = 0;
					else if (w == "nmap") C.tool = 1;
						 else C.tool = -1;
				} else C.tool = -1;
 				scanChain.push_back(C);
			}
			s = "";
			if (i == n - 1) break;
			if (ch == ';') {
				i++;
				continue;
		}	}
		s += ch;
	}

	return 0;
}

int SelenaJob::CountCommand()
{
	return ((int)scanChain.size());
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::StorageDir(std::string& d)
{
	if (!dirName.length()) dirName = ".";
	else {
#if defined(__linux__)
		mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
		_mkdir(dirName.c_str());
#endif
	}
	d = dirName;
	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::FullStorageName(std::string& fileName, std::string& fullName)
{
	std::string sn;
	StorageDir(sn);
	sn += "/";
	sn += fileName;
	fullName = sn;
	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::MakeUniqueName(
	const char* rootName,
	std::string& jobName)
{
	char fileName[128];
 
	struct timespec spec;
	struct tm       timeinfo;
	LocalTimeInfo(&timeinfo, &spec);

	char buffer[80];
	char format[] = "%Y%m%d_%H%M%S";
	strftime(buffer, 80, format, &timeinfo);

	double ms = ((double)spec.tv_nsec)/1e6;

	snprintf(fileName, 128, "%s%s%03d", rootName, buffer, (int)ms);
	jobName  = fileName;

	Sleep(1);

	return 0;
}

//-----------------------------------------------------------------------------
void SelenaJobsManager::WriteNameString(std::ofstream& f, std::string& name)
{
	int n = name.length();
	f.write((char*)&n, sizeof(int));
	if (n) f.write(name.c_str(), n);
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::StorageJ(SelenaJob& J)
{
	std::string sn;

	if (!J.storageName.length()) {
		MakeUniqueName("SJ", J.storageName);
		// If the command contains a file with addresses
		if (J.fileInputName.length()) {
			FullStorageName(J.storageName, sn);
			sn += ".jobi";
			// Rename origin input file to storage input file
			rename(J.fileInputName.c_str(), sn.c_str());
			// Delete origin input file
			remove(J.fileInputName.c_str());
			// In fact, we create a new input file and now we need 
			// to edit the network scan command
			ReplaceStdSubstr(J.command, J.fileInputName, sn);
		}
	}

	sn =  (J.stage ? (J.stage == 100 ? "__" : "_") : "") + J.storageName;
	FullStorageName(sn, sn);
	sn+= ".job";

	std::ofstream f(sn, std::ios::out | std::ios::binary);
	if (!f.is_open()) return -1;

	f.write(reinterpret_cast<char*>(&J.stage), sizeof(J.stage));
	f.write(reinterpret_cast<char*>(&J.completCode), sizeof(J.completCode));
	
	J.h.sizeCommad = (unsigned short)J.command.length();
	f.write(reinterpret_cast<char*>(&J.h), sizeof(J.h));

	WriteNameString(f, J.command);
	WriteNameString(f, J.fileInputName);
	WriteNameString(f, J.fileOutputName);
	WriteNameString(f, J.errorMessage);
	
	return 0;
}

//-----------------------------------------------------------------------------
void SelenaJobsManager::ReadNameString(std::ifstream& f, char** buf, int& lenbuf, std::string& name)
{
	int n = 0;
	f.read((char*)&n, sizeof(int));
	if (n) {
		if (n > lenbuf) {
			delete[] (*buf);
			lenbuf = n + 1;
			(*buf) = new char[lenbuf];
		}
		f.read((*buf), n);
		(*buf)[n] = '\0';
		name = (*buf);
	}
	else name = "";
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::RestoryJ(SelenaJob& J, char stage)
{
	if (!J.storageName.length()) return -1;

	std::ifstream f;
	std::string   sn;

	if (stage == 1) goto m_current;
	if (stage == 2) goto m_results;

	FullStorageName(J.storageName, sn);
	sn += ".job";

	// RU. ѕопытка чтени€ задани€ на выполнение работы.
	// EN. Attempt to read job assignment.
	f.open(sn, std::ios::out | std::ios::binary);
	if (!f.is_open()) {
m_current:;
		// RU. ѕопытка чтени€ текущей работы.
		// EN. Attempt to read current job.
		sn = ("_" + J.storageName);
		FullStorageName(sn, sn);
		sn += ".job";
		f.open(sn, std::ios::out | std::ios::binary);
		if (!f.is_open()) {
m_results:;
			// RU. ѕопытка чтени€ результатов выполненной работы.
			// EN. An attempt to read the results of the  performed work
			sn = ("__" + J.storageName);
			FullStorageName(sn, sn);
			sn += ".job";
			f.open(sn, std::ios::out | std::ios::binary);
	}	}
	if (!f.is_open()) return -1;

	SelenaJob JR;
	JR.storageName = J.storageName;

	f.read(reinterpret_cast<char*>(&JR.stage), sizeof(J.stage));
	f.read(reinterpret_cast<char*>(&JR.completCode), sizeof(J.completCode));

	f.read(reinterpret_cast<char*>(&JR.h), sizeof(J.h));

	char* buf = nullptr;
	int   n = 0;
	ReadNameString(f, &buf, n, JR.command);
	ReadNameString(f, &buf, n, JR.fileInputName);
	ReadNameString(f, &buf, n, JR.fileOutputName);
	ReadNameString(f, &buf, n, JR.errorMessage);

	J = JR;

	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::RemoveJ(SelenaJob& J)
{
	if (!J.storageName.length()) return -1;

	std::string fn;
	FullStorageName(J.storageName, fn);
	std::string sn = fn;
	sn += ".job";
	remove(sn.c_str());

	sn = "_";
	sn+= J.storageName;
	FullStorageName(sn, sn);
	sn+= ".job";
	remove(sn.c_str());

	sn = "__";
	sn += J.storageName;
	FullStorageName(sn, sn);
	sn += ".job";
	remove(sn.c_str());

	sn = fn;
	sn += "jobr";
	remove(sn.c_str());

	sn = fn;
	sn += "jobi";
	remove(sn.c_str());

	return 0;
}

//-----------------------------------------------------------------------------
bool SelenaJobsManager::IsStrEqualMask(char* s, char* p)
{
	char *rp = 0; // последн€€ проверенна€ звездочка
	char *rs = 0; // часть строки, котора€ относитс€ к найденой звездочке
	while (1) {
		if (*p == '*')	rs = s, rp = ++p;
		else if (!*s) return !*p;
			 else if (*s == *p || *p == '?') ++s, ++p;
				  else if (rs) s = ++rs, p = rp;
					   else	return false;
	}
	return false;
}

int SelenaJobsManager::GetFirstJob(SelenaJob& J)
{
	std::vector<std::string> FL;
	GetJobsList(FL);
	if (!FL.size()) return-1;
	J.storageName = FL[0];
	return(RestoryJ(J));
}

int SelenaJobsManager::GetFirstRes(SelenaJob& J)
{
	std::vector<std::string> FL;
	GetJobsList(FL, true);
	if (!FL.size()) return-1;
	J.storageName = FL[0];
	J.storageName.erase(J.storageName.begin(), J.storageName.begin() + 2);
	return(RestoryJ(J, 2));
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::GetJobsCount()
{
	std::vector<std::string> JL;
	GetJobsList(JL);
	return JL.size();
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::GetResultsCount()
{
	std::vector<std::string> JL;
	GetJobsList(JL, true);
	return JL.size();
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::GetFileList(
		std::vector<std::string>&	FL, 
		const char*					templFile,
		bool						excludeExt)
{
	std::string dirName;
	StorageDir(dirName);

#ifdef defWINDOWS
	WIN32_FIND_DATA wfd;
	std::string templ = dirName;
	templ += "/";
	templ += templFile;
	HANDLE const hFind = FindFirstFile(templ.c_str(), &wfd);

	if (INVALID_HANDLE_VALUE != hFind) {
		do {
			if (!strcmp(wfd.cFileName, ".") ||
				!strcmp(wfd.cFileName, "..") ||
				(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
				(wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
				continue;
			if (excludeExt) {
				int n = strlen(wfd.cFileName);
				while (--n > 0) {
					if (wfd.cFileName[n] == '.') {
						wfd.cFileName[n] = '\0';
						break;
				}	}
			}
			FL.push_back(wfd.cFileName);
		} while (NULL != FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}
#else
	DIR				*dp;
	struct dirent	*dent;

	if (!(dp = opendir((char*)dirName.c_str()))) return 1;
	while ((dent = readdir(dp))) {
		if (strcmp(".", dent->d_name) && strcmp("..", dent->d_name))
			if (IsStrEqualMask(dent->d_name, (char*)templFile)) {
				if (excludeExt) {
					int n = strlen(dent->d_name);
					while (--n > 0) {
						if (dent->d_name[n] == '.') {
							dent->d_name[n] = '\0';
							break;
					}	}
				}
				FL.push_back(dent->d_name);
			}
	}
	closedir(dp);
#endif

	sort(FL.begin(), FL.end());
	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::GetJobsList(
		std::vector<std::string>&	JL, 
		bool						forRes)
{
	const char* templFile = forRes ? "__SJ*.job" : "SJ*.job";
	return GetFileList(JL, templFile);
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::CreateResultFile(SelenaJob& J)
{
	std::string name;
	FullStorageName(J.storageName, name);
	name += ".jobr";

	// Create file for write results
	std::ofstream f(name, std::ios::out);
	f.close();

	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::TakeToWork(SelenaJob& J)
{
	J.stage = 1;
	StorageJ(J);

	std::string name;
	FullStorageName(J.storageName, name);
	name += ".job";
	remove(name.c_str());

	CreateResultFile(J);

	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::OutputOfWork(SelenaJob& J)
{
	J.stage = 100;
	StorageJ(J);

	std::string name("_" + J.storageName + ".job");
	FullStorageName(name, name);
	remove(name.c_str());

	return 0;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::TakeToWorkFirst(SelenaJob& J)
{
	if (!GetFirstJob(J)) {
		return (TakeToWork(J));
	}
	return-1;
}

//-----------------------------------------------------------------------------
int SelenaJobsManager::PutCommandToJobStorage(
	std::string& command,	// Command received from server
	std::string& comFile)	// File containing destination IP addresses
{
	// ‘ормирование задание на выполнение работы
	SelenaJob J;
	J.h.sizeCommad     = (unsigned short)command.length();
	J.h.targetCount    = 1;
	J.h.fileInputSize  = 0;
	J.h.fileOutputSize = 0;
	J.h.varScan        = 1;

	J.command       = command;
	J.fileInputName = comFile;
	J.errorMessage  = "";
	J.completCode   = 0;

	CommandFindOutFileName(command, J.fileOutputName);
	if (!J.fileOutputName.length()) {
		J.fileOutputName = "Output";
		J.fileOutputName += comFile;
	}

	StorageJ(J);

	return 0;
}
//-----------------------------------------------------------------------------