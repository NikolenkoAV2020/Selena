// SelenaClient.cpp
// Walter Artyomenko, August 2019
//
#include <stdio.h>
#include <signal.h>
#include <SelenaConfig.h>
#include "SelenaClient.h"
#include "SelenaClientGlobal.h"

//------------------------------------------------------------------------------
void SigBreakHandler(int n_signal)
{
	// Перед закрытием клиентского приложения необходимо
	// остановить текущую работу по сканированию и прервать соответствующий поток.
	// После чего отправить сигнал на закрытие всех остальных потоков:
	// чтения команд сервера, отправки сообщений серверу 
	// и потока выполнения поступающих команд. 

	// Информация о текущей работе
	SelenaJob J;
	clientJob->V().Get(J);

	// Принудительное прерывание потока сканировния.
	if (J.stage == 101) {
		if (IsValiPTread(J.h.jID)) {
			PostExecuteMasScan(J);
		}
	}

	// Сигнал на закрытие остальных потоков
	stopSignal->V() = true;
}

//-----------------------------------------------------------------------------
// Restarting program in case resived server command about terminate and 
// new launch client application  
std::string programName;

void RestartClient()
{
#ifdef defWINDOWS
	ShellExecute(
		NULL, "open", programName.c_str(), 
		NULL, NULL, SW_SHOWDEFAULT);
#endif
	stopSignal->V() = true;
	//unsigned long PID = GetCurrentProcessId();
	//TerminateProcess(
	//OpenProcess(PROCESS_TERMINATE, FALSE, PID), 0);
}

//-----------------------------------------------------------------------------
int GoClient()
{
	int r = 0;
	r = system("color 0F");
	SetWindowName("Selena Client");

	//-------------------------------------------------------------------------
	//clientCom->V().Restory("SelenaClientStorageCom.txt");
	//clientMsg->V().Restory("SelenaClientStorageMsg.txt");

	// Для перехвата закрытия консольного окна
	//signal(SIGINT, &SigBreakHandler);
	//signal(SIGBREAK, &SigBreakHandler);
	//signal(SIGTERM,  &SigBreakHandler);

	ZSelenaConsolOut::DoHelpful();
	ZSelenaConsolOut SO;

	SO.Beg(smtText, 0);
		SO << "Selena client v0.1 start.";
	SO.End();

	// Create thr client object
	ZSelenaSocket Client(sstListening, 1);

	// Connect client to server.
	r = Client.Connect(); if (r) return r;

	//-------------------------------------------------------------------------
	pthread_t threadReader = PThreadInit;
	pthread_t threadWriter = PThreadInit;
	pthread_t threadHandler = PThreadInit;
	int       status_thread = 0;

	//-------------------------------------------------------------------------
	// Creating a main flow (thread) for processing server commands 
	// and client messages
	r = pthread_create(&threadHandler, NULL, MainHandler, (void*)&Client);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create main message processing thread,\n"
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
	// Создание потока чтения команд поступающих от сервера
	r = pthread_create(&threadReader, NULL, ReaderCommand, (void*)&Client);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create thread,\n" 
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
	// Создание потока отправки сообщений серверу
	r = pthread_create(&threadWriter, NULL, WriterMessage, (void*)&Client);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create thread,\n" 
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
	// Ожидание завершения всех потоков
	if (IsValiPTread(threadHandler))
		pthread_join(threadHandler, (void**)&status_thread);
	if (IsValiPTread(threadWriter))
		pthread_join(threadWriter, (void**)&status_thread);
	if (IsValiPTread(threadReader))
		pthread_join(threadReader, (void**)&status_thread);

	// Сохранение очереди комманд и сообщений в тестовых файлах.
	clientCom->V().HoldOver();
	clientMsg->V().HoldOver();

	// Terminating connection
	Client.DisConnect();
	ZSelenaConsolOut::DoNeedless();
	return 0;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	//clientJobsStorage->V().PutCommandToJobStorage(
	//	std::string("masscan -p21,80,443  47.52.48.215/22 -oJ Joutput2.json"), 
	//	std::string(""));

	//clientJobsStorage->V().PutCommandToJobStorage(
	//	std::string("masscan -p21,80 47.51.48.215/23 -oJ Joutput3.json"), 
	//	std::string(""));
	// masscan -p21,80 47.51.48.215/23 -oJ A.json

	//"  -oL/-oJ/-oD/-oG/-oB/-oX/-oU <file>: Output scan in List/JSON/nDjson/Grepable/Binary/XML/Unicornscan format,\n"
	//	"     respectively, to the given filename. Shortcut for\n"
	//	"     --output-format <format> --output-file <file>\n"

	//clientJobsStorage->V().PutCommandToJobStorage(
		//std::string("masscan -p21,80,443 47.51.48.215/23 -oX A.json; nmap -p80,443 --take-finger-print"),
		//std::string("masscan -p80,443 47.51.48.215/23 -oJ A.json; nmap -p80,443 -Otfp"),
	// masscan -p80,443 47.51.48.215/23 -oJ A1.json; nmap -p80,443 -Otfp
	// masscan -p80,443 47.51.48.215/23 -oJ A2.json; nmap -p80,443 -Otfp
	// masscan -p80,443 47.51.48.215/23 -oJ A3.json; nmap -p80,443 -Otfp
		//std::string(""));
	// nmap -p 80 -oLAA.txt --script "dns-brute-new" ya.ru


	/*
	clientJobsStorage->V().PutCommandToJobStorage(
		std::string("masscan -p21,80,443 47.52.48.215/24 -oJ Joutput4.json"), 
		std::string(""));

	clientJobsStorage->V().PutCommandToJobStorage(
		std::string("masscan -p21,80,443 47.52.48.215/24 -oJ Joutput5.json"), 
		std::string(""));

	clientJobsStorage->V().PutCommandToJobStorage(
		std::string("masscan -p21,80,443 47.51.48.215/24 -oJ Joutput6.json"), 
		std::string(""));
*/
/*
	SelenaJobsManager JR;
	std::string f1;
	JR.MakeUniqueName("SelenaJob", f1);
	JR.MakeUniqueName("SelenaJob", f1);
	JR.MakeUniqueName("SelenaJob", f1);

	SelenaJob J;
	J.command = "masscan -p80, 443, 8080 -finputip.txt -joutput.rez";
	J.fileInputName  = "inputip.txt";
	J.fileOutputName = "output.rez";	
	J.storageName    = "";
	JR.StorageJ(J);

	J.command = "masscan -p80, 443, 8080 -finputip2.txt -joutput2.rez";
	J.fileInputName  = "inputip2.txt";
	J.fileOutputName = "output2.rez";
	J.storageName    = "";
	JR.StorageJ(J);

	J.command = "masscan -p80, 443, 8080 -finputip3.txt -joutput3.rez";
	J.fileInputName  = "inputip3.txt";
	J.fileOutputName = "output3.rez";
	J.storageName    = "";
	JR.StorageJ(J);

	J.command = "masscan -p80, 443, 8080 -finputip4.txt -joutput4.rez";
	J.fileInputName  = "inputip4.txt";
	J.fileOutputName = "output4.rez";
	J.storageName    = "";
	JR.StorageJ(J);

	J.command = "masscan -p80, 443, 8080 -finputip5.txt -joutput5.rez";
	J.fileInputName  = "inputip5.txt";
	J.fileOutputName = "output5.rez";
	J.storageName    = "";
	JR.StorageJ(J);

	JR.GetFirst(J);
	JR.TakeToWork(J);
	J.stage = 2;
	JR.StorageJ(J);
	J.stage = 3;
	JR.StorageJ(J);
	J.stage = 0;
	JR.RestoryJ(J);

	JR.RemoveJ(J);

	std::vector<std::string> FL;
	JR.GetJobsList(FL);
	for (auto name : FL)
		std::cout << name << std::endl;
	std::cout << JR.GetJobsCount() << std::endl;
*/
	programName = argv[0];
	GoClient();
	return 0;
}

//------------------------------------------------------------------------------
