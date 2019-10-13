// SelenaClientSendScanFailToServ.cpp
// Walter Artyomenko, September 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
// RU. „то-то пошло не так! сообщаем об этом серверу
// EN. Something went wrong! report this to the server
int SendScanFailToServer(SelenaJob J)
{
	std::string s;
	s += "Job: " + J.storageName + "\n";
	s += "Command: " + J.command + "\n";
	s += "File: " + J.fileOutputName + "\n";
	s += "Solution of the problem completed";
	char ia[32];
	if (J.stage == 101 && IsValiPTread(J.h.jID)) {
		pthread_cancel(J.h.jID);
		s += ". Scaning was interrupted. ";
	}
	else {
		s += " with an error code ";
		sprintf_s(ia, 32, "%d. ", J.completCode);
		s += ia;
	}
	sprintf_s(ia, 32, "%d", J.stage);
	s += ia;
	s += "% completed.\n";
	s += "Error message: ";
	s += J.errorMessage;
	MessageToServer(s);

	return 0;
}

//-----------------------------------------------------------------------------