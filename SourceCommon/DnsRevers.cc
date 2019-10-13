#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")

//-----------------------------------------------------------------------------
char af_types[][24] = { "AF_UNSPEC", "AF_UNIX", "AF_INET",
  "AF_IMPLINK", "AF_PUP", "AF_CHAOS", "AF_IPX", "AF_NS",
  "AF_ISO | AF_OSI", "AF_ECMA", "AF_DATAKIT", "AF_CCITT", "AF_SNA",
  "AF_DECnet", "AF_DLI", "AF_LAT", "AF_HYLINK", "AF_APPLETALK",
  "AF_NETBIOS", "AF_VOICEVIEW", "AF_FIREFOX", "AF_UNKNOWN1",
  "AF_BAN", "AF_ATM", "AF_INET6"
};

//-----------------------------------------------------------------------------
int GetHost(
	const char* name,
	char*       resultName,
	int         nameLen);

int GetIpForDomainName(
	const char* domaneName,
	char*       ip,
	int         lenIp)
{
	return GetHost(domaneName, ip, lenIp);
}

int GetDomainNameForIp(
	const char* ip,
	char*       domaneName,
	int         nameLen)
{
	return GetHost(ip, domaneName, nameLen);
}

//-----------------------------------------------------------------------------
int GetHost(
	const char* query,
	char*		resultName,
	int         nameLen)
{
	struct hostent* host;	// the hostent structure    
	struct in_addr  ipHost;	// if argv[1] is ip address 
	int    i, isIP = 1;		// isIP flag if ip address  

	resultName = "";

	// Your localhost, and the search query ...
	// gethostname(buffer[1], 50);

	// Check if our parameter is hostname or ip address ...
	for (i = 0; i < (int)strlen(query); i++) {
		if (query[i] == '.') continue;
		else if (isalpha(query[i])) {
			isIP = 0;
			break;
	}	}

	// The gethostbyname() and gethostbyaddr() functions ...
	if (!isIP) {
		host = gethostbyname(query);
	} else {
		ipHost.s_addr = inet_addr(query);
		host = gethostbyaddr((const char*)&ipHost, sizeof(struct in_addr), AF_INET);
	}

	if (!host) return 0;

	// Display the hostname ...
	if (isIP) {
		if (!host->h_name) return -1;
		sprintf_s(resultName, nameLen, "%s", host->h_name);
		// If there are aliases to this domain, we'll list them!
		//if (alias && host->h_aliases[0]) {
		//	alias->push_back(host->h_aliases[0]);
		//	for (i = 1; host->h_aliases[i]; i++) {
		//		alias->push_back(host->h_aliases[i]);
		//	}
		//}
		return 0;
	}

	// The Address Info (usually AF_INET and 4 bytes address length)
	//sprintf_s(buffer[0], 50, af_types[host->h_addrtype]);
	//cout << "Address Info ............ "
	//	<< buffer[0]
	//	<< ", ";
	//sprintf_s(buffer[0], 50, "%d", host->h_length);
	//cout << buffer[0]
	//	<< " Bytes"
	//	<< endl;

	resultName = inet_ntoa(*((struct in_addr *)host->h_addr));

	// Let's print the IP Address (Addresses if more than one)
	for (i = 1; host->h_addr_list[i]; i++) {
		//alias->push_back(inet_ntoa(*((struct in_addr *)host->h_addr_list[i])));
	}

	return 0;
}

//-----------------------------------------------------------------------------
