// SelenaClientScanRun.cpp
// Walter Artyomenko, August 2019
//
// It's very simple code. Here solely work thread begin where execute 
// scanning according command received from server.
//
#include <NetWinUnixCompat.h>
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
int RunScan(SelenaJob& J, pthread_t& threadScan)
{
	int r = 0;

	// Update job state and writing a task to the operative job storage
	J.stage = 2;
	clientJob->V().Set(J);

	//-------------------------------------------------------------------------
	// Create scaning thread
	r = pthread_create(&threadScan, NULL, Scaning, &J);
	if (r) {
		ZSelenaConsolOut SO;
		SO.Beg(smtError, 1);
			SO << "Error: can't create scan processing thread, "
			   << "status " << r;
		SO.End();
	}
	else {
		// Writing a task to the operative job storage
		J.h.jID = threadScan;
		clientJob->V().Set(J);
		// Informing the server about the start of work
		MessageToServer("Scanning begins...");
	}

	return 0;
}

//-----------------------------------------------------------------------------
// RU
// ¬ыполнение всех операций необходимых после завершени€ сканировани€: 
//		- разбор результатов сканировани€;
//		- отправка полученных результатов серверу.
// EN
// Performing all operations necessary after the scan is completed: 
//		- analysis of the scan results; 
//		- sending the results to the server.
int PostExecuteMasScan(SelenaJob J)
{
	if (J.stage == 100) {
		if (IsValiPTread(J.h.jID)) {
			pthread_join(J.h.jID, nullptr);
		}
		//SendScanResultToServer(J);
	} else {
		// RU
		// „то-то пошло не так! сообщаем об этом серверу
		// EN
		// Something went wrong! report this to the server
		SendScanFailToServer(J);
	}
	
	// RU
	// “екуща€ работы выполнена. ќсвобождаем место в глобальной переменной 
	// дл€ нового задани€ клиенту.
	// EN
	// Ongoing work done. We free up space in the global variable 
	// for a new job for the client.
	J.stage = 0;
	memset(&J.h.jID, 0, sizeof(pthread_t));
	clientJob->V().Set(J);

	return 0;
}

//-----------------------------------------------------------------------------


