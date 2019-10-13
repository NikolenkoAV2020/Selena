// SelenaServerHandler.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaServer.h"

//-----------------------------------------------------------------------------
void* MainHandler(void *args)
{
	ZSelenaSocket* pServer = (ZSelenaSocket*)args;
	// If the server address is not passed, 
	// then immediately terminate the function and the corresponding thread.
	if (!pServer) return nullptr;

	ZSelenaMessage M;

	//-------------------------------------------------------------------------
	// Loop responding to commands entered in the console.
	while (1) {
		// Checking the arrival of a signal to complete all threads 
		// and close the server application
		if (stopSignal->V()) break;

		M.Clear();

		// Requestion of command from the terminal
		waitCommandSignal->V() = true;
		std::cout << "Selena Server >>>";
		std::getline(std::cin, M.msg);
		waitCommandSignal->V() = false;

		// We consider that all that the operator enters 
		// from the console for the client is commands.
		M.type = smtCommand;

		//---------------------------------------------------------------------
		// Check received some commands that need execute immediately 

		// Checking the receipt of the command to shut down the server.
		if (IsExitCommand(M.msg)) {
			// Activating a signal to close all threads 
			// and terminate the application.
			stopSignal->V() = true;
			// Immediate completing the server command processing cycle
			break;
		}

		// Checking the receipt of the command to restart client and
		// reconnect server to restarting client.
		if (IsRestartCommand(M.msg)) {
			ReconnectionToClient(pServer);
			// Immediate completing the server command processing cycle
			break;
		}

		// Checking the receipt of the command to interrupt 
		// client connection
		if (IsInterruptCommand(M.msg)) {
			InterruptClient(pServer);
			// Immediate completing the server command processing cycle
			break;
		}

		//---------------------------------------------------------------------
		// Record the received command in the command store.
		serverCom->V().Push(M);
		// Pushed commands will be extract and execute later in "Reader"

		// 
		if (pServer->isDisconnectError) {
			ZSelenaConsolOut SO;
			SO.Beg(smtWarning, 1);
				SO << "Warning: Connection is absent. "
				   << "Command not dispatch now." << SO.endl;
			SO.End();
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------