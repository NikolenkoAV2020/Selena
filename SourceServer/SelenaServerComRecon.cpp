// SelenaServerComRecon.cpp 
// Walter Artyomenko, September 2019
//
//-----------------------------------------------------------------------------
#include "SelenaServer.h"

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
void ReconnectionToClient(ZSelenaSocket* pServer)
{
	// Mast be not null!
	// assert(pServer);

	// Set signal to reconnect whith client
	reconnectSignal->V() = true;

	// Build command for client
	ZSelenaMessage M;
	M.msg          = "restart";
	M.type         = smtCommand;
	M.msgFileName  = "";
	M.storFileName = "";

	// Send command to client
	pServer->WriteOutgoing(M);

	// Set signal to terminate threads reader, writer and handler
	stopSignal->V() = true;
}

//-----------------------------------------------------------------------------