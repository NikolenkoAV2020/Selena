// SelenaSocket.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_SOCKET_HEADER
#define SELENA_SOCKET_HEADER

#include <NetWinUnixCompat.h>
#include <GetIPBeadRoll.h>
#include <SelenaJob.h>
#include <SelenaGlobal.h>
#include <SelenaSafeMessage.h>

#define SELENA_HEADER_KEY	"Selena.Size\0"
#define SELENA_BEG_FILE_KEY	"beginning of file\0"
#define SELENA_END_FILE_KEY	"end of file\0"
#define SELENA_READBUFFSIZE 4096

int GetDomainNameForIp(
	const char*  ip, 
	std::string& domaneName,
	std::vector<std::string>* alias = nullptr);
int GetIpForDomainName(
	const char*  domaneName, 
	std::string& ip,
	std::vector<std::string>* alias = nullptr);

#pragma pack(push, 1)
struct ZSelenaMsgHeader {
	// Key phrase
	char     Key[24];		
	// Text message size.
	// If it is equal to zero, then there is 
	// no text in the message and only file transfers take place.
	uint32_t Size = 0;		
	// File attached size.
	// If equal to zero, then there is no file in the message.
	// If both Size = 0 and fileSize = 0,
	// there is an empty message.
	uint32_t fileSize = 0;	
	// Message type (values as SelenaMsgType)
	uint8_t  type = 0;		
	// Is it previously pending message
	bool     isPending = false;

