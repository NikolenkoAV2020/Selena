// SelenaSafeMessage.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_SAFE_MESSAGE
#define SELENA_SAFE_MESSAGE

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stack> 
#include <list> 
#include <queue> 

#include <SelenaSafeObject.h>

enum SelenaMsgType {
	smtText		  = 0,
	smtCommand	  = 1,
	smtMessage    = 2,
	smtError	  = 3,
	smtWarning    = 4,
	smtFile		  = 5,
	smtHello      = 6,
	smtScanState  = 7,
	smtScanMsg    = 8,
	smtScanResult = 9
};

struct ZSelenaMessage {
	std::string		msg = "";
	std::string		msgFileName  = "";
	std::string		storFileName = "";
	SelenaMsgType	type;	//  smtText		
							//  smtCommand
	bool            isPending = false;

	void Clear() {
		msg          = "";
		storFileName = "";
		msgFileName  = "";
		type = smtText;
	}

	bool IsEmpty() { return (msg.length() ? false : true); }
};

//-----------------------------------------------------------------------------
class ZSelenaMessages {
protected:
	std::list<ZSelenaMessage> Q;
	std::string StorageName = "";

public:
	ZSelenaMessages() {
	}

	~ZSelenaMessages() {
		ClearAll();
	}

	ZSelenaMessages(const ZSelenaMessages&)  = delete;
	ZSelenaMessages(const ZSelenaMessages&&) = delete;

	unsigned int Count() {
		return (unsigned int)Q.size();
	}

	void GetList(std::string& s) {
		s = "";
		if (!Q.size()) return;
		auto  e  = Q.end();
		auto  it = e;
		while (1) {
			if (it != e) s += "\n";
			it--;
			s += it->msg;
			if (it == Q.begin()) break;
	}	}

	int Remove(std::vector<int>* n) {
		stdsize s = Q.size();
		if (!s) return 0;
		if (!n) {
			Q.erase(Q.begin(), Q.end());
			return (int)s;
		}

		int  k = 1, d = 0, t =-1;
		auto sn= n->size();
		auto it= Q.end(); 

		for (int j = 0; j < (int)sn; ++j) {
			k = (*n)[j];
			if (k > (int)s) break;
			if (k == t) continue;
			t = k;
			it= Q.end();
			int l = k - d;
			while (l--) it--;
			Q.erase(it);
			d++;
 		}
		return d;
	}

	unsigned int GetFirst(ZSelenaMessage& M) {
		unsigned int s = (unsigned int)Q.size();
		if (!s) {
			M.Clear();
			return 0;
		}
		M = Q.front();
		return s;
	}

	unsigned int Set(ZSelenaMessage& M) {
		unsigned int s = (unsigned int)Q.size();
		if (!s) return 0;
		Q.front() = M;
		return s;
	}

	// Забрать первый элемент из очереди
	void Pop(ZSelenaMessage& M, int* count = nullptr) {
		if (Q.empty()) {
			M.Clear(); return;
		} else M = Q.front();
		Q.erase(Q.begin());
		if (count) *count = (int)Q.size();
	}

	// Положить сообщение в очередь
	void Push(ZSelenaMessage& M, int* count = nullptr) {
		Q.push_back(M);
		if (count) *count = (int)Q.size();
	}

	void ClearAll() {
		Q.clear();
	}

	void HoldOver() {
		if (!StorageName.length()) return;

		std::string   s; 
		std::ofstream file;
		file.open(StorageName, std::ios::out | std::ios::trunc);
		if (!file.is_open()) return;

		auto  i = Q.begin();
		while (i != Q.end()) {
			file << i->msg << std::endl;
			file << i->msgFileName << std::endl;
			file << (int)i->type << std::endl;
			i++;
	}	}

	void Restory(const std::string& storage = "") {
		if ( storage.length()    ) StorageName = storage;
		if (!StorageName.length()) return;
		ClearAll();

		std::string   s; 
		std::ifstream file(StorageName, std::ofstream::in);
		if (!file.is_open()) return;

		ZSelenaMessage M;
		while (1) {
			M.Clear();
			getline(file, s); if (file.eof()) break;
			M.msg = s;
			getline(file, s); if (file.eof()) break;
			M.msgFileName = s;
			int t = 0;
			file >> t;		  if (file.eof()) break;
			M.type = (SelenaMsgType)t;
			Push(M);
		}
		file.close();
	}
};

//-----------------------------------------------------------------------------
typedef ZSelenaSafeObject<bool>				ThrSfBool;
typedef ZSelenaSafeObject<ZSelenaMessages>	ThrSfSelenaMessages;

//-----------------------------------------------------------------------------
#endif // #ifndef SELENA_SAFE_MESSAGE