// SelenaServerReader.cpp
// Walter Artyomenko, August 2019
//
// Функция потока чтения сообщений получаемых через сокет от клинта.
// При наличии каких-либо данных в потоке сокета сообщение считывается,
// сохраняется в хранилище сообщений, на следующей итерации цикла сообщение
// извлекается из хранилища и отображается в консоли сервера.
// То есть данная функция не только получает сообщения от клиента 
// но и обрабатывает их.
//
// Завершение функции потока выполняется либо в случае обнаружения сигнала
// о необходимости завершении работы сервера, либо в случае получения сообщения
// от клиента "exit" или ему подобного, свидетельствующего о завершении клиентом
// своей работы.
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

	// Опрос сокета на предмет наличия входящего сообщения
	// и при его наличии считывание.
	while (1) {
		// Checking the arrival of a signal to complete all threads 
		// and close the server application
		if (stopSignal->V()) break;

		int r = 0;

		// Извлечение сообщения от клиента из хранилища
		M.Clear();
		clientMsg->V().Pop(M);

		// Проверка есть ли сообщение?
		if (!M.IsEmpty()) {
			// Обнаружено входящее сообщение от клиента.
			// Отображение полученного сообщения.
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
			// От клиента получено сообщение о завершении
			// клиентского приложения - программы на удалённом
			// компьютере была закрыта.
			// Данное сообщение не нуждается в обработке сервером
			// и требует только завершения потока обработки сообщений
			// от данного клиента. Поэтому счётчик сообщений не увеличивается
			// на единицу и сообщение считается обработанным.
			// Генерируем сигнал о завершении серверного приложения.
			stopSignal->V() = true;
			break;
		}

		// If the connection was broken, then reading anything from the socket
		// stream is pointless. Therefore, we skip the iteration of the loop.
		if (pServer->isDisconnectError) continue;

		// Проверка наличия данных во входном потоке сокета
		// и определения размера (byteInStream) входных данных
		unsigned int byteInStream = 0;
		r = pServer->ByteForRead(byteInStream, pServer->GetSubSocket());
		if (r) {
			// Здесь нужно инициировать сигнал на перезапуск всех потоков
			// и создание нового соединения с Клиентом.


		}
		if (!byteInStream) continue;

		// Чтение из сокета поступившего от клиента сообщения
		r = pServer->ReadIncoming(M);

		// Если сообщение пустое или при его считывании произошла ошибка, 
		// то сообщение не обрабатывается
		if (r || M.IsEmpty()) continue;

		// Любое сообщение от клиента помещается в хранилище сообщений
		// после чего обрабатывается основным потоком программы
		clientMsg->V().Push(M);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------