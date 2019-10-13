// NetWinUnixCompat.cpp 
// Walter Artyomenko, August 2019
//
#include <NetWinUnixCompat.h>
#include <ConsolColor.h>

#include <ctime>
#include <time.h>

void SetWindowName(const char* name)
{
	std::cout << "\033]0;" << name << "\007";
	std::cout << "\x1b[7l";
}

#if defined(__linux__)
// For Unix, Linox, and so on...
//------------------------------------------------------------------------------
int GetSocketError() { 
	return errno; 
}

bool IsNetDisconnectError(int r)
{
	switch (r) {
		case ENETRESET:
		case ENETUNREACH:
		//case WSAENETRESET:
		//case WSAECONNABORTED:
		//case WSAECONNRESET:
			return true;
	}
	return false;
}

int GetSocketErrorText(char* errorText, int maxSize) {
	if (!maxSize || !errorText) return-1;
	errorText[0] = '\0';
	char* reterr = strerror_r(errno, errorText, maxSize); reterr = reterr;
	return errno;
}

int SocketsSwitchOn(bool coutEcho) {
	return 0; 
}

int SocketsSwitchOff(void) { 
	return 0; 
}

//------------------------------------------------------------------------------
int  OpenSocket(SOCKET& idSocket) {
	idSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (idSocket == INVALID_SOCKET || idSocket == SOCKET_ERROR)
		return SOCKET_ERROR;
	return 0;
}

//------------------------------------------------------------------------------
int CloseSocket(SOCKET& idSocket) { 
	if (!idSocket) return 0;
	int r =close(idSocket);
	if (!r) idSocket = 0;
	return r; 
}

//------------------------------------------------------------------------------
int BytesInSocket(SOCKET idSocket, unsigned int& bytes)
{
	size_t bytes_t = 0;
	int r = ioctl(idSocket, FIONREAD, (char*)&bytes_t);
	bytes = !r ? (unsigned int)bytes_t : 0;
	return r;
}

//------------------------------------------------------------------------------
int BuilUlongSaddr(unsigned int famaly, const char* ipString, struct in_addr* addr){
	return(inet_aton(ipString, addr));
}

#else
// For Windows only
//------------------------------------------------------------------------------
#pragma message("		Connectin ... ws2_32.lib")
#pragma comment(lib, "ws2_32.lib")

//------------------------------------------------------------------------------
int GetSocketError() { 
	return WSAGetLastError(); 
}

bool IsNetDisconnectError(int r)
{
	switch (r) {
	case WSAENETDOWN:
	case WSAENETUNREACH:
	case WSAENETRESET:
	case WSAECONNABORTED:
	case WSAECONNRESET:
		return true;
	}
	return false;
}

