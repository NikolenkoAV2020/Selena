// SelenaClientWriter.cpp
// Walter Artyomenko, August 2019
//
#include <iostream>
#include <sstream>

#include <NetWinUnixCompat.h>
#include <GetIPBeadRoll.h>
#include <ConsolColor.h>

#include <SelenaJob.h>
#include <SelenaStrings.h>
#include <SelenaSocket.h>
#include <SelenaSafeMessage.h>
#include <SelenaThreads.h>

#include "SelenaClient.h"

//-----------------------------------------------------------------------------
// Everything is simple here. 
//
// We read the queue of messages waiting to be sent  
// from the storage "clientMsg", and if there are something messages, 
// we send them through socket "ZSelenaClientEngene* pClient".
// Write loop terminates when appear signal "stopSignal",
// all threads stopping.
//-----------------------------------------------------------------------------
void* WriterMessage(void *args)
{
	ZSelenaSocket* pClient = (ZSelenaSocket*)args;
	if (!pClient) return nullptr;

	ZSelenaMessage M;

	// Establish timer for periodical check in job storage
	ZSelenaTimer Timer;
	Timer.Start(5);
	
	while (1) {
		// Checking the completion signal for all threads
		if (stopSignal->V()) break;

		if (Timer.IsTimeUp() && pClient && 
			pClient->IsReady()) 
		{	
			SelenaJob J;
			// Check ... Is there any job results
			int r = clientJobsStorage->V().GetFirstRes(J);
			if (!r) {
				// Begin send first result to server.
				SendScanResultToServer(J, pClient);
			} else {
				if (r == -1) {
					// Not any resalts for send to server
				}
				else {
					// Some error
		}	}	}

		// Get first message from the queue
		int count = clientMsg->V().GetFirst(M);
		// RU. ≈сли нет ни одного сообщени€ то отправл€ть серверу не чего
		// EN. If there is no message, then thereТs nothing to send to the server
		if (!count) continue;

		// Set the type of message as a text message
		M.type = smtMessage;

		if (pClient->isDisconnectError) {
			// Oh connection to the server is lost .... 
			// and what to do? Nothing can be sent to the server ....
			// You just wait until the connection is restored.
			// If you donТt know what to do then donТt do anything, itТs not 
			// the best way, but nonetheless ... 
			// Our message writer wait and working at idle.s

			//ZSelenaConsolOut SO;
			//SO.Beg(smtWarning, 1);
			//SO << "Warning: Connection is absent. "
			//   << "Anything not dispatch now." << SO.endl;
			//SO.End();
			continue;
		}

		// Yes! All is Ok! Writing message to socket and such way delivery 
		// it to the server
		int r = pClient->WriteOutgoing(M);
		if (!r) {
			// We delete the last message from message storage, 
			// we just sent from the message storage and it not need for 
			// us more. Strike out it from storage.
			clientMsg->V().Pop(M);
		} else {
			// Error sending message to server
		}
	}

	return SUCCESS_THREAD;
}

//-----------------------------------------------------------------------------
