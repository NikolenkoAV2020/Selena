// SelenaGlobal.cpp
// Walter Artyomenko, August 2019
//
#include <NetWinUnixCompat.h>
#include <SelenaJob.h>
#include <SelenaStrings.h>
#include <SelenaSafeMessage.h>
#include <SelenaThreads.h>
#include <SelenaGlobal.h>

//-----------------------------------------------------------------------------
// Глобальный сигнал об ожидании ввода с консоли
ThrSfBool	waitCommandSignal;

pthread_mutex_t ZSelenaConsolOut::locker;
char ZSelenaConsolOut::endl[2] = "\n";
char ZSelenaConsolOut::countStatusRow = 0;

//-----------------------------------------------------------------------------
void ZSelenaConsolOut::Atr(SelenaMsgType kind, char fields) 
{
	int colorKind = (int)kind;
	leftFields = fields;
	BR();
	Setfields(fields);
	switch (colorKind) {
		case 0:	std::cout << SELENA_CCLR_SERV_DEF; break;
		case 1: std::cout << SELENA_CCLR_SERV_COM; break;
		case 2: std::cout << SELENA_CCLR_SERV_MSG; break;
		case 3:	std::cout << SELENA_CCLR_SERV_ERR; break;
		case 4: std::cout << SELENA_CCLR_SERV_WAR; break;
	}
}

void ZSelenaConsolOut::Beg(SelenaMsgType kind, char fields)
{
	if (countStatusRow) {
		if ((kind == smtScanState ? 0 : 1)) {
			std::cout << std::endl;
		}
		else {
			std::cout << "\r" << "\x1b[2K";
		}
	} 
	countStatusRow = (kind == smtScanState ? 1 : 0);
	pthread_mutex_lock(&locker);
	isWait = waitCommandSignal->V();
	Atr(kind, fields);
}

void ZSelenaConsolOut::End() 
{
	std::cout << SELENA_CCLR_SERV_DEF;
	if (!countStatusRow) {
		std::cout << std::endl;
	}
	if (isWait) {
		// Возобновление запроса команды
		std::cout << "Selena Server >>>";
		//std::cout << "Selena Client >>>";
	}
	std::cout << std::flush;
	pthread_mutex_unlock(&locker);
}

void ZSelenaConsolOut::Setfields(char fields) 
{
	leftFields = fields;
	BR();
	if (leftFields) {
		std::string sf = "";
		while (fields--) sf += "\t";
		std::cout << sf;
	}
}

ZSelenaConsolOut& ZSelenaConsolOut::operator << (std::string v) 
{
	if (leftFields) {
		std::string s = "\n";
		int i = leftFields;
		while (i--) s += "\t";
		ReplaceStdSubstr(v, "\n", s);
	}
	std::cout << v;
	return *this;
}

ZSelenaConsolOut& ZSelenaConsolOut::operator << (int   v) { std::cout << v; return *this; }
ZSelenaConsolOut& ZSelenaConsolOut::operator << (short v) { std::cout << v; return *this; }
ZSelenaConsolOut& ZSelenaConsolOut::operator << (long  v) { std::cout << v; return *this; }
ZSelenaConsolOut& ZSelenaConsolOut::operator << (unsigned int   v) { std::cout << v; return *this; }
ZSelenaConsolOut& ZSelenaConsolOut::operator << (unsigned short v) { std::cout << v; return *this; }
ZSelenaConsolOut& ZSelenaConsolOut::operator << (unsigned long  v) { std::cout << v; return *this; }

void ZSelenaConsolOut::BR() 
{
	std::cout << "\r                                                 \r";
}

