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

// ��������� ������ �������
extern ThrSfSelenaMessages	serverCom;
// ��������� ��������� �������
extern ThrSfSelenaMessages	clientMsg;
// ���������� ������ � �������������� ���������� ���������� ����������
extern ThrSfBool			stopSignal;
extern ThrSfBool			reconnectSignal;

//-----------------------------------------------------------------------------
#endif // #ifndef SELENA_SERVER_GLOBAL
