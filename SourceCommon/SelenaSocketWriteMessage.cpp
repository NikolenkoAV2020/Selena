// SelenaSocketReadMessage.cpp 
// Walter Artyomenko, August 2019
//
// In this file located implementation one of the most important functions 
// of Selena project, videlicet, sending a message through a socket.
//_____________________________________________________________________________
// 
#include <NetWinUnixCompat.h>
#include <ConsolColor.h>
#include <SelenaSocket.h>
#include <SelenaStrings.h>
#include <SelenaGlobal.h>

//_____________________________________________________________________________
// Is redy socket to write/read data
bool ZSelenaSocket::IsReady(SOCKET readerSocket)
{
	if (!readerSocket) readerSocket = RWS();
	// If invalid socket.
	if (!readerSocket) return false;
	// If early connection was corrupted.
	if (isDisconnectError) return false;

	// If isDisconnectError = false but threadConnection != nullptr
	// that reconnection thread is finish successful.
	// Delete reconnection thread.
	flowConnector.WaitFinish();

	return true;
}

//_____________________________________________________________________________
// Write Selena message to socket
int ZSelenaSocket::WriteMsg(
	ZSelenaMessage& M,				// Message 
	SOCKET		    readerSocket)	// The socket to write the message
{
	// Test several obvious errors

	// If socket not ready to work.
	if (!IsReady(readerSocket)) return-1;
	// If message and file name is absent, then nothing will be sent.
	if (!M.msg.length() && !M.msgFileName.length()) return 0;


	bool isMsgOk   = false;		// Is test of message send Ok?
	bool isFileOk  = false;		// Is file of message send Ok?
	int  sentBytes = 0;			// The count sended bytes.
	int  needSend  = 0;			// The count bytes that need send
	int  r;

	// Sending message lenght
	unsigned int sendstrlen = (unsigned int)M.msg.length();

	//_________________________________________________________________________
	// File for sending
	std::ifstream file;
	unsigned int  lengthFile = 0;

	// if the file name is specified
	if (M.msgFileName.length()) {
		// Try open file
		int r = PreReadFile(M.msgFileName, lengthFile);
		if (r < 0) {
			// Show in consol message about error
			ZSelenaConsolOut SO;
			SO.Beg(smtError, 1);
				SO << "Error send message: file "
				   << M.msgFileName << " not open or not exist." << SO.endl
				   << "Message not sent.";
			SO.End();
			return -101;
		}
		if (!lengthFile) {
			// Show in consol message about error
			ZSelenaConsolOut SO;
			SO.Beg(smtWarning, 1);
				SO << "Warning: file " << M.msgFileName << "is empty.";
			SO.End();
	}	}

	//_________________________________________________________________________
	// Post message header
	ZSelenaMsgHeader H(sendstrlen, lengthFile, (SelenaMsgType)M.type, M.isPending);

	// Send header of message
	r = SelfSend(readerSocket, (char*)&H, sizeof(H)); 
	if (r) {
		M.isPending = true;
		return r;
	}

	//_________________________________________________________________________
	// Sending text of message
	if (sendstrlen) {
		r = SelfSend(readerSocket, M.msg); 
		if (r) {
			M.isPending = true;
			return r;
		}
		isMsgOk = true;
	}

	//_________________________________________________________________________
	// Sending files of message
	if (M.msgFileName.length()) {
		// attemption file open 
		file.open(M.msgFileName, std::ios_base::in);
		if (!file.is_open()) {
			// We display an error message in the console.
			ZSelenaConsolOut SO;
			SO.Beg(smtError, 1);
				SO << "Error send message: file " 
				   << M.msgFileName << " not open or not exist.";
			SO.End();
			// In this situation, we do not complete the procedure,
			// but send a zero length file. 
		}

		// Sending to the socket "tag" of the file transfer beginning
		r = SelfSend(readerSocket, SELENA_BEG_FILE_KEY, strlen(SELENA_BEG_FILE_KEY));
		if (r) {
			M.isPending = true;
			// Even if there was an error sending the file, 
			// before exit from here with error don't forget close the file.
			file.close();
			return r;
		}

		char buf[4097];
		unsigned int countSendingBytes = 0;

		// Send name of result file
		needSend = M.storFileName.length();
		if (!needSend) {
			M.storFileName = "SelenaClientResult";
			needSend = M.storFileName.length();
		}
		int k = sizeof(int);
		memcpy(buf, &needSend, k);
		memcpy(&buf[k], M.storFileName.c_str(), needSend);
		needSend += k;
		// actually sending
		r = SelfSend(readerSocket, buf, needSend);
		if (r) {
			M.isPending = true;
			// So... don't forget ... you already know.
			file.close();
			return r;
		}

		if (file.is_open()) {
			// Next, reading the file itself in 4kb parts and simultaneously 
			// sending the read to socket
			while (1) {
				// Part of the file reading operation
				file.read(buf, 4096);
				// determination of the actually read bytes number 
				std::streamsize curSize = file.gcount();
				if (curSize > 0) {
					// sending read byte array to socket
					sentBytes = (int)send(readerSocket, buf, (int)curSize, 0);
					if (sentBytes <= 0) {
						M.isPending = true;
						// So... don't forget ... you already know.
						file.close();
						// exit
						return HandleError("send");
					}
					countSendingBytes += sentBytes;
				}
				if (!curSize || file.eof()) {
					// Reading and sending file completed
					break;
			}	}
			file.close();
		} else {
			// Report the error in sending it to the file.
			// Instead of a file that could not be opened, 
			// we send an error message.
			r = SelfSend(readerSocket, "Error sending file", 18); 
			if (r) {
				M.isPending = true;
				return r;
		}	}

		// Sending file transfer completion flag to socket
		r = SelfSend(readerSocket, SELENA_END_FILE_KEY, strlen(SELENA_END_FILE_KEY)); 
		if (r) {
			M.isPending = true;
			return r;
		}

		if (countSendingBytes != lengthFile) {
			// The number of bytes sent to the socket 
			// does not match the file length.
			// A warning is issued to the console.
			ZSelenaConsolOut SO;
			SO.Beg(smtWarning, 1);
				SO << "Warning send file: "
				   << "Not all file " << M.msgFileName << " was transferred.";
			SO.End();
		}
		else isFileOk = true;
	}

	//_________________________________________________________________________
	// Duplicate the sent message on the console.
	// It is not obligatory.
	if (coutEcho) {
		if (isMsgOk && sendstrlen > 0) {
			ZSelenaConsolOut SO;
			SO.Beg(smtText, 1);
				SO << "Send message:\n";
			SO.Atr(smtMessage, 2);
				SO << M.msg << SO.endl;
				if (isFileOk && lengthFile) {
					SO << M.msgFileName << " file was attached to the message";
				}
			SO.End();
	}	}

	return 0;
}

//_____________________________________________________________________________
// elementary operation of sending data to a socket
int ZSelenaSocket::SelfSend(SOCKET &socket, const char* M, int needSend)
{
	int sentBytes = 0;
	while (1) {
		// Trying send some bytes to socket.
		sentBytes = (int)send(socket, (char*)M, needSend, 0);
		// Checking. If error sending?
		if (sentBytes <= 0) {
			int r = HandleError("send");
			// If happen any error automaticaly start reconnect procedure.
			HotReConnect();
			return r;
		}
		// Checking. If all data is sended?
		if (sentBytes == needSend) break;
		// decrease size of data that need send.
		needSend -= sentBytes;
		// Move pointer of sending data to next send bytes.
		M = ((char*)M) + sentBytes;
	}
	return 0;
}

int ZSelenaSocket::SelfSend(SOCKET &socket, std::string& M)
{
	return SelfSend(socket, M.c_str(), M.length());
}

//_____________________________________________________________________________
//