// SelenaServerGlobal.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_SERVER_GLOBAL
#define SELENA_SERVER_GLOBAL

#include <SelenaGlobal.h>

class ZSelenaSocket;

void  ReconnectionToClient(ZSelenaSocket* pServer);
void  InterruptClient     (ZSelenaSocket* pServer);

void* ReaderMessage(void *args);
void* WriterMessage(void *args);
void* MainHandler  (void *args);

// Хранилище команд сервера
extern ThrSfSelenaMessages	serverCom;
// Хранилище сообщений клиента
extern ThrSfSelenaMessages	clientMsg;
// Глобальный сигнал о неообходимости завершения серверного приложения
extern ThrSfBool			stopSignal;
extern ThrSfBool			reconnectSignal;

//-----------------------------------------------------------------------------
#endif // #ifndef SELENA_SERVER_GLOBAL