	ZSelenaMsgHeader(
		unsigned int  ms  = 0, 
		unsigned int  fs  = 0,
		SelenaMsgType msgType = smtText,
		bool          isp = false)
	{
		memcpy(Key, SELENA_HEADER_KEY, 12); 
		Size      = ms;
		fileSize  = fs;
		type      = (uint8_t)msgType;
		isPending = isp;
	}
	bool Verify() {			// Validation of a key phrase.
		int r = strcmp(Key, SELENA_HEADER_KEY);
		return(r? false : true);
	}
	bool IsInf() { return (type == 0); }
	bool IsAns() { return (type == 1); }
	bool IsWar() { return (type == 2); }
	bool IsErr() { return (type == 3); }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SelenaMassege {
	ZSelenaMsgHeader h;
	std::string      s;
	std::string      fn;
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
// !!!! Type of ZSelenaSocket !!!!
//
// A program using a socket will either listen the socket and 
// wait for an incoming connection - 
// - this type of connection is called "sstListening", 
// or it will knock on the specified hostname or IP address itself -
// - "sstKnocking" type of connection.
//-----------------------------------------------------------------------------
enum ZSelenaSocketType {
	sstDefault   = 0,	// Default type
	sstKnocking  = 1,	// Like client
	sstListening = 2	// Like server
};

void* FuncConnectThread(void* C);

//-----------------------------------------------------------------------------
// The main class implements the exchange of messages, commands and files 
// between the client and server.
class ZSelenaSocket {
public:
	IpBeadRoll	 aboutIp;
	// Echo all messages & commands to consol
	bool		 coutEcho = true;
	// Mark of disconnection.
	// Established in true if obtain any tcp connection error 
	// during read/write operations.
	bool		 isDisconnectError = false;

protected:
	// own properties
	SOCKET		 ownSocket   = 0;
	unsigned int ownPort     = 0;
	std::string	 ownHostName = "";
	std::string	 ownHostIp   = "";
	std::string  inputFileDefaultName = "SelenaInput";

	// subscriber properties
	SOCKET		 subSocket   = 0;
	unsigned int subPort     = 0;
	std::string	 subHostName = "";
	std::string	 subHostIp   = "";

	// 0 - Server
	// 1 - Client
	// if ownVar = 0 then application behavior himself like server 
	// and listen a port and waiting some connection.
	// if ownVar = 1 then application similar to client application 
	// and knock to given host and port to establish connection
	char ownVar = 0;

	ZSelenaSocketType sockType = sstKnocking;

public:
	ZSelenaSocket (ZSelenaSocketType type = sstKnocking, char ownType = 0);
	~ZSelenaSocket();

	int DisConnect();
	int Connect   (ZSelenaSocketType type = sstDefault);
	int ReConnect ();
	// Hot reconnection of socket, restore corrupted connection without 
	// terminate main thread and scanning procedure.
	// This is an essential for Selena project feature of socket. 
	// If somthings wrong happen in tcp-connection by any reason Selena 
	// workflow must don't stop, and if it's possible connection must 
	// be rocovery in shadow mode.
	int HotReConnect();

	// Is redy socket to write/read data
	bool IsReady(SOCKET readerSocket = 0);

private:
	int JustConnect(ZSelenaSocketType type = sstDefault);
	int ConnectLikeServer();
	int ConnectLikeClient();

public:
	int ConnectByHostName(const char* hostName, unsigned int port);
	int ConnectByHostIp(const char* hostIp, unsigned int port);

	SOCKET		 GetSocket  () const { return ownSocket; }
	const char*  GetHostName() const { return ownHostName.c_str(); }
	const char*  GetHostIp  () const { return ownHostIp.c_str(); }
	unsigned int GetHostPort() const { return ownPort; }

	SOCKET		 GetSubSocket  () const { return subSocket; }
	const char*  GetSubHostName() const { return subHostName.c_str(); }
	const char*  GetSubHostIp  () const { return subHostIp.c_str(); }
	unsigned int GetSubHostPort() const { return subPort; }

	int SendMessageToHost(const char* message, unsigned int len);
	// number of bytes waiting to be read in the input stream
	unsigned int WaitingBytes();
	// Is there anything in the input stream
	int IsThereIncoming();

	bool IsKeepAlive(SOCKET srcSocket = 0);

	int  ByteForRead(unsigned int& bytes, SOCKET srcSocket = 0);
	int  ByteWrited (unsigned int& bytes, SOCKET srcSocket = 0);

	bool IsThereAnyRead(SOCKET srcSocket = 0);

	int  ReadIncoming (ZSelenaMessage& M);
	int  WriteOutgoing(ZSelenaMessage& M);

	int  NagleOff (SOCKET srcSocket = 0);
	int  NagleOn  (SOCKET srcSocket = 0);
	bool IsNagleOn(SOCKET srcSocket = 0);

	int  ReadMsg (ZSelenaMessage& M, SOCKET srcSocket = 0);
	int  WriteMsg(ZSelenaMessage& M, SOCKET srcSocket = 0);

protected:
	int CreateSocket();
	int WhoIsThere ();
	int HandleError(const char* Func, const char* err = nullptr);
	int PreReadFile(const std::string& fileName, unsigned int& fileSize);

	SOCKET RWS() { 
		return (sockType == sstKnocking ? ownSocket : subSocket); 
	}

private :
	// Elementary operation of sending data to a socket
	int SelfSend(SOCKET &socket, const char* M, int needSend);
	int SelfSend(SOCKET &socket, std::string& M);

	// So ... let's write "flow connector" for shadow (background)
	// established tcp-connection
	class ConnectThread {
	public:
		// Connection thread, it feature need for hot reconnection 
		// that execution in apart thread.
		// It as well as use for deprecate of repeatedly socket 
		// connection when connection thread is already launch.
		pthread_t threadConnection = PThreadInit;

		ConnectThread() {}
		~ConnectThread() {}

		// Launch process background tcp connection recovery.
		int Go(ZSelenaSocket* sock) {
			return( pthread_create(&threadConnection, NULL,
				// So ... first time in this project I use lambda-function ))).
				[](void* C) -> void* {
					ZSelenaSocket* cl = (ZSelenaSocket*)C;
					cl->JustConnect(cl->sockType);
					return nullptr;
				}, sock));
		};

		// Is background recovery tcp connection thread running.
		bool IsGo() {
			return IsValiPTread(threadConnection) ? true : false;
		}

		// Force terminate background recovery tcp connection thread.
		void Kill() {
			if (!IsGo()) return;
			pthread_cancel(threadConnection);
			threadConnection = PThreadInit;
		}

		// Waiting for the natural completion of the background thread 
		// of TCP connection recovery.
		int WaitFinish() {
			if (!IsGo()) return 0;
			int status_thread;
			int r = pthread_join(threadConnection, (void**)&status_thread);
			threadConnection = PThreadInit;
			return r;
		}
	} flowConnector;

};

#endif // #ifndef SELENA_SOCKET_HEADER

