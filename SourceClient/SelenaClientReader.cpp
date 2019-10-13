// SelenaClientReader.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
void* ReaderCommand(void *args)
{
	ZSelenaSocket* pClient = (ZSelenaSocket*)args;
	if (!pClient) return nullptr;

	// ����� ������ �� ������� ������� ��������� ���������
	// � ��� ��� ������� ����������.
	while (1) {
		// Checking the signal about all threads complete
		if (stopSignal->V()) break;
		if (pClient->isDisconnectError)	continue;

		ZSelenaMessage M;

		// Is there anything in the socket stream for reading
		unsigned int byteInStream;
		int r = pClient->ByteForRead(byteInStream);
		if (r) {
			// ��� ����� ���� ������ �� �������������� ����������
			// � �������� � ���������� ���� �������!


			break;
		}
		if (!byteInStream) 
			// Nothint there is, therfor skip loops iteration
			continue;

		// ������ �� ������ ������������ �� ������� ���������
		// It's may by as a text message as a command.
		pClient->coutEcho = false;
		r = pClient->ReadIncoming(M);
		pClient->coutEcho = true;
		if (r) {
			// 

		}

		// If the message is empty or an error occurred while reading it, 
		// then message is not processed.
		if (r || !M.msg.length()) {
			// Just skip loop iteration.
			continue;
		}

		// Verification. Is the command a text message
		if (IsSendCommand(M.msg)) {
			// A similar message is simply displayed in the console
			int j = CommandFindWord(M.msg, "send");
			// Exclude keyword "send" from the command
			M.msg = M.msg.substr(j + 4);
			// Transform to usually text message.
			M.type = smtText;
		}

		// Show simple text message
		if (M.type == smtText) {
			// Show message
			EchoMessageToConsol(M);
			// Neither pushing nor duplicating the server 
			// receiving such a command is not necessary
			continue;
		}

		// All other messages are regarded as commands.
		M.type = smtCommand;
		EchoMessageToConsol(M);

		//---------------------------------------------------------------------
		// ��������� ������� ������ ����������� ���������� � �� ��������� 
		// � ��������� ������ ��� ����������� ����������.
		// Immediate command execution.
		//---------------------------------------------------------------------
		
		// �������� ����������� ������� �� ���������� ����������� ����������.
		if (IsExitCommand(M.msg)) {
			// �� ������� �������� ��������� � ���������� ������.
			// ������ ��������� �� ��������� � ��������� ��������
			// � ������� ������ ���������� ������ ��������� �������,
			// ����������� �� �������. 
			// ������������� ������ � ���������� ����������� ����������
			stopSignal->V() = true;
			// ��������� ���� ������ ��������� ������ ������ �
			// ��� ����� ��������� ����� ������ �������� ���������
			break;
		}

		// �������� ����������� ������� �� ���������� ����������� ����������.
		if (IsRestartCommand(M.msg)) {
			RestartClient();
			break;
		}

		r = ExecutFastCommand(M, pClient);
		if (!r) {
			// ���� ���������� � ��������� �������
			continue;
		} 
		if (r > 0) {
			// ��������� ������ ��� ���������� �������
		}
		r = 0;

		//---------------------------------------------------------------------
		// ������������� ��������� ������� ��� �������
		EchoCommandToServer(M);

		//---------------------------------------------------------------------
		// �������� ������� �� ������ ���������� ������ masscan,
		// � ����� ������� � ������� �������� �����
		if (IsMasscanCommand(M.msg)) {

		} else {
			// Received message is not a masscan/nmap command.
			std::string s = "Unknown command to execute: \n";
			s += M.msg;
			s += "...";
			MessageToServer(s, "", smtError);
			continue;
		}

		// ��� ���������� ������� �� ������� ���������� � ��������� ������
		// ����� ���� �������������� �������� ������� ���������.
		r = clientJobsStorage->V().PutCommandToJobStorage(M.msg, M.msgFileName);
		if (r) {
			std::string s = "Error saving received command (" + M.msg + ")";
			MessageToServer(s, "", smtWarning);
		}
		//clientCom->V().Push(M);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// ������������ ������� ��������� �� ���� �������
void EchoCommandToServer(ZSelenaMessage& M)
{
	ZSelenaMessage sendM;
	sendM.msg = "Received command: \n";
	sendM.msg += M.msg;
	sendM.type = smtText;
	clientMsg->V().Push(sendM);
}

//-----------------------------------------------------------------------------
void EchoMessageToConsol(ZSelenaMessage& M)
{
	ZSelenaConsolOut SO;
	std::string		 entity = (M.type == smtCommand ? "command" : "message");

	SO.Beg(smtText, 2);
		SO << "Received from server: " << SO.endl;
	SO.Atr(M.type, 3);
		SO << entity << ": " << M.msg;
		if (M.msgFileName.length()) {
			SO << "\n" << "file transferred with " << entity
			   << ": " << M.msgFileName;
		}
	SO.End();
}

//-----------------------------------------------------------------------------