// SelenaServer.cpp 
// Walter Artyomenko, August 2019
//
//-----------------------------------------------------------------------------
#include "SelenaServer.h"

#include <stdio.h>
#include <stdlib.h>

#include <LibNmapScan.h>
#include <happyhttp.h>

static int count = 0;
std::string programName = "";

//-----------------------------------------------------------------------------
int DoServer()
{
	ZSelenaConsolOut::DoHelpful();
	ZSelenaConsolOut SO;

	SO.Beg(smtText, 0);
		if (reconnectSignal->V()) {
			SO << "\n==================================================\n"
			   << "Selena Server v0.1 reconnect to client";
		} else {
			SO << "Selena Server v0.1 start";
		}
	SO.End();

	// Turn off client reconnect signal. It may be switch on.
	reconnectSignal->V()   = false;

	pthread_t threadReader  = PThreadInit;
	pthread_t threadWriter  = PThreadInit;
	pthread_t threadHandler = PThreadInit;
	int       status_thread;

	// Create thr server object
	ZSelenaSocket Server(sstKnocking, 0);

	// Connect server to client.
	int r = Server.Connect(); if (r) goto end;

	// Blocking the automatic displaying of  all data received and 
	// transmitted through the socket 
	Server.coutEcho = false;

	//-------------------------------------------------------------------------
	// Creating a main flow for processing server commands 
	// and client messages
	r = pthread_create(&threadHandler, NULL, MainHandler, (void*)&Server);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create main message processing thread,\n"
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
	// Creating a stream of reading messages received from the client
	r = pthread_create(&threadReader, NULL, ReaderMessage, (void*)&Server);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create thread,\n" 
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
	// Creating a flow of sending commands to the client
	r = pthread_create(&threadWriter, NULL, WriterMessage, (void*)&Server);
	if (r) {
		SO.Beg(smtError, 1);
			SO << "Error: can't create thread,\n"
			   << "status " << r << SO.endl;
		SO.End();
		exit(ERROR_CREATE_THREAD);
	}

	//-------------------------------------------------------------------------
end:;
	// Waiting for all threads normal terminated 
	if (IsValiPTread(threadHandler))
		r = pthread_join(threadHandler, (void**)&status_thread);
	if (IsValiPTread(threadReader))
		r = pthread_join(threadReader, (void**)&status_thread);
	if (IsValiPTread(threadWriter))
		r = pthread_join(threadWriter, (void**)&status_thread);

	// close connection whith client
	Server.DisConnect();

	ZSelenaConsolOut::DoNeedless();

	return r;
}

//-----------------------------------------------------------------------------
int GoServer()
{
	system("color 0F"); 
	SetConsoleTitle("Selena Server");

	// Client connection cycle 
	// and start of its message processing cycle.
	// Possible lot of connections and disconnections with client 
	// by user command "reconnect"
	int r;
	do {
		waitCommandSignal->V() = false;
		stopSignal->V()        = false;
		r = DoServer();
 	} while (reconnectSignal->V());

	system("pause");
	return 0;
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	//_main(argc, argv);
	programName = argv[0];
	GoServer();
	return 0;
}

//-----------------------------------------------------------------------------