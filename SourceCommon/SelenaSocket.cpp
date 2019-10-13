// SelenaSocket.cpp 
// Walter Artyomenko, August 2019
//
#include <NetWinUnixCompat.h>
#include <ConsolColor.h>
#include <SelenaSocket.h>
#include <SelenaConfig.h>
#include <SelenaStrings.h>
#include <SelenaGlobal.h>

//-----------------------------------------------------------------------------
ZSelenaSocket::ZSelenaSocket(ZSelenaSocketType Type, char ownType)
{
	sockType = Type;
	ownVar = ownType;

	subPort = 50555;
	subHostName = "localhost";
}

ZSelenaSocket::~ZSelenaSocket()
{
	DisConnect();
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::DisConnect()
{
	// If ttere is reconnection thread now that kill it.
	flowConnector.Kill();

	// If connection is absent than exit
	if (!ownSocket) return 0;

	int r = 0;

	if (subSocket) {
		r = CloseSocket(subSocket);
		if (r == SOCKET_ERROR) {}
	}

	subPort = 0;
	subHostIp = "";
	subHostName = "";

	if (ownSocket) {
		r = CloseSocket(ownSocket);
		if (r == SOCKET_ERROR) {}
	}

	ownPort = 0;
	ownHostIp = "";
	ownHostName = "";

	SocketsSwitchOff();
	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::JustConnect(ZSelenaSocketType type)
{
	if (type != sstDefault)	sockType = type;

	int r = (sockType == sstKnocking ?
			ConnectLikeClient() :
			ConnectLikeServer());

	isDisconnectError = r ? true : false;
	return r;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::Connect(ZSelenaSocketType type) 
{
	// Hot reconnection process is runing.
	if (flowConnector.IsGo()) return -1;
	// Disconnection
	DisConnect();
	// Here we just build connection without any conditions
	return (JustConnect(type));
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ReConnect() 
{
	DisConnect();
	return (Connect());
}

//-----------------------------------------------------------------------------
// Hot reconnection of socket, restore corrupted connection without 
// terminate main thread and scanning procedure.
// This is an essential for Selena project feature of socket. 
// If somthings wrong happen in tcp-connection by any reason Selena 
// workflow must don't stop, and if it's possible connection must 
// be rocovery in shadow mode.
int ZSelenaSocket::HotReConnect()
{
	// If reconnection thread is exist now, that procedure of anew 
	// connection was launch afore and work now. Simply exit from here.
	if (flowConnector.IsGo()) return -1;

	// Kill all that have any relation to old tcp connection
	DisConnect();
	isDisconnectError = true;

	// Create connection thread.
	flowConnector.Go(this);
	
	// Check error launch connection thread.s
	if (!flowConnector.IsGo()) {
		ZSelenaConsolOut SO;
		SO.Beg(smtError, 1);
			SO << "Error: can't create hot reconnaction thread.";
		SO.End();
	} else {
		ZSelenaConsolOut SO;
		SO.Beg(smtWarning, 1);
			SO << "Warning: start hot reconnaction procedura.";
		SO.End();
	}

	// Now while Selena Client make targetal work, 
	// early corrupted socket try restory tcp connection.
	// When connection will be rested Client program send to Server 
	// all last messages and results.

	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ConnectLikeServer()
{
	int r = CreateSocket(); if (r) return r;

	// Прежде чем передавать данные через сокет, его необходимо связать с 
	// адресом в выбранном домене(эту процедуру называют именованием сокета).
	// Иногда связывание осуществляется неявно(внутри функций 
	// connect и accept), но выполнять его необходимо во всех случаях.
	// Вид адреса зависит от выбранного вами домена.
	// В Unix - домене это текстовая строка - имя файла, через который 
	// происходит обмен данными.В Internet - домене адрес задаётся 
	// комбинацией IP - адреса и 16 - битного номера порта.
	// IP - адрес определяет хост в сети, а порт - конкретный сокет 
	// на этом хосте.
	// Протоколы TCP и UDP используют различные пространства портов.

	// Для явного связывания сокета с некоторым адресом используется 
	// функция bind.

	NagleOff(ownSocket);

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port   = htons(AURORA_PORT);
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (coutEcho && !flowConnector.IsGo()) {
		std::cout << std::endl << "Connecting on behalf of..." << std::endl;
	}


	//_________________________________________________________________________
	//
	HostIp ipHostLoc;
	IpFromHostName("localhost", ipHostLoc, !flowConnector.IsGo());
	//____________________________________________________________________________
	//

	//r = BuilUlongSaddr(AF_INET, ipHostLoc.ipAddr[0].c_str(), &serverAddr.sin_addr);
	char ip[64] = "0.0.0.0";
	r = BuilUlongSaddr(AF_INET, ip, &serverAddr.sin_addr);
	if (r != 1) return HandleError("inet_pton", nullptr);

	r = bind(ownSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (r) return HandleError("bind", nullptr);

	if (coutEcho && !flowConnector.IsGo()) {
		std::cout << std::flush;
	}

	//____________________________________________________________________________
	// На следующем шаге создаётся очередь запросов на соединение.
	// При этом сокет переводится в режим ожидания запросов со стороны 
	// клиентов.Всё это выполняет функция listen.
	r = listen(ownSocket, 128);
	// Первый параметр - дескриптор сокета, а второй задаёт размер очереди 
	// запросов.Каждый раз, когда очередной клиент пытается соединиться с 
	// сервером, его запрос ставится в очередь, 
	// так как сервер может быть занят обработкой других запросов.
	// Если очередь заполнена, все последующие запросы будут 
	// игнорироваться.Когда сервер готов обслужить очередной запрос, 
	// он использует функцию accept.
	//____________________________________________________________________________

	if (r) return HandleError("listen", nullptr);

	if (coutEcho && !flowConnector.IsGo()) {
		// If not hot reconnection.
		ZSelenaConsolOut SO;
		SO.Beg(smtText, 0);
			SO << "Some Selena " << (!ownVar ? "client" : "server")
			   << "s" << " connection wait...";
		SO.End();
	}

	//____________________________________________________________________________
	// Who is online? Anybody client?
	r = WhoIsThere(); if (r) return r;

	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ConnectLikeClient()
{
	int r = 0;

	//_________________________________________________________________________
	// Reading addres for connection
	unsigned int PortNumber = 0;
	std::string  Name = "";
	std::string  Ip = "";

	r = ReadConfig(ownVar, "SelenaClient.cfg", Name, Ip, PortNumber);
	
	// if an error occurred or the user refused to start the program,
	// terminates client application.
	if (r) return-1;

	//_________________________________________________________________________
	// Connection to server.
	r = -1;
	// either by IP-address, if it specified, ...
	if (Ip.length()) {
		r = ConnectByHostIp(Ip.c_str(), PortNumber);
	}
	// or by host name, if IP-address not know or any error happened.
	if (r == -1 && Name.length()) {
		r = ConnectByHostName(Name.c_str(),PortNumber);
		if (r) {
			// if an error occurred in this case, then the connection is imposible...
			// Terminates client application.
			return 1;
	}	}

	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ConnectByHostName(const char* hostName, unsigned int port)
{
	int r = 0;
	r = CreateSocket(); if (r) return r;

	HostIp ipServerHost;
	std::cout << std::endl << "Connected to..." << std::endl;
	IpFromHostName(hostName, ipServerHost, coutEcho);
	if (!ipServerHost.countAddr) {
		return HandleError(
			"IpFromHostName",
			"There is no one address for the given host name");
	}

	subHostName = hostName;

	r = ConnectByHostIp(ipServerHost.ipAddr[0].c_str(), port);
	return r;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ConnectByHostIp(const char* hostIp, unsigned int port)
{
	int r = 0;

	if (!ownSocket) {
		r = CreateSocket(); if (r) return r;
	}

	subHostIp = hostIp;
	subPort   = port;

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((uint16_t)port);

	r = BuilUlongSaddr(AF_INET, hostIp, &serverAddr.sin_addr);
	if (r != 1)	return HandleError("inet_pton");

	//_________________________________________________________________________
	// Establish the connection to the server
	r = connect(ownSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (r < 0) return HandleError("connect", nullptr);

	NagleOff(ownSocket);

	//_________________________________________________________________________
	// It is necessary to identify yourself to the listener 
	// with whom you just connected
	ZSelenaMessage M;
	M.type = smtHello;
	M.msg  = "Connection test succesfull";
	r = WriteOutgoing(M);

	return r;
}

int ZSelenaSocket::WhoIsThere()
{
	sockaddr_in clientAddr;
	socklen_t	sizeOfClientAddr = sizeof(clientAddr);

restart:;
	subSocket = accept(ownSocket, (sockaddr*)&clientAddr, &sizeOfClientAddr);
	if (subSocket == INVALID_SOCKET || subSocket == SOCKET_ERROR) {
		return HandleError("accept", nullptr);
	}
	NagleOff(subSocket);

	// Функция accept создаёт для общения с клиентом новый сокет и возвращает
	// его дескриптор.Параметр sockfd задаёт слушающий сокет.
	// После вызова он остаётся в слушающем состоянии и может принимать 
	// другие соединения.В структуру, на которую ссылается addr, 
	// записывается адрес сокета клиента, который установил соединение с 
	// сервером.В переменную, адресуемую указателем addrlen, 
	// изначально записывается размер структуры; 
	// функция accept записывает туда длину, которая реально была 
	// использована.Если вас не интересует адрес клиента, вы можете просто 
	// передать NULL в качестве второго и третьего параметров.

	// Обратите внимание, что полученный от accept новый сокет связан с тем же
	// самым адресом, что и слушающий сокет.
	// Сначала это может показаться странным.Но дело в том, что адрес 
	// TCP-сокета не обязан быть уникальным в Internet-домене.
	// Уникальными должны быть только соединения, для идентификации которых 
	// используются два адреса сокетов, между которыми происходит обмен 
	// данными.

	if (subSocket && !flowConnector.IsGo()) {
		if (coutEcho) {
			std::cout
				<< "__________________________________________________" 
				<< std::endl;
		}

		char    S[256];
		HOSTENT *hst;
		hst = gethostbyaddr((char*)&clientAddr.sin_addr.s_addr, 4, AF_INET);
		subHostName = (hst) ? hst->h_name : "";
		subHostIp = inet_ntop(AF_INET, &clientAddr.sin_addr, S, 256);
		if (coutEcho) {
			std::cout
				<< "\t" << subHostName << "[" << subHostIp << "]"
				<< std::endl;
			std::cout
				<< "__________________________________________________\n" 
				<< std::endl;
		}
	}
	std::cout << std::flush;

	ZSelenaMessage M;
	int r =-1;
	while (r==-1) {
		bool _coutEcho = coutEcho;
		coutEcho = false;
		r = ReadIncoming(M);
		coutEcho = _coutEcho;
	}

	if (r || M.type != smtHello || M.msg != "Connection test succesfull") {
		CloseSocket(subSocket);
		std::cout
			<< "\t" << subHostName << "[" << subHostIp << "]"
			<< "\t disconnect!" << std::endl;
		goto restart;
	}

	//-----------------------------------------------------------------------------
	if (coutEcho) {
		if (flowConnector.IsGo()) {
			ZSelenaConsolOut SO;
			SO.Beg(smtMessage, 0);
			SO << "__________________________________________________\n"
			   << "\tConnection recovered\n"
			   << "__________________________________________________\n";
			SO.End();
		}
		else {
			ZSelenaConsolOut SO;
			SO.Beg(smtText, 1);
				SO << "Received greeting from "
				   << (!ownVar ? "client" : "server")
			       << ": " << SO.endl;
			SO.Atr(smtMessage, 2);
				SO << M.msg << SO.endl;
			SO.End();
	}	}

	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::HandleError(const char* Func, const char* usererr)
{
	// Get last error text
	char errorText[512];
	int  numError = GetSocketErrorText(errorText, 512);

	ZSelenaConsolOut SO;
	SO.Beg(smtError, 1);
		SO << "An error occurred: " << errorText;
		SO << "Error " << Func << ":";
		if (usererr && strlen(usererr) > 0) {
			SO << " " << usererr;
		}
		SO << " code - " << numError;
	SO.End();

	isDisconnectError = IsNetDisconnectError(numError);
	return -1;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::CreateSocket()
{
	// Перебор серых и белого ip адресов
	GetIPBeadRoll(aboutIp);

	// Если сокет был раньше открыт то закрываем его и соответственно
	// закрываем соответствующее соединение
	if (ownSocket) {
		CloseSocket(ownSocket);
	}

	int r = OpenSocket(ownSocket);
	if (r == SOCKET_ERROR) return HandleError("socket");

	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ReadIncoming(ZSelenaMessage& M)
{
	return(ReadMsg(M, RWS()));
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::WriteOutgoing(ZSelenaMessage& M)
{
	int r = WriteMsg(M, RWS());
	if (M.isPending) {
		// Show in consol message about Pending
		ZSelenaConsolOut SO;
		SO.Beg(smtWarning, 1);
		if (r) {
			if (M.msg.length()) {
				SO << "Warning: message ..." << SO.endl;
				SO.Atr(smtWarning, 2);
				SO << "\"" << M.msg << "\"" << SO.endl;
				SO << "rejected and delayed.";
			} else {
				if (M.msgFileName.length()) {
					SO << "Warning: file ..." << SO.endl;
					SO.Atr(smtWarning, 2);
					SO << "\"" << M.msgFileName << "\"" << SO.endl;
					SO << "rejected and delayed.";
			}	}
		} else {
			SO.Atr(smtText, 1);
			if (M.msg.length()) {
				SO << "Pending message ..." << SO.endl;
				SO.Atr(smtText, 2);
				SO << "\"" << M.msgFileName << "\"" << SO.endl;
				SO << "is sended.";
			} else {
				if (M.msgFileName.length()) {
					SO << "Pending  file ..." << SO.endl;
					SO.Atr(smtText, 2);
					SO << "\"" << M.msg << "\"" << SO.endl;
					SO << "is sended.";
		}	}	}
		SO.End();
	}
	return r;
}

//-----------------------------------------------------------------------------
bool ZSelenaSocket::IsKeepAlive(SOCKET srcSocket)
{
	return true;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::SendMessageToHost(const char* message, unsigned int len)
{
	SOCKET socket = RWS();
	if (!socket) return 0;
	if (!message || !len) return 0;

	ssize_t sentBytes = 0;
	ssize_t sendstrlen = (ssize_t)len;

	sentBytes = send(socket, message, sendstrlen, 0);
	if (sentBytes <= 0) HandleError("send");
	else {
		if (coutEcho && sentBytes == sendstrlen) {
			ZSelenaConsolOut SO;
			SO.Beg(smtText, 1);
			SO << "Send to " 
			   << (ownVar ? "client" : "server") 
			   << " message";
			SO.End();
	}	}

	return (int)sentBytes;
}

//-----------------------------------------------------------------------------
// Is there anything in the input stream
int ZSelenaSocket::IsThereIncoming()
{
	SOCKET socket = RWS();
	return(IsThereAnyRead(socket));
}

//-----------------------------------------------------------------------------
// number of bytes waiting to be read in the input stream
unsigned int ZSelenaSocket::WaitingBytes()
{
	//SOCKET socket = RWS();
	//return(ByteForRead(socket));
	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ByteForRead(unsigned int& bytes, SOCKET srcSocket)
{
	if (!srcSocket) srcSocket = RWS();
	bytes = 0;
	int r = BytesInSocket(srcSocket, bytes);
	if (r == SOCKET_ERROR) return HandleError("Get bytes in socket");
	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ByteWrited(unsigned int& bytes, SOCKET srcSocket)
{
	if (!srcSocket) srcSocket = RWS();
	bytes = 0;
	int r = BytesInSocket(srcSocket, bytes);
	if (r == SOCKET_ERROR) return HandleError("Get bytes in socket");
	return 0;
}

//-----------------------------------------------------------------------------
bool ZSelenaSocket::IsThereAnyRead(SOCKET srcSocket)
{
	fd_set  Read;
	timeval Wait;

	int events;

	// очищаем набор сокетов
	FD_ZERO(&Read);

	// добавляем сокет в набор
	FD_SET(srcSocket, &Read);

	// заполняем структуру времени ожидания
	Wait.tv_sec  = 0;
	Wait.tv_usec = 100;

	// ожидаем подтверждения
	events = select(0, &Read, NULL, NULL, &Wait);

	// если нет ошибки
	if ((events != SOCKET_ERROR) && events) return true;

	return false;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::NagleOff(SOCKET srcSocket)
{
	unsigned int flag = 0;
	int r = (int)setsockopt(srcSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
	return r;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::NagleOn(SOCKET srcSocket)
{
	int flag = 1;
	int r= setsockopt(srcSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
	return r;
}

//-----------------------------------------------------------------------------
bool ZSelenaSocket::IsNagleOn(SOCKET srcSocket)
{
	int       flag = 0;
	socklen_t Size = sizeof(int);
	getsockopt(srcSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, &Size);
	return flag == 1 ? true : false;
}

//-----------------------------------------------------------------------------
