// SelenaClient.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_CL_HEADER
#define SELENA_CL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>

#include <NetWinUnixCompat.h>
#include <GetIPBeadRoll.h>
#include <ConsolColor.h>

#include <SelenaJob.h>
#include <SelenaThreads.h>
#include <SelenaStrings.h>
#include <SelenaTimer.h>
#include <SelenaSafeMessage.h>
#include <SelenaSocket.h>
#include <IpAddressTrawler.h>

#include "SelenaClientGlobal.h"

#include <LibNmapScan.h>
#include <LibMasScan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

	// Отправка сообщения на сервер
	void MessageToServer(
		const std::string message,
		const std::string fileMsg = "",
		SelenaMsgType	  type = smtText);

	void MessageToClient(
		const std::string message,
		int type);

	// Рабочие функции потоков
	void* ReaderCommand(void *args);
	void* WriterMessage(void *args);
	void* MainHandler  (void *args);
	void* Scaning      (void *args);

	// RU. Запуск масскана
	// EN. Start scan process
	int RunScan(SelenaJob& J, pthread_t& threadScan);

	// RU. Выполнение всех операций необходимых после завершения сканирования: 
	//		- разбор результатов сканирования;
	//		- отправка полученных результатов серверу.
	// EN. Performing all operations necessary after the scan is completed: 
	//		- analysis of the scan results; 
	//		- sending the results to the server.
	int PostExecuteMasScan    (SelenaJob J);
	// RU. Отправка результатов на сервер
	// EN. Send result to server.
	int SendScanResultToServer(SelenaJob J, ZSelenaSocket* pClient);
	// RU. Что-то пошло не так! сообщаем об этом серверу
	// EN. Something went wrong! report this to the server
	int SendScanFailToServer  (SelenaJob J);

	void EchoCommandToServer(ZSelenaMessage& M);
	void EchoMessageToConsol(ZSelenaMessage& M);
	int  ExecutFastCommand  (ZSelenaMessage& M, ZSelenaSocket* pClient);

	int MergeIpJsonFiles(
		// source file 1
		const std::string sourcef1,
		// source file 2
		const std::string sourcef2,
		// result file
		const std::string resultf);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef SELENA_CL_HEADER
