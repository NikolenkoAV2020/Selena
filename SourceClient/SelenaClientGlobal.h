// SelenaClientGlobal.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_CLIENT_GLOBAL
#define SELENA_CLIENT_GLOBAL

#include <SelenaGlobal.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

	void* MainHandler  (void *args);
	void* ReaderCommand(void *args);
	void* WriterMessage(void *args);
	void* Scaning      (void* P);
	void  RestartClient();

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

extern ThrSfBool			stopSignal;
extern ThrSfSelenaMessages	clientMsg;
extern ThrSfSelenaMessages	clientCom;
extern ThrSfSelenaJob		clientJob;
extern ThrSfSelenaJobsMng	clientJobsStorage;

//-----------------------------------------------------------------------------
#include <LibNmapScan.h>
#include <LibMasScan.h>

//-----------------------------------------------------------------------------
#endif // #ifndef SELENA_CLIENT_GLOBAL

