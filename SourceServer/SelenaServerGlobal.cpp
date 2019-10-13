// SelenaServerGlobal.cpp
// Walter Artyomenko, August 2019
//
#include <NetWinUnixCompat.h>
#include <SelenaJob.h>
#include <SelenaStrings.h>
#include <SelenaSafeMessage.h>
#include <SelenaThreads.h>
#include "SelenaServerGlobal.h"

//-----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)
	// For Windows only

#	include <Windows.h>
#	include <Windns.h>

#	ifdef NDEBUG
#		pragma message("		Connectin ... pthread.lib")
#		pragma comment(lib, "pthread.lib")
#	else
#		pragma message("		Connectin ... pthreadD.lib")
#		pragma comment(lib, "pthreadD.lib")
#	endif // #ifdef NDEBUG

#	pragma message("		    Now we can use linux-like threads,")
#	pragma message("		    i.e. pthreads-win32 - POSIX Threads Library for Win32.")
#	pragma message("		    It's an implementation of the threads API of POSIX 1003.1-2001.")
#	pragma message("		    Such approach is well suited for future portability to Linux.")

#	if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)
#		define NMAP_LIB			"nmapSD.lib            "
#		define MASSCAN_LIB		"MasScanSD.lib         "
#		define NBASE_LIB		"nbaseSD.lib           "
#		define NSOCK_LIB		"nsockSD.lib           "
#		define LIBLUA_LIB		"libluaSD.lib          "
#		define LIBLINEAR_LIB	"liblinearSD.lib       "
#		define LIBPCRE_LIB		"libpcreSD.lib         "
#		define LIBDNET_LIB		"libdnet-strippedSD.lib"
#		define LIBNETUTIL_LIB	"libnetutilSD.lib      "
#		define LIBSSH2_LIB		"libssh2D.lib         "
#		define ZLIBWAPI_LIB		"zlibwapiD.lib        "
#	else
#		define NMAP_LIB			"nmapS.lib             "
#		define MASSCAN_LIB		"MasScanS.lib          "
#		define NBASE_LIB		"nbaseS.lib            "
#		define NSOCK_LIB		"nsockS.lib            "
#		define LIBLINEAR_LIB	"liblinearS.lib        "
#		define LIBLUA_LIB		"libluaS.lib           "
#		define LIBPCRE_LIB		"libpcreS.lib          "
#		define LIBDNET_LIB		"libdnet-strippedS.lib "
#		define LIBNETUTIL_LIB	"libnetutilS.lib       "
#		define LIBSSH2_LIB		"libssh2.lib          "
#		define ZLIBWAPI_LIB		"zlibwapi.lib         "
#	endif // if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)

#	define WPCAP_LIB		"Npcap/wpcap.lib          "
#	define PACKET_LIB		"Npcap/packet.lib         "
#	define LIBEAY_LIB		"OpenSSL/libeay32.lib     "
#	define SSLEAY_LIB		"OpenSSL/ssleay32.lib     "
#	define IPHIPAPI_LIB		"IPHlpAPI.lib             "
#	define DELAYIMP_LIB		"delayimp.lib             "

#	pragma message("		Connection to the program Nmap as static library whith anll dependences.")
#	pragma message("		Connectin ... " NMAP_LIB)
#	pragma comment (lib, NMAP_LIB)

#	pragma message("		Connectin ... " MASSCAN_LIB)
#	pragma comment (lib, MASSCAN_LIB)

#	pragma message("		Connectin ... " NBASE_LIB)
#	pragma comment (lib, NBASE_LIB)

#	pragma message("		Connectin ... " LIBLINEAR_LIB)
#	pragma comment (lib, LIBLINEAR_LIB)

#	pragma message("		Connectin ... " LIBLUA_LIB)
#	pragma message("		              For LUA scripts use in Nmap program")
#	pragma comment (lib, LIBLUA_LIB)

#	pragma message("		Connectin ... " NSOCK_LIB)
#	pragma message("		              Nmap sockets wrapper")
#	pragma comment (lib, NSOCK_LIB)

#	pragma message("		Connectin ... " LIBPCRE_LIB)
#	pragma comment (lib, LIBPCRE_LIB)

#	pragma message("		Connectin ... " LIBDNET_LIB)
#	pragma message("		              Stripped \"dnet\" Linux library portated from Linux to Windows")
#	pragma comment (lib, LIBDNET_LIB)

#	pragma message("		Connectin ... " LIBNETUTIL_LIB)
#	pragma comment (lib, LIBNETUTIL_LIB)

#	pragma message("		Connectin ... " LIBSSH2_LIB)
#	pragma message("		              For employment of security shell  technology")
#	pragma comment (lib, LIBSSH2_LIB)

#	pragma message("		Connectin ... " ZLIBWAPI_LIB)
#	pragma message("		              For data compression")
#	pragma comment (lib, ZLIBWAPI_LIB)

#	pragma message("		Connectin ... ws2_32.lib")
#	pragma message("		              For employment of sockets technology")
#	pragma comment (lib, "ws2_32.lib")

#	pragma message("		Connectin ... " WPCAP_LIB)
#	pragma message("		              For use packet capture library")
#	pragma comment (lib, WPCAP_LIB)

#	pragma message("		Connectin ... " PACKET_LIB)
#	pragma comment (lib, PACKET_LIB)

#	pragma message("		Connectin ... " LIBEAY_LIB)
#	pragma message("		              For use SSL librarys")
#	pragma comment (lib, LIBEAY_LIB)

#	pragma message("		Connectin ... " SSLEAY_LIB)
#	pragma comment (lib, SSLEAY_LIB)

#	pragma message("		Connectin ... " IPHIPAPI_LIB)
#	pragma comment (lib, IPHIPAPI_LIB)

#	pragma message("		Connectin ... " DELAYIMP_LIB)
#	pragma message("		              For delayous loading dll")
#	pragma comment (lib, DELAYIMP_LIB)

#else
	// For Linux & so on...


#endif // #if defined(WIN32) ...

//-----------------------------------------------------------------------------
// Хранилище команд сервера
ThrSfSelenaMessages	serverCom;
// Хранилище сообщений клиента
ThrSfSelenaMessages	clientMsg;
// Глобальный сигнал о неообходимости завершения серверного приложения
ThrSfBool			stopSignal;
ThrSfBool			reconnectSignal;

//-----------------------------------------------------------------------------