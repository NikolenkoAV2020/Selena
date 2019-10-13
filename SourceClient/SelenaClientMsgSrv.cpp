// SelenaClientMsgSrv.cpp 
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
// ������������ ������� ��������� �� ���� �������
void MessageToServer(
	const std::string message,
	const std::string fileMsg,
	SelenaMsgType	  type)
{
	ZSelenaMessage M;
	M.msg         = message;
	M.msgFileName = fileMsg;
	M.type        = type;

	clientMsg->V().Push(M);
}

//-----------------------------------------------------------------------------
void MessageToClient(const std::string message, int type) 
{
	// ���������� � ������� ���������
	ZSelenaConsolOut SO;
	SO.Beg((SelenaMsgType)type, 1);
		SO << message;
	SO.End();
}

//-----------------------------------------------------------------------------