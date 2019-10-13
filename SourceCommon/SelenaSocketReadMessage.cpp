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

	// Далее считывание самого файла частями во 4kb и одновременная отправка
	// прочитанного в адрес сокета
	char buf[4097];
	while (1) {
		// Операция чтения части файла
		file.read(buf, 4096);
		// определение реально прочитанного колличества байт
		std::streamsize curSize = file.gcount();
		if (curSize > 0) fileSize += (unsigned int)curSize;
		if (!curSize || file.eof()) {
			// Чтение и отправка файла завершены
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
		// Нет достаточных данных для чтения из сокета
		return -1;
	}

	//-------------------------------------------------------------------------
	// Read header
	readBytes = (uint32_t)recv(srcSocket, (char*)(&H), headerSize, MSG_WAITALL);
	if (readBytes != (uint32_t)headerSize || !H.Verify()) {
		// Данные не известного формата
		// Изъятие данных из потока 
		// Данные игнорируются 
		//do {
		//	readBytes = (int)recv(srcSocket, buffer, SELENA_READBUFFSIZE, MSG_WAITALL);
		//	if (readBytes == 0) break;
		//	if (readBytes == (uint32_t)SOCKET_ERROR) break;
			// В считанных данных из потока целесообразно искать заголовок
			// сообщения. Так-как среди ошибочных данных может появиться
			// формализованное сообщение, начинающиеся со структуры ZSelenaMsgHeader



		//} while (readBytes > 0);
		return -201;
	}

	if (!H.Size && !H.fileSize) {
		// Данные сообщения отсутствуют 
		return -202;
	}

	// Если в потоке данных заголовок определён, то начинается считывание
	// массива байт заданной в заголовке длинны
	if (coutEcho) {
		ZSelenaConsolOut SO;
		SO.Beg(smtText, 1);
			SO << "Data received: ";
		SO.End();
	}

	M.type = (SelenaMsgType)H.type;

	// Резервируем память в строке результата считывания
	std::string s = "";
	if (s.capacity() < 1024) s.reserve(1024);

	//-------------------------------------------------------------------------
	if (H.Size) {
		while (1) {
			// Размер читаемой порции данных из потока сокета
			ssize_t reedLen = H.Size > (allBytes + SELENA_READBUFFSIZE) ?
							  SELENA_READBUFFSIZE : (H.Size - allBytes);

			if (reedLen <= 0) {
				// Все данные заявленные в заголовке сообщения прочитаны,
				// значит цыкл чтения завершается.
				break;
			}

			// Считывание порции длинной не более READBUFFSIZE
			readBytes = (uint32_t)recv(srcSocket, buffer, reedLen, 0);
			if (readBytes == 0) {
				// Если попытка считывания очередной порции данных приводит
				// к возврату 0, то это значит что весь входной поток данных сокета
				// себя исчерпал и сообщение возможно прочитано полностью
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
				// Обнаружена ошибка считывания данных из сокета
				return HandleSocketError("recv", nullptr, 0);
			}

			// Увеличение общего числа прочитанных байт
			allBytes += readBytes;
			// Завершение строки прочитанных байт нулём
			buffer[readBytes] = '\0';
			// Отображение на консоли, если это необходимо
			//if (coutEcho) {
			//	printf("%s", buffer);
			//}

			// Резервирование дополнительной памяти в строке результатов,
			// если это необходимо.
			if (allBytes >= (uint32_t)s.capacity()) {
				s.reserve(s.capacity() + 1024);
			}
			// Аккамулирование полученных из сокета данных.
			s += std::string(buffer, readBytes);
		}
		if (coutEcho) std::cout << std::endl;
	}

	M.msg = s;

	//-------------------------------------------------------------------------
	// Чтение входящего файла если он задан в заголовке сообщения 
	if (H.fileSize) {
		std::ofstream file;
		int k, n;

		// Проверка наличия ключевой фразы.
		int keyLenght = strlen(SELENA_BEG_FILE_KEY);
		// Считывание порции длинной не более keyLenght
		readBytes = (uint32_t)recv(srcSocket, buffer, keyLenght, 0);
		buffer[keyLenght] = '\0';
		if (readBytes != (uint32_t)keyLenght || strcmp(buffer, SELENA_BEG_FILE_KEY)) {
			// Передаваемый файл в потоке отсутствует.
			// Возвращается ошибка, имени файла присваивается пустая строка.
			return -204;
		}

		// Считывание имени файла
		k = sizeof(int);
		n = (int)recv(srcSocket, buffer, k, 0);
		if (n != k) goto m_error;
		memcpy(&k, buffer, n);

		n = (int)recv(srcSocket, buffer, k, 0);
		if (n != k) goto m_error;
		buffer[n] = '\0';
		M.msgFileName = buffer;

		//---------------------------------------------------------------------
		// создание файла для сохранения считанных данных
		if (!M.msgFileName.length()) M.msgFileName = inputFileDefaultName;

		file.open(M.msgFileName, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open()) {
m_error:;
			// Отображаем в консоли сообщение об ошибке
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
			// Размер читаемой порции данных из потока сокета
			ssize_t reedLen = H.fileSize > (allBytes + SELENA_READBUFFSIZE) ?
							  SELENA_READBUFFSIZE : (H.fileSize - allBytes);

			if (reedLen <= 0) {
				// Все данные заявленные в заголовке сообщения прочитаны,
				// значит цыкл чтения завершается.
				break;
			}

			// Считывание порции длинной не более READBUFFSIZE
			readBytes = (uint32_t)recv(srcSocket, buffer, reedLen, 0);
			if (readBytes == 0) {
				// Если попытка считывания очередной порции данных приводит
				// к возврату 0, то это значит что весь входной поток данных сокета
				// себя исчерпал и сообщение возможно прочитано полностью
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
				// Обнаружена ошибка считывания данных из сокета
				return HandleSocketError("recv", nullptr, 0);
			}

			// Увеличение общего числа прочитанных байт
			allBytes += readBytes;
			// Завершение строки прочитанных байт нулём
			buffer[readBytes] = '\0';
			// Отображение на консоли, если это необходимо
			//if (coutEcho) {
			//	printf("%s", buffer);
			//}

			// полученные данные записываются в файл
			file.write(buffer, (std::streamsize)readBytes);

			// Проверка достигнут ли конец файла
			if (buffer[readBytes - 1] == EOF) {
				break;
		}	}

		file.close();

		//---------------------------------------------------------------------
		keyLenght = strlen(SELENA_END_FILE_KEY);
		// Считывание порции длинной не более keyLenght
		readBytes = (uint32_t)recv(srcSocket, buffer, keyLenght, 0);
		buffer[keyLenght] = '\0';
		if (readBytes != (uint32_t)keyLenght || strcmp(buffer, SELENA_END_FILE_KEY)) {
			// В потоке отсутствует признак завершения передаваемого файла.
			// Возвращается ошибка, имени файла присваивается пустая строка.
			M.msgFileName = "";
			return -207;
	}	}

	return r;
}

//-----------------------------------------------------------------------------