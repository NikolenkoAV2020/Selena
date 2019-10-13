// SelenaGlobal.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_GLOBAL
#define SELENA_GLOBAL

#include <ConsolColor.h>
#include <SelenaStrings.h>
#include <SelenaThreads.h>

#if defined(__cplusplus)
	#define ExtC extern "C"
#else
	#define ExtC extern
#endif

// ���������� ������ �� �������� ����� � �������
extern ThrSfBool	waitCommandSignal;

//-----------------------------------------------------------------------------
class ZSelenaConsolOut {
public:
	static char endl[2];

	bool isWait;
	char leftFields;
	char colorKind;		// 0 - ������� ���� 
						// 1 - ��������� ��� ����� �� �������
						// 2 - ������� �������
						// 3 - ������
private:
	static pthread_mutex_t locker;
	static char countStatusRow;

public:
	ZSelenaConsolOut() {
		isWait = 0;
		colorKind = 0;
		leftFields = 0;
	}
	~ZSelenaConsolOut() {}

	void Atr(SelenaMsgType kind = smtText, char fields = 0);
	void Beg(SelenaMsgType kind = smtText, char fields = 0);
	void End();
	void Setfields(char fields);

	ZSelenaConsolOut& operator << (std::string v);
	ZSelenaConsolOut& operator << (int   v);
	ZSelenaConsolOut& operator << (short v);
	ZSelenaConsolOut& operator << (long  v);
	ZSelenaConsolOut& operator << (unsigned int   v);
	ZSelenaConsolOut& operator << (unsigned short v);
	ZSelenaConsolOut& operator << (unsigned long  v);

	static void DoHelpful() {
		pthread_mutex_init(&locker, NULL);
	}

	static void DoNeedless() {
		pthread_mutex_destroy(&locker);
	}

	void BR();
};

//-----------------------------------------------------------------------------

#endif // #ifndef SELENA_GLOBAL
