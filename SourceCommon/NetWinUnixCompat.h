// NetWinUnixCompat.h
// Walter Artyomenko, August 2019
//
// Here collected all macros, functions redefinition,platform dependent 
// header files for compile source code under different OS 
// without any changes.
// 
#ifndef NET_WIN_UNIC_COMATABLE_H
#define NET_WIN_UNIC_COMATABLE_H

#include <iostream>
#include <fstream> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>  

#define AURORA_PORT 50555

#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)
#	define defWINDOWS 1
#else
#	undef defWINDOWS
#endif // #if defined(WIN32) ...

// platform depended including & difinition
#ifdef defWINDOWS
	// for Windows only
#	include <winsock2.h>
#	include <windows.h>
#	include <WS2tcpip.h>
#	include <direct.h>

#	define IsValiPTread(thread)	((thread).p)
#	define PThreadInit				{nullptr, 0}
#	define socklen_t				int 
#	define ssize_t					int 	

#elif defined(__linux__)
	// For Unix, Linux and so on...
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/socket.h>
#	include <sys/socket.h>
#	include <sys/ioctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#	include <termios.h>
#	include <dirent.h>

// Features definition that there are in Windows 
// but absent in Linux
#	define IsValiPTread(thread) ((thread))
#	define INVALID_SOCKET	-1
#	define SOCKET_ERROR		-1
#	define SOCKET			int
#	define HOSTENT			hostent
#	define sprintf_s		snprintf
#	define strtok_s			strtok_r
#	define PThreadInit		0
#else						// #if defined(__linux__)
	// anything other...
#	error Compilation for unknown platform.		
#endif						// #ifdef WINDOWS

#define stdsize		std::string::size_type

#ifndef nullptr
// Before C++ 2011 used NULL fol void pointer. 
// In further appling nullptr macros. Now, 
// I insert next definition for compatible source code
#	define nullptr NULL
#endif

void SetWindowName(const char* name);

int SocketsSwitchOn (bool coutEcho = true);
int SocketsSwitchOff(void);

int  GetSocketError      ();
int  GetSocketErrorText  (char*  errorText, int maxSize = 256);
bool IsNetDisconnectError(int r);
int  HandleSocketError   (const char* Func, const char* err, 
						 SOCKET idSocket, bool coutEcho = true);

int OpenSocket    (SOCKET& idSocket);
int CloseSocket   (SOCKET& idSocket);
int BytesInSocket (SOCKET  idSocket, unsigned int& bytes);
int BuilUlongSaddr(unsigned int famaly, const char* ipString,
				   struct in_addr* addr);

#ifdef defWINDOWS
	int  clock_gettime(int, struct timespec *t);
#else
	void Sleep(int microseconds);
#endif

int  LocalTimeInfo(struct tm* Tinf, struct timespec* spec = nullptr);


//------------------------------------------------------------------------------
#endif // #ifndef NET_WIN_UNIC_COMATABLE_H
