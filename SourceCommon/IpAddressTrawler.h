// IpAddressTrawler.h
// Walter Artyomenko, September 2019
//
#ifndef IP_ADDRESS_TRAWLER_H
#define IP_ADDRESS_TRAWLER_H

class IpAddressTrawler {
public:
	char  ips[24];

private:
	char* _s = nullptr;
	char* _s0 = nullptr;
	int   k = 0;
	int   ns = 0;

public:
	IpAddressTrawler();
	~IpAddressTrawler();

	char* operator () (char* str, char** nextStr);

private:
	bool IsIpAddress(char** _s, char** _s0, int& k, char* ips);
};

#endif // #ifndef IP_ADDRESS_TRAWLER_H
