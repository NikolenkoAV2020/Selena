// LibNmapScan.h

#ifndef LIB_NMAP_HEADER
#define LIB_NMAP_HEADER

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

// Execute Nmap scaning as function
int LaunchNmap(
	const char* Command,	// Nmap command.
	const char* JobName,	// Name of job for create output file name.
	const char* DirName);	// Directory name for results.

int DnsReverse(
	const char*	ipAddr,
	char*		domaneName,
	int			nameLen);

#include <string>

#ifdef _LIB
	void MessageToServer(
		const std::string message,
		const std::string fileMsg);
#endif

void GetNmapGlobalMessage(std::string& msg);
void GetNmapGlobalWarning(std::string& war);
void GetNmapGlobalFatal  (std::string& fat);
void GetNmapGlobalState  (bool& isFatal, bool& isMsg, bool& isWar);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef LIB_NMAP_HEADER