int GetSocketErrorText(char* S, int Slen) {
	if (!S || !Slen) return-1;
	S[0] = '\0';

	int r = GetSocketError();

	switch (r) {
	case WSAENETDOWN:
		sprintf_s(S, Slen, "A socket operation encountered a dead network.");
		//sprintf_s(S, Slen, "The network subsystem has failed.\n\0");
		break;
	case WSAENETUNREACH:
		sprintf_s(S, Slen, "A socket operation was attempted to an unreachable network.");
		//sprintf_s(S, Slen, "The network cannot be reached from this host at this time.\n");
		break;
	case WSAENETRESET:
		sprintf_s(S, Slen, "The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.");
		break;
	case WSAECONNABORTED:
		sprintf_s(S, Slen, "An established connection was aborted by the software in your host machine.");
		break;
	case WSAECONNRESET:
		sprintf_s(S, Slen, "An existing connection was forcibly closed by the remote host.");
		break;
	case WSASYSNOTREADY:
		sprintf_s(S, Slen, "The underlying network subsystem is not ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED:
		sprintf_s(S, Slen, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
		break;
	case WSAEPROCLIM:
		sprintf_s(S, Slen, "A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
		break;
	case WSANOTINITIALISED:
		sprintf_s(S, Slen, "A successful WSAStartup call must occur before using this function.\n\0");
		break;
	case WSAEACCES:
		sprintf_s(S, Slen, "An attempt was made to access a socket in a way forbidden by its access permissions.\n\0");
		break;
	case WSAEADDRNOTAVAIL:
		sprintf_s(S, Slen, "The requested address is not valid in its context.\n\0");
		break;
	case WSAEFAULT:
		sprintf_s(S, Slen, "The system detected an invalid pointer address in attempting to use a pointer argument in a call.\n\0");
		break;
	case WSAEINVAL:
		sprintf_s(S, Slen, "An invalid argument was supplied.\n\0");
		break;
	case WSAENOBUFS:
		sprintf_s(S, Slen, "An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.\n\0");
		break;
	case WSAEADDRINUSE:
		sprintf_s(S, Slen, "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR\n");
		break;
	case WSAEINTR:
		sprintf_s(S, Slen, "The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n");
		break;
	case WSAEINPROGRESS:
		sprintf_s(S, Slen, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n");
		break;
	case WSAEALREADY:
		sprintf_s(S, Slen, "A nonblocking connect call is in progress on the specified socket.\n");
		break;
	case WSAEAFNOSUPPORT:
		sprintf_s(S, Slen, "Addresses in the specified family cannot be used with this socket.\n");
		break;
	case WSAECONNREFUSED:
		sprintf_s(S, Slen, "The attempt to connect was forcefully rejected.\n");
		break;
	case WSAEISCONN:
		sprintf_s(S, Slen, "The socket is already connected (connection-oriented sockets only).\n");
		break;
	case WSAEHOSTUNREACH:
		sprintf_s(S, Slen, "A socket operation was attempted to an unreachable host.\n");
		break;
	case WSAENOTSOCK:
		sprintf_s(S, Slen, "The descriptor specified in the s parameter is not a socket.\n");
		break;
	case WSAETIMEDOUT:
		sprintf_s(S, Slen, "An attempt to connect timed out without establishing a connection.\n");
		break;
	case WSAEWOULDBLOCK:
		sprintf_s(S, Slen, "The socket is marked as nonblocking and the connection cannot be completed immediately.\n");
		break;
	default:
		sprintf_s(S, Slen, "Unknow error\n");
		break;
	}

	return r;
}

//------------------------------------------------------------------------------
int SocketsSwitchOn(bool coutEcho) {
	WSADATA wsaData;
	WORD	wVersionRequested = MAKEWORD(2, 2);
	// wVersionRequested = 0x0202;
	int r = WSAStartup(wVersionRequested, &wsaData);
	if (!r) {
		// Confirm that the WinSock DLL supports 2.2.
		// Note that if the DLL supports versions greater    
		// than 2.2 in addition to 2.2, it will still return 
		// 2.2 in wVersion since that is the version we      
		// requested. 
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
			// Tell the user that we could not find a usable
			// WinSock DLL.   
			std::cout
				<< SELENA_CCLR_SERV_ERR
				<< "Could not find a usable version of Winsock.dll"
				<< SELENA_CCLR_SERV_DEF << std::endl;
			WSACleanup();
			return 1;
		}
		return 0;
	}

	std::cout 
		<< SELENA_CCLR_SERV_ERR
		<< "Error WSAStartup " << GetSocketError()
		<< SELENA_CCLR_SERV_DEF << std::endl;
	char errorText[256];
	r = GetSocketErrorText(errorText, 256);
	if (r>0) 
		std::cout 
			<< SELENA_CCLR_SERV_ERR 
			<< errorText 
			<< SELENA_CCLR_SERV_DEF << std::endl;

	return -1;
}

//------------------------------------------------------------------------------
int SocketsSwitchOff(void) { 
	WSACleanup(); 
	return 0;
}

//------------------------------------------------------------------------------
int  OpenSocket(SOCKET& idSocket) {
	// С каждым сокетом связываются три атрибута : домен, 
	// тип и протокол.Эти атрибуты задаются при создании сокета и 
	// остаются неизменными на протяжении всего времени его существования.
	// Для создания сокета используется функция socket, имеющая следующий 
	// прототип.
	idSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Проверка необходимости инициализации библиотеки сокетов
	if (idSocket == INVALID_SOCKET && GetSocketError() == WSANOTINITIALISED) {
		// Если при создании сокета выяснилось что необходима инициалзация
		// системной библиотеки, то выполняем инициализацию...
		// В Windows библиотеку Winsock необходимо явно проинициализировать 
		// до обращения к любым другим функциям из неё
		int r = SocketsSwitchOn(false);
		if (r) return SOCKET_ERROR;
		// После чего повторно пытаемся создать сокет
		idSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	if (idSocket == INVALID_SOCKET || idSocket == SOCKET_ERROR)
		return SOCKET_ERROR;

	return 0;
}

//------------------------------------------------------------------------------
int CloseSocket(SOCKET& idSocket) { 
	if (!idSocket) return 0;
	int r = closesocket(idSocket); 
	if (!r) idSocket = 0;
	return r;
}

//------------------------------------------------------------------------------
int BytesInSocket(SOCKET idSocket, unsigned int& bytes)
{
	u_long arg = 0;
	int r = ioctlsocket(idSocket, FIONREAD, &arg);
	bytes = !r ? (unsigned int)arg : 0;
	return r;
}

//------------------------------------------------------------------------------
int BuilUlongSaddr(unsigned int famaly, const char* ipString, struct in_addr* addr) {
	#if _WIN32_WINNT <= 0x0501
		//for XP
		addr->s_addr = inet_addr(ipString);
		return 1;
	#else
		//for Vista or higher
		int r = inet_pton(famaly, ipString, &(addr->s_addr));
		return r;
	#endif
}

#endif // #if defined(__linux__)

//------------------------------------------------------------------------------
int HandleSocketError(const char* Func, const char* err, SOCKET idSocket, bool coutEcho)
{
	if (err && strlen(err) > 0) {
		std::cout
			<< SELENA_CCLR_SERV_ERR
			<< "Error "
			<< Func << ": " << err << " Code - " << GetSocketError()
			<< SELENA_CCLR_SERV_DEF << std::endl;
	}
	else {
		std::cout
			<< SELENA_CCLR_SERV_ERR
			<< "Error "
			<< Func << ". Code - " << GetSocketError()
			<< SELENA_CCLR_SERV_DEF << std::endl;
	}

	char S[512];
	if (GetSocketErrorText(S, 512) > 0 && strlen(S) > 0)
		std::cout
			<< SELENA_CCLR_SERV_ERR 
			<< "      " << S
			<< SELENA_CCLR_SERV_DEF << std::endl;

	if (idSocket > 0) CloseSocket(idSocket);

	SocketsSwitchOff();
	return -1;
}

//-----------------------------------------------------------------------------
// For substitute Linux function clock_gettime in Windows
// It makes sense for only the windows!

#ifdef defWINDOWS

#include <windows.h>

int clock_gettime(int, struct timespec *t)
{
	__int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= 116444736000000000i64;			//1jan1601 to 1jan1970
	t->tv_sec = wintime / 10000000i64;         //seconds
	t->tv_nsec = wintime % 10000000i64 * 100;   //nano-seconds
	return 0;
}

#else
	// In Linux clock_gettime is native function

void Sleep(int microseconds)
{
	usleep(microseconds);
}

#endif // #ifdef defWINDOWS

//------------------------------------------------------------------------------
int LocalTimeInfo(struct tm* Tinf, struct timespec* pspec)
{
	struct timespec spec;
	clock_gettime(0, &spec);
	if (pspec) memcpy(pspec, &spec, sizeof(spec));

	time_t seconds = (time_t)spec.tv_sec;

#ifdef defWINDOWS
	localtime_s(Tinf, &seconds);
#else
	struct tm* timeinfo = localtime(&seconds);
	memcpy(Tinf, timeinfo, sizeof(struct tm));
#endif

	return 0;
}

//------------------------------------------------------------------------------