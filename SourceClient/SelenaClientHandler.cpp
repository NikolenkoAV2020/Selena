// SelenaClientHandler.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
void* MainHandler(void *args)
{
	ZSelenaSocket* pClient = (ZSelenaSocket*)args;
	if (!pClient) return nullptr;

	//-------------------------------------------------------------------------
	// –абочий поток дл€ запуска масскана
	pthread_t threadScan = PThreadInit;
	SelenaJob J;

	//-------------------------------------------------------------------------
	// Sending greetings to the server
	ZSelenaMessage M;
	M.msg  = "Connection test succesfull\0";
	M.type = smtText;
	clientMsg->V().Push(M);

	// Message about readiness to do scan job
	M.msg  = "Ready get the job.\0";
	M.msg += "\nType any masscan command...for exampl: masscan -p80 192.168.0.0/8\0";
	M.type = smtText;
	clientMsg->V().Push(M);
	M.Clear();

	// Establish timer for periodical check in job storage
	ZSelenaTimer Timer;
	Timer.Start(3);

	//-------------------------------------------------------------------------
	// ќсновной цыкл главного потока, реагирующий на сообщени€
	// клиента и интерпритирующий команды вводимые с консоли.
	while (1) {
		// ѕроверка сигнала завершени€ всех потоков
		bool isNeedBreak = false;
		// Befor termination loop need chech current job and 
		// terminate it also.
		if (stopSignal->V()) isNeedBreak = true;

		// View messages and errors from nmap-thread
		//bool isNmFatal, isNmMsg, isNmWar;
		//GetNmapGlobalState(isNmFatal, isNmMsg, isNmWar);
		//if (isNmFatal || isNmMsg || isNmWar) {
			// If there are some messages or error then send them to server
		//	std::string s, S;
		//	if (isNmFatal) {
		//		GetNmapGlobalFatal(S);
		//		s = "Nmap fatal error has occurred.\n";	s += S;
		//		MessageToServer(s);
		//	}
		//	if (isNmWar) {
		//		GetNmapGlobalWarning(S);
		//		s = "Nmap warning or error has occurred.\n";	s += S;
		//		MessageToServer(s);
		//	}
		//	if (isNmMsg) {
		//		GetNmapGlobalMessage(S);
		//		s = "Nmap message. ";	s += S;
		//		MessageToServer(s);
		//	}
		//}

		std::cout << std::flush;

		// »нформаци€ о текущей работе
		clientJob->V().Get(J);

		// ѕроверка завершени€ последней запущенной работы
		if (J.stage == 100) {
			// —канирование либо завершено успешно,
			// либо прервано в результате ошибки или по иным 
			// причинам
			PostExecuteMasScan(J);
		}

		// ≈сли получен сигнал о завершении клиентского приложени€,
		// то в случае если работа ещЄ выполн€етс€ соответствующий ей
		// поток принудительно завершаетс€.
		if (isNeedBreak) {
			if (J.stage && IsValiPTread(J.h.jID)) {
				PostExecuteMasScan(J);
			}
			break;
		}

		if (!J.stage && Timer.IsTimeUp()) {
			J.stage = 1;
			clientJob->V().Set(J);
			// If in current time we have not any job processing
			// Check job storage and select first wait
			if (!clientJobsStorage->V().TakeToWorkFirst(J)) {
				// Begin scaning... make the job.
				RunScan(J, threadScan);
			} else {
				J.stage = 0;
				clientJob->V().Set(J);
			}
		}

		// ѕроверка наличи€ в хранилище команд хот€ бы одной 
		// команды от сервера.
		// ≈сли какие-либо команды отсутствуют то ничего не делаетс€,
		// цыкл выполн€ет посто€нный опрос поток читающий командв 
		// от сервера
		if (!clientCom->V().Count()) continue;

		// Check first command in the queue,
		clientCom->V().GetFirst(M);

		//---------------------------------------------------------------------
		// Validate client termination command
		if (IsExitCommand(M.msg)) {
			clientCom->V().Pop(M);
			std::cout << "SelenaClient > exit" << std::endl;
			stopSignal->V() = true;
			break;
		}

		// While scanning is in progress, 
		// a new job cannot be started.
		if (J.stage) {
			if (IsMasscanCommand(M.msg)) {
				// if it's masscan command, then skip loop iteration
				continue;
			}
		}

		// Now extract the command from the repository 
		// of the commands received from the server.
		clientCom->V().Pop(M);

		if (!M.msg.length()) continue;

		//---------------------------------------------------------------------
		// ѕроверка команды по поводу возможного вызова masscan,
		// а также наличи€ в команде входного файла
		if (IsMasscanCommand(M.msg)) {
			// Received message - a masscan command.
			// ...then put it command to jobs storage.
			// clientJobsStorage->V().PutCommandToJobStorage(M.msg, M.msgFileName);
		} else {
			// Received message is not a masscan command.
			std::string s = "Unknown command to execute: \n";
			s += M.msg;
			s += "...";
			MessageToServer(s);
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------