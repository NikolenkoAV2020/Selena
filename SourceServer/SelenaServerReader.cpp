// SelenaServerReader.cpp
// Walter Artyomenko, August 2019
//
// ������� ������ ������ ��������� ���������� ����� ����� �� ������.
// ��� ������� �����-���� ������ � ������ ������ ��������� �����������,
// ����������� � ��������� ���������, �� ��������� �������� ����� ���������
// ����������� �� ��������� � ������������ � ������� �������.
// �� ���� ������ ������� �� ������ �������� ��������� �� ������� 
// �� � ������������ ��.
//
// ���������� ������� ������ ����������� ���� � ������ ����������� �������
// � ������������� ���������� ������ �������, ���� � ������ ��������� ���������
// �� ������� "exit" ��� ��� ���������, ������������������ � ���������� ��������
// ����� ������.
//-----------------------------------------------------------------------------
#include "SelenaServer.h"

//-----------------------------------------------------------------------------
void* ReaderMessage(void *args)
{
	ZSelenaSocket* pServer = (ZSelenaSocket*)args;
	// If the server address is not passed, 
	// then immediately terminate the function and the corresponding thread.
	if (!pServer) return nullptr;

	ZSelenaMessage M;

	// ����� ������ �� ������� ������� ��������� ���������
	// � ��� ��� ������� ����������.
	while (1) {
		// Checking the arrival of a signal to complete all threads 
		// and close the server application
		if (stopSignal->V()) break;

		int r = 0;

		// ���������� ��������� �� ������� �� ���������
		M.Clear();
		clientMsg->V().Pop(M);

		// �������� ���� �� ���������?
		if (!M.IsEmpty()) {
			// ���������� �������� ��������� �� �������.
			// ����������� ����������� ���������.
			ZSelenaConsolOut SO;
			SO.Beg(smtText, 2);
				SO << "Selena Client:\n";
			SO.Atr(smtMessage, 3);
				SO << M.msg << SO.endl;
				if (M.msgFileName.length()) {
					SO << "file: " << M.msgFileName;
				}
			SO.End();
		}

		if (IsExitCommand(M.msg)) {
			// �� ������� �������� ��������� � ����������
			// ����������� ���������� - ��������� �� ��������
			// ���������� ���� �������.
			// ������ ��������� �� ��������� � ��������� ��������
			// � ������� ������ ���������� ������ ��������� ���������
			// �� ������� �������. ������� ������� ��������� �� �������������
			// �� ������� � ��������� ��������� ������������.
			// ���������� ������ � ���������� ���������� ����������.
			stopSignal->V() = true;
			break;
		}

		// If the connection was broken, then reading anything from the socket
		// stream is pointless. Therefore, we skip the iteration of the loop.
		if (pServer->isDisconnectError) continue;

		// �������� ������� ������ �� ������� ������ ������
		// � ����������� ������� (byteInStream) ������� ������
		unsigned int byteInStream = 0;
		r = pServer->ByteForRead(byteInStream, pServer->GetSubSocket());
		if (r) {
			// ����� ����� ������������ ������ �� ���������� ���� �������
			// � �������� ������ ���������� � ��������.


		}
		if (!byteInStream) continue;

		// ������ �� ������ ������������ �� ������� ���������
		r = pServer->ReadIncoming(M);

		// ���� ��������� ������ ��� ��� ��� ���������� ��������� ������, 
		// �� ��������� �� ��������������
		if (r || M.IsEmpty()) continue;

		// ����� ��������� �� ������� ���������� � ��������� ���������
		// ����� ���� �������������� �������� ������� ���������
		clientMsg->V().Push(M);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------