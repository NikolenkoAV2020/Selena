// IpAddressTrawler.cpp
// Walter Artyomenko, September 2019
//
// What is an IP address trawler for? 
// If I have same text file or long text string and I would wanna find
// ip-addresses in it than I simple create IpAddressTrawler object
//
//		IpAddressTrawler T;
//
// and execute procedure searching something similar to ip-address..
//		char* str;		// pointer to test string where searching ip-address
//		char* nextStr;	// pointer to beginning where will search next ip-address from.
//		char* ipStr;	// pointer to found ip-address, if length of ipStr equal zero 
//						// then ip-address not found.
//		ipStr = T.(str, &nextStr); 
//		. . . . . . . . . . . . 
//		str = nextStr;
//		ipStr = T.(str, &nextStr); 
//_____________________________________________________________________________

#include <cstring>
#include <IpAddressTrawler.h>

IpAddressTrawler::IpAddressTrawler() 
{
	ips[0] = '\0';
}
IpAddressTrawler::~IpAddressTrawler() 
{
}

//_____________________________________________________________________________
//
char* IpAddressTrawler::operator () (char* str, char** nextStr) 
{
	_s0 = nullptr; k = 0; ips[0] = '\0';
	_s = (char*)str;
	while (1) {
		if (IsIpAddress(&_s, &_s0, k, ips)) {
			if (nextStr) *nextStr = _s;
			break;
	}	}
	return &ips[0];
}

//_____________________________________________________________________________
//
bool IpAddressTrawler::IsIpAddress(char** _s, char** _s0, int& k, char* ips) 
{
	char* s = *_s;
	if (*s == '\0') {
		ips[0] = '\0';
		return true;
	}
	if ((k >= 0 && k < 3 ) || (k > 3  && k < 7) ||
		(k >  7 && k < 11) || (k > 11 && k < 15)) {
		if ((*s >= '0' && *s <= '9') || (*s == ' ' && k < 13)) {
			// Ok
			if (k == 0) {
				*_s0 = *_s;
				ns = 0;
			}
			if (*s != ' ') ++k;
			else ++ns;
			if (ns > 2) {
				// stop
				k = 0;
				*_s0 = nullptr;
			}
			(*_s)++;
			return false;
		}
		else {
			if (*s == '.') {
				// Stop
				if (k > 12);
				else {
					// Ok.
					ns = 0;
					if (k > 8) { k = 12; (*_s)++; return false; }
					if (k > 4) { k = 8;  (*_s)++; return false; }
					if (k > 1) { k = 4;  (*_s)++; return false; }
	}	}	}	}
	else {
		if (*s == '.' && k < 15) { ns = 0;  ++k; (*_s)++; return false; }
	}
	if (*_s0) {
		if (*_s - *_s0 > 0 && k > 12) {
			// Ok
			int l = *_s - *_s0;
			memcpy(ips, *_s0, l);
			ips[l] = '\0';
		} else {
			// stop
			ips[0] = '\0';
		}
		k = 0;
		*_s0 = nullptr;
		return (ips[0] == '\0' ? false : true);
	}

	(*_s)++;
	return false;
}
//_____________________________________________________________________________
//
