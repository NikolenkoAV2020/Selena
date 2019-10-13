// SelenaSocketReadMessage.cpp 
// Walter Artyomenko, August 2019
//
#include <NetWinUnixCompat.h>
#include <ConsolColor.h>
#include <SelenaSocket.h>
#include <SelenaStrings.h>
#include <SelenaGlobal.h>

//-----------------------------------------------------------------------------
int ZSelenaSocket::PreReadFile(
	const std::string&	fileName, 
	unsigned int&		fileSize)
{
	std::ifstream file;
	fileSize = 0;

	file.open(fileName, std::ios_base::in);
	if (!file.is_open()) return -1;

	// ����� ���������� ������ ����� ������� �� 4kb � ������������� ��������
	// ������������ � ����� ������
	char buf[4097];
	while (1) {
		// �������� ������ ����� �����
		file.read(buf, 4096);
		// ����������� ������� ������������ ����������� ����
		std::streamsize curSize = file.gcount();
		if (curSize > 0) fileSize += (unsigned int)curSize;
		if (!curSize || file.eof()) {
			// ������ � �������� ����� ���������
			break;
	}	}

	file.close();
	return 0;
}

//-----------------------------------------------------------------------------
int ZSelenaSocket::ReadMsg(
	ZSelenaMessage& M,			// Message 
	SOCKET		    srcSocket)	// The socket to read the message from
{
	M.Clear();

	if (!srcSocket) return 0;

	// Here we use "ssize_t" type for compatable whith Linux.
	// In Windows this type defined as "int"
	char	 buffer[SELENA_READBUFFSIZE + 1];// Buffer for incomming data 
	uint32_t readBytes;						 // Size of received message  
	uint32_t allBytes = 0;					 // Size of the entire read data array 
	int      r = 0;

	// Message header
	ZSelenaMsgHeader H;
	unsigned int	 headerSize = sizeof(H);

	//-------------------------------------------------------------------------
	// Checking is there data in a socket
	unsigned int byteInStream;
	r = ByteForRead(byteInStream, srcSocket);
	if (r == SOCKET_ERROR) return HandleError("Read message");
	if (byteInStream < headerSize) {
		// ��� ����������� ������ ��� ������ �� ������
		return -1;
	}

	//-------------------------------------------------------------------------
	// Read header
	readBytes = (uint32_t)recv(srcSocket, (char*)(&H), headerSize, MSG_WAITALL);
	if (readBytes != (uint32_t)headerSize || !H.Verify()) {
		// ������ �� ���������� �������
		// ������� ������ �� ������ 
		// ������ ������������ 
		//do {
		//	readBytes = (int)recv(srcSocket, buffer, SELENA_READBUFFSIZE, MSG_WAITALL);
		//	if (readBytes == 0) break;
		//	if (readBytes == (uint32_t)SOCKET_ERROR) break;
			// � ��������� ������ �� ������ ������������� ������ ���������
			// ���������. ���-��� ����� ��������� ������ ����� ���������
			// ��������������� ���������, ������������ �� ��������� ZSelenaMsgHeader



		//} while (readBytes > 0);
		return -201;
	}

	if (!H.Size && !H.fileSize) {
		// ������ ��������� ����������� 
		return -202;
	}

	// ���� � ������ ������ ��������� ��������, �� ���������� ����������
	// ������� ���� �������� � ��������� ������
	if (coutEcho) {
		ZSelenaConsolOut SO;
		SO.Beg(smtText, 1);
			SO << "Data received: ";
		SO.End();
	}

	M.type = (SelenaMsgType)H.type;

	// ����������� ������ � ������ ���������� ����������
	std::string s = "";
	if (s.capacity() < 1024) s.reserve(1024);

	//-------------------------------------------------------------------------
	if (H.Size) {
		while (1) {
			// ������ �������� ������ ������ �� ������ ������
			ssize_t reedLen = H.Size > (allBytes + SELENA_READBUFFSIZE) ?
							  SELENA_READBUFFSIZE : (H.Size - allBytes);

			if (reedLen <= 0) {
				// ��� ������ ���������� � ��������� ��������� ���������,
				// ������ ���� ������ �����������.
				break;
			}

			// ���������� ������ ������� �� ����� READBUFFSIZE
			readBytes = (uint32_t)recv(srcSocket, buffer, reedLen, 0);
			if (readBytes == 0) {
				// ���� ������� ���������� ��������� ������ ������ ��������
				// � �������� 0, �� ��� ������ ��� ���� ������� ����� ������ ������
				// ���� �������� � ��������� �������� ��������� ���������
				if (coutEcho) {
					ZSelenaConsolOut SO;
					SO.Beg(smtWarning, 1);
						SO << "End of data. " << allBytes
						   << " bytes have been read. The connection was broken.";
					SO.End();
					r = -203;
				}
				break;
			}
			if (readBytes == (uint32_t)SOCKET_ERROR) {
				// ���������� ������ ���������� ������ �� ������
				return HandleSocketError("recv", nullptr, 0);
			}

			// ���������� ������ ����� ����������� ����
			allBytes += readBytes;
			// ���������� ������ ����������� ���� ����
			buffer[readBytes] = '\0';
			// ����������� �� �������, ���� ��� ����������
			//if (coutEcho) {
			//	printf("%s", buffer);
			//}

			// �������������� �������������� ������ � ������ �����������,
			// ���� ��� ����������.
			if (allBytes >= (uint32_t)s.capacity()) {
				s.reserve(s.capacity() + 1024);
			}
			// ��������������� ���������� �� ������ ������.
			s += std::string(buffer, readBytes);
		}
		if (coutEcho) std::cout << std::endl;
	}

	M.msg = s;

	//-------------------------------------------------------------------------
	// ������ ��������� ����� ���� �� ����� � ��������� ��������� 
	if (H.fileSize) {
		std::ofstream file;
		int k, n;

		// �������� ������� �������� �����.
		int keyLenght = strlen(SELENA_BEG_FILE_KEY);
		// ���������� ������ ������� �� ����� keyLenght
		readBytes = (uint32_t)recv(srcSocket, buffer, keyLenght, 0);
		buffer[keyLenght] = '\0';
		if (readBytes != (uint32_t)keyLenght || strcmp(buffer, SELENA_BEG_FILE_KEY)) {
			// ������������ ���� � ������ �����������.
			// ������������ ������, ����� ����� ������������� ������ ������.
			return -204;
		}

		// ���������� ����� �����
		k = sizeof(int);
		n = (int)recv(srcSocket, buffer, k, 0);
		if (n != k) goto m_error;
		memcpy(&k, buffer, n);

		n = (int)recv(srcSocket, buffer, k, 0);
		if (n != k) goto m_error;
		buffer[n] = '\0';
		M.msgFileName = buffer;

		//---------------------------------------------------------------------
		// �������� ����� ��� ���������� ��������� ������
		if (!M.msgFileName.length()) M.msgFileName = inputFileDefaultName;

		file.open(M.msgFileName, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open()) {
m_error:;
			// ���������� � ������� ��������� �� ������
			ZSelenaConsolOut SO;
			SO.Beg(smtError, 1);
				SO << "Error read message: file "
				   << M.msgFileName << " not created.";
			SO.End();
			return -205;
		}

		//---------------------------------------------------------------------
		allBytes = 0;
		while (1) {
			// ������ �������� ������ ������ �� ������ ������
			ssize_t reedLen = H.fileSize > (allBytes + SELENA_READBUFFSIZE) ?
							  SELENA_READBUFFSIZE : (H.fileSize - allBytes);

			if (reedLen <= 0) {
				// ��� ������ ���������� � ��������� ��������� ���������,
				// ������ ���� ������ �����������.
				break;
			}

			// ���������� ������ ������� �� ����� READBUFFSIZE
			readBytes = (uint32_t)recv(srcSocket, buffer, reedLen, 0);
			if (readBytes == 0) {
				// ���� ������� ���������� ��������� ������ ������ ��������
				// � �������� 0, �� ��� ������ ��� ���� ������� ����� ������ ������
				// ���� �������� � ��������� �������� ��������� ���������
				if (coutEcho) {
					ZSelenaConsolOut SO;
					SO.Beg(smtWarning, 1);
						SO << "End of data. " << allBytes
						   << " bytes have been read. The connection was broken.";
					SO.End();
					r = -206;
				}
				break;
			}
			if (readBytes == (uint32_t)SOCKET_ERROR) {
				// ���������� ������ ���������� ������ �� ������
				return HandleSocketError("recv", nullptr, 0);
			}

			// ���������� ������ ����� ����������� ����
			allBytes += readBytes;
			// ���������� ������ ����������� ���� ����
			buffer[readBytes] = '\0';
			// ����������� �� �������, ���� ��� ����������
			//if (coutEcho) {
			//	printf("%s", buffer);
			//}

			// ���������� ������ ������������ � ����
			file.write(buffer, (std::streamsize)readBytes);

			// �������� ��������� �� ����� �����
			if (buffer[readBytes - 1] == EOF) {
				break;
		}	}

		file.close();

		//---------------------------------------------------------------------
		keyLenght = strlen(SELENA_END_FILE_KEY);
		// ���������� ������ ������� �� ����� keyLenght
		readBytes = (uint32_t)recv(srcSocket, buffer, keyLenght, 0);
		buffer[keyLenght] = '\0';
		if (readBytes != (uint32_t)keyLenght || strcmp(buffer, SELENA_END_FILE_KEY)) {
			// � ������ ����������� ������� ���������� ������������� �����.
			// ������������ ������, ����� ����� ������������� ������ ������.
			M.msgFileName = "";
			return -207;
	}	}

	return r;
}

//-----------------------------------------------------------------------------