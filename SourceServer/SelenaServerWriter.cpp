// SelenaServerWriter.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaServer.h"

//-----------------------------------------------------------------------------
void* WriterMessage(void *args)
{
	ZSelenaSocket* pServer = (ZSelenaSocket*)args;
	// If the server address is not passed, 
	// then immediately terminate the function and the corresponding thread.
	if (!pServer) return nullptr;

	ZSelenaMessage M;

	while (1) {
		// Checking the arrival of a signal to complete all threads 
		// and close the server application
		if (stopSignal->V()) break;

		// Если в хранилище команд нет то итерация цикла пропускается
		if (!serverCom->V().Count()) continue;

		// View the first command in the server command storage
		M.Clear();
		serverCom->V().GetFirst(M);

		// Checking the receipt of the command to shut down the server.
		if (IsExitCommand(M.msg)) {
			// Extracting
			serverCom->V().Pop(M);
			// Send command to client if connect is exist only
			M.msgFileName = "";
			if (!pServer->isDisconnectError) {
				pServer->WriteOutgoing(M);
			}
			// Activating a signal to close all threads 
			// and terminate the application.
			stopSignal->V() = true;
			// Complete the server command processing cycle
			break;
		}
		
		// If the connection was broken, then reading anything from the socket
		// stream is pointless. Therefore, we skip the iteration of the loop.
		if (pServer->isDisconnectError) continue;

		// extracting command from server storage
		serverCom->V().Pop(M);

		// If the command is zero long, then loop iteration is skipped
		if (M.IsEmpty()) continue;

		// Checking the receipt of a text message for the client.
		if (IsSendCommand(M.msg)) {
			// Dispatch 
			pServer->WriteOutgoing(M);
			// Skip current loop iteration
			continue;
		}

		// If the command format meets the requirements masscan
		if (IsMasscanCommand(M.msg)) {
			// Checking file presence in a command
			CommandFindInFileName(M.msg, M.msgFileName);
		}

		// Обнаруженая команда отправляется клиенту.
		// Запуск команды на выполнение, то есть отправка 
		// данной команды клиенту. Отправка команды вместе 
		// с сопутствующим файлом.
		pServer->WriteOutgoing(M);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
