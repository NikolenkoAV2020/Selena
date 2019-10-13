// SelenaClientReader.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
void* ReaderCommand(void *args)
{
	ZSelenaSocket* pClient = (ZSelenaSocket*)args;
	if (!pClient) return nullptr;

	// ќпрос сокета на предмет наличи€ вход€щего сообщени€
	// и при его наличии считывание.
	while (1) {
		// Checking the signal about all threads complete
		if (stopSignal->V()) break;
		if (pClient->isDisconnectError)	continue;

		ZSelenaMessage M;

		// Is there anything in the socket stream for reading
		unsigned int byteInStream;
		int r = pClient->ByteForRead(byteInStream);
		if (r) {
			// “ут нужно дать сигнал на восстановление соединени€
			// с сервером и перезапуск всех потоков!


			break;
		}
		if (!byteInStream) 
			// Nothint there is, therfor skip loops iteration
			continue;

		// „тение из сокета поступившего от сервера сообщени€
		// It's may by as a text message as a command.
		pClient->coutEcho = false;
		r = pClient->ReadIncoming(M);
		pClient->coutEcho = true;
		if (r) {
			// 

		}

		// If the message is empty or an error occurred while reading it, 
		// then message is not processed.
		if (r || !M.msg.length()) {
			// Just skip loop iteration.
			continue;
		}

		// Verification. Is the command a text message
		if (IsSendCommand(M.msg)) {
			// A similar message is simply displayed in the console
			int j = CommandFindWord(M.msg, "send");
			// Exclude keyword "send" from the command
			M.msg = M.msg.substr(j + 4);
			// Transform to usually text message.
			M.type = smtText;
		}

		// Show simple text message
		if (M.type == smtText) {
			// Show message
			EchoMessageToConsol(M);
			// Neither pushing nor duplicating the server 
			// receiving such a command is not necessary
			continue;
		}

		// All other messages are regarded as commands.
		M.type = smtCommand;
		EchoMessageToConsol(M);

		//---------------------------------------------------------------------
		// Ќекоторые команды должны исполн€тьс€ немедленно и не поступать 
		// в хранилище команд дл€ отложенного исполнени€.
		// Immediate command execution.
		//---------------------------------------------------------------------
		
		// ѕроверка поступлени€ команды на завершение клиентского приложени€.
		if (IsExitCommand(M.msg)) {
			// ќт сервера получено сообщение о завершении работы.
			// ƒанное сообщение не нуждаетс€ в обработке клиентом
			// и требует только завершени€ потока обработки комманд,
			// поступающих от сервера. 
			// ”станавливаем сигнал о завершении клиентского приложени€
			stopSignal->V() = true;
			// «авершаем цыкл чтени€ вход€щего потока данных и
			// тем самым завершаем поток чтени€ вход€щих сообщений
			break;
		}

		// ѕроверка поступлени€ команды на перезапуск клиентского приложени€.
		if (IsRestartCommand(M.msg)) {
			RestartClient();
			break;
		}

		r = ExecutFastCommand(M, pClient);
		if (!r) {
			// Ѕыла обнаружена и исполнена команда
			continue;
		} 
		if (r > 0) {
			// ѕроизошла ошибка при исполнении команды
		}
		r = 0;

		//---------------------------------------------------------------------
		// ѕодтверждение получени€ команды дл€ сервера
		EchoCommandToServer(M);

		//---------------------------------------------------------------------
		// ѕроверка команды по поводу возможного вызова masscan,
		// а также наличи€ в команде входного файла
		if (IsMasscanCommand(M.msg)) {

		} else {
			// Received message is not a masscan/nmap command.
			std::string s = "Unknown command to execute: \n";
			s += M.msg;
			s += "...";
			MessageToServer(s, "", smtError);
			continue;
		}

		// ¬се корректные команды от сервера помещаютс€ в хранилище команд
		// после чего обрабатываетс€ основным потоком программы.
		r = clientJobsStorage->V().PutCommandToJobStorage(M.msg, M.msgFileName);
		if (r) {
			std::string s = "Error saving received command (" + M.msg + ")";
			MessageToServer(s, "", smtWarning);
		}
		//clientCom->V().Push(M);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// ƒублирование серверу пришедшей от него команды
void EchoCommandToServer(ZSelenaMessage& M)
{
	ZSelenaMessage sendM;
	sendM.msg = "Received command: \n";
	sendM.msg += M.msg;
	sendM.type = smtText;
	clientMsg->V().Push(sendM);
}

//-----------------------------------------------------------------------------
void EchoMessageToConsol(ZSelenaMessage& M)
{
	ZSelenaConsolOut SO;
	std::string		 entity = (M.type == smtCommand ? "command" : "message");

	SO.Beg(smtText, 2);
		SO << "Received from server: " << SO.endl;
	SO.Atr(M.type, 3);
		SO << entity << ": " << M.msg;
		if (M.msgFileName.length()) {
			SO << "\n" << "file transferred with " << entity
			   << ": " << M.msgFileName;
		}
	SO.End();
}

//-----------------------------------------------------------------------------