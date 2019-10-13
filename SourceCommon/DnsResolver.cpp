#include <iostream>
#include <winsock.h>
#include <vector>
#include <string>

using namespace std;

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
	const char*  name, 
	std::string& resultName, 
	std::vector<std::string>* alias);

int GetIpForDomainName(
	const char*  domaneName, 
	std::string& ip,
	std::vector<std::string>* alias)
{
	return GetHost(domaneName, ip, alias);
}

int GetDomainNameForIp(
	const char* ip, 
	std::string& domaneName,
	std::vector<std::string>* alias)
{
	return GetHost(ip, domaneName, alias);
}

//-----------------------------------------------------------------------------
int GetHost(
	const char* query, 
	std::string& resultName,
	std::vector<std::string>* alias)
{              
	hostent *host;                  // the hostent structure    
	in_addr ipHost;                 // if argv[1] is ip address 
	int		i, isIP = 1;            // isIP flag if ip address  
 
	resultName = "";
	if (alias) alias->clear();

	// Your localhost, and the search query ...
	// gethostname(buffer[1], 50);

	// Check if our parameter is hostname or ip address ...
	for (i = 0; i < (int)strlen(query); i++)	{
		if (query[i] == '.') continue;
		else if (isalpha(query[i]))	{
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
		resultName = host->h_name;
		// If there are aliases to this domain, we'll list them!
		if (alias && host->h_aliases[0]) {
			alias->push_back(host->h_aliases[0]);
			for (i = 1; host->h_aliases[i]; i++) {
				alias->push_back(host->h_aliases[i]);
		}	}
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
		alias->push_back(inet_ntoa(*((struct in_addr *)host->h_addr_list[i])));
	}

	return 0;
}

//-----------------------------------------------------------------------------
void ReverseIP(char* pIP, int lIP)
{
	char  seps[] = ".";
	char  pIPSec[4][64];
	char* token;
	char* nexttoken;
	int   i = 0;
	token = strtok_s(pIP, seps, &nexttoken);
	while (token != NULL && i < 4) {
		sprintf_s(pIPSec[i], 64, "%s", token);
		token = strtok_s(NULL, seps, &nexttoken);
		i++;
	}
	sprintf_s(pIP, lIP, "%s.%s.%s.%s.%s", pIPSec[3], pIPSec[2], pIPSec[1], pIPSec[0], "IN-ADDR.ARPA");
}

//-----------------------------------------------------------------------------
/*
#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) 
	#include <Windns.h>
	#pragma message("		Connectin ... Dnsapi.lib")
	#pragma comment (lib, "Dnsapi.lib")
#else
	#include <arpa/nameser.h>
	#include<resolv.h>
#endif
*/
/*
//DNS-servers
char ipDNSserver[20][64] = {
	"208.67.222.222",
	"208.67.220.220",
	"208.67.222.220",
	"208.67.220.222",
	"82.200.69.80",
	"84.200.70.40",
	"209.244.0.3",
	"209.244.0.4",
	"4.2.2.1",
	"4.2.2.2",
	"4.2.2.3",
	"4.2.2.4",
	"8.26.56.26",
	"8.20.247.20",
	"77.88.8.1",
	"77.88.8.2",
	"77.88.8.3",
	"77.88.8.7",
	"77.88.8.8",
	"77.88.8.88" };

DNS_STATUS	Status;
PIP4_ARRAY	SrvList = NULL;
PDNS_RECORD DnsRecord;
IN_ADDR		ipaddr;

SrvList = (PIP4_ARRAY)LocalAlloc(LPTR, sizeof(IP4_ARRAY));
SrvList->AddrCount = 1;
SrvList->AddrArray[0] = inet_addr("208.67.222.222"); // Тут задаем IP - адрес DNS - сервера

//Status = DnsQuery("google.ru", // Тут задаем адрес хоста, который надо преобразовать
char sip[64] = "5.255.255.5";
ReverseIP(sip, 64);
Status = DnsQuery(sip,
	//DNS_TYPE_A,
	DNS_TYPE_PTR,
	DNS_QUERY_BYPASS_CACHE,
	SrvList,
	&DnsRecord,
	NULL);

if (Status) {
	//Form1->Memo1->Lines->Add("Error dns");
}
else {
	ipaddr.S_un.S_addr = (DnsRecord->Data.A.IpAddress);
	std::cout << "\n" << inet_ntoa(ipaddr) << "\n";
	if (DnsRecord->Data.PTR.pNameHost)
		std::cout << DnsRecord->Data.PTR.pNameHost << "\n";
	DnsRecordListFree(DnsRecord, freetype);
}
LocalFree(SrvList);
*/