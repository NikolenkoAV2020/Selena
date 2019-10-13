// GetIPBeadRoll.cpp 
// Walter Artyomenko, August 2019
//
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <NetWinUnixCompat.h>
#include <GetIPBeadRoll.h>

int GetIPBeadRoll(IpBeadRoll& ipR)
{
	struct sockaddr_in mapped;
	const char *srv;
	int r = 0;

	uint16_t src_port = AURORA_PORT;
	r = GetExternalIPbySTUN(13, &mapped, &srv, src_port);
	if (r) return r;

	char S[256] = "\0";
	inet_ntop(AF_INET, &mapped.sin_addr, S, 256);
	if (strlen(S)) {
		ipR.whitetIp    = new std::string[1];
		ipR.whitetIp[0] = S;
		std::cout << "white IP: " << ipR.whitetIp[0].c_str() << std::endl;
	}

	GetGreyIPList(ipR);

	std::cout << std::flush;

	return 0;
}

//------------------------------------------------------------------------------
int GetGreyIPList(IpBeadRoll& IpR)
{
	char chInfo[256];

	if (IpR.greyIp) delete[] IpR.greyIp;
	IpR.localName = "";
	IpR.countGrey = 0;

	if (!gethostname(chInfo, sizeof(chInfo))) {
		hostent *sh0;
		sh0 = gethostbyname((const char*)chInfo);
		if (sh0) {
			short N = 0;
			IpR.localName = sh0->h_name;
			for (int i = 0; i < 2; ++i) {
				hostent *sh = sh0;
				IpR.countGrey = 0;
				while (sh->h_addr_list[IpR.countGrey]) {
					if (i && IpR.countGrey == N) break;
					struct sockaddr_in adr;
					char ipaddr[64];
					memcpy(&adr.sin_addr, sh->h_addr_list[IpR.countGrey], sh->h_length);
					sprintf_s(ipaddr, 64, "%s", inet_ntop(AF_INET, &adr.sin_addr, ipaddr, 64));		
					if (i) {
						IpR.greyIp[IpR.countGrey] = ipaddr;
						printf("IP is: %s\n", ipaddr);
					}
					IpR.countGrey++;
				}
				if (!i) {
					N = IpR.countGrey;
					IpR.greyIp = IpR.countGrey ?
						new std::string[N] :
						nullptr;
	}	}	}	}
	return 0;
}

//------------------------------------------------------------------------------
int IpFromHostName(const char* HOST_NAME, HostIp& H, bool isEcho) {
	char buf[BUFSIZ], **pp;
	struct hostent* hp;

	H.hostName = HOST_NAME;

	if (!(hp = gethostbyname(HOST_NAME))) {
		fprintf(stderr, "Error!\n");
		exit(1);
	}

	if (isEcho) {
		std::cout
			<< "__________________________________________________\n"
			<< "\tOfficial name:  "
			<< hp->h_name << std::endl
			<< "\tAliases:         ";
		int i = 0;
		for (pp = hp->h_aliases; *pp; ++pp) {
			if (i) std::cout << "\t                 ";
			std::cout << *pp << std::endl;
			++i;
		}
		if (!i) std::cout << std::endl;
	}

	if (hp->h_addrtype != AF_INET) {
		fprintf(stderr, "Unknown address type!\n");
		exit(1);
	}

	if (H.ipAddr != nullptr) {
		delete[] H.ipAddr;
		H.ipAddr = nullptr;
	}
	H.countAddr = 0;
	for (pp = hp->h_addr_list; *pp; ++pp)
		H.countAddr++;

	if (H.countAddr) {
		H.ipAddr = new std::string[H.countAddr];
		if (isEcho) {
			std::cout << "\tIP address(es): ";
		}
		int i = 0;
		for (pp = hp->h_addr_list; *pp; ++pp) {
			H.ipAddr[i] = inet_ntop(hp->h_addrtype, *pp, buf, sizeof(buf));
			if (isEcho) {
				if (i) std::cout << "\t                ";
				std::cout 
					<< H.ipAddr[i].c_str()
					<< std::endl;
			}
			++i;
		}
		if (!i) std::cout << std::endl;
	}

	if (isEcho) {
		std::cout << "__________________________________________________\n\n";
	}

	std::cout << std::flush;

	return 0;
}

//------------------------------------------------------------------------------
int doit()
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		std::cerr << "Error " << GetSocketError() <<
			" when getting local host name." << std::endl;
		return 1;
	}
	std::cout << "Host name is " << ac << "." << std::endl;

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		std::cerr << "Yow! Bad host lookup." << std::endl;
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		char S[256];
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << "Address " << i << ": " << inet_ntop(AF_INET, &addr, S, 256) << std::endl;
	}

	return 0;
}

//------------------------------------------------------------------------------
#if defined(WIN32) || defined(WINDOWS)
// for Windows only
#include <iostream>
#include <Snmp.h>
#include <Mgmtapi.h>
#include <Winuser.h>
#include <Winbase.h>
#include <stdio.h>
#include <tchar.h>

// For Windows only
#pragma message("		Connectin ... Snmpapi.lib")
#pragma comment(lib, "Snmpapi.lib")
#pragma message("		Connectin ... Mgmtapi.lib")
#pragma comment(lib, "Mgmtapi.lib")

// ����� ��������� WAN ip ����� �������, ��������������� ��� � ������ � ���� ���.���.���.���
// � ���������� ��� � ����!
int GlobalIP()
{
	AsnInteger errorStatus = 0; //��� ���������� ������. 
	AsnInteger errorIndex = 0;  //�����-�� ������ ������ (���� ���� �������)

	AsnObjectIdentifier OID;  //������������� ������� (����������, � ������� ����� ������� ip)
	SnmpVarBindList snmpVarList; //�������� ������������ ������� 
	snmpVarList.len = 1;         //��� �� ������ ���� ���������� ����������, ���� :)
								 //��������� �� ��� ���� ���������, ���������� ��� ���� ��� ��� ����� ���������� =)
								 //������ ��� ��� �������� �������� SnmpUtilMemAlloc. ����� ���������� ���������� ������ �������� SnmpUtilMemFree()
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(sizeof(SnmpVarBind)*snmpVarList.len);

	//��������������� ��������� ������������� ���������, � ������� ��������� IP ����� �������, � ���������� ������������� ���� ����������-OID
	SnmpMgrStrToOid((char*)".1.3.6.1.2.1.4.20.1.1", &OID);
	SnmpUtilOidCpy(&snmpVarList.list[0].name, &OID);

	// �������� ������ � SNMP �������: (���������� LAN ����� �������(SNMP������) - 10.0.0.1 (����� - 10.0.0.2) 
	// ������ �� ���������� ����������, �� ��������� - "public")
	// 1000 - ��� ������� ����� ��������� �������� ����������� � ������� (� ������������)
	// 1 - ���-�� ������� ���������� �������������� (���� �� �����������)
	PVOID snmpH = SnmpMgrOpen((char*)"10.0.0.1", (char*)"public", 1000, 1);
	if (!snmpH)
	{
		MessageBox(0, _T("�� ������� ������ SNMP ������"), _T("�������� �����"), MB_OK | MB_ICONERROR);
	}
	short i;

	// � ����� ������ ������ 3 ����. 
	// � ������ ��� �������� LAN ����� ������� - (� ���� ������ 10.0.0.1) - �� ���������
	// �� ������ ��� �������� ����� ���������� ����� - 127.0.0.1          - �� ���������
	// �, � ����, � ������ ��� �������� WAN ����� ������� - (� ������� �� ��������� ����������) - �� ��� ����!!!!
	for (i = 0; i < 3; i++)
	{
		if (!SnmpMgrRequest(snmpH, SNMP_PDU_GETNEXT, &snmpVarList, &errorStatus, &errorIndex))
		{
			MessageBox(0, _T("�� ������� ��������� ����������"), _T("�� ��� � �����!?"), MB_OK | MB_ICONERROR);
			SnmpUtilVarBindListFree(&snmpVarList);
			SnmpUtilOidFree(&OID);
			return 1;
		}
	}

	// ��������� �� ���������� IP ������
	// �������� �� ��������� � ��������������� ����������, ������ ���� - ���� �� 4-� ���������� ���� ������
	// �������� ���� IP - 101.11.238.54 �� � ������ �� ��������: 650BEE36
	// ������� ���������� ������ ��������. ������? ��� ��� ��������� ������.
	int *ipPointer = (int *)snmpVarList.list[0].value.asnValue.address.stream;
	char buffer[256] = "";
	char buf[256] = "";
	// ����� � ����� � ������ �������������� IP � ��������� ���
	// ���������� ����� �����
	for (i = 0; i < 4; i++)
	{
		int ipAddress = *ipPointer;
		ipAddress >>= i * 8;
		ipAddress &= 255;
		sprintf_s(buf, 256, "%u", ipAddress);
		strcat_s(buffer, buf);
		if (i < 3) {
			strcat_s(buffer, ".");
		}
	}
	// ����������� ������, ���������� �������� SnmpUtilMemAlloc:
	SnmpUtilMemFree(snmpVarList.list);
	// � buffer - ���������� ������

	// ��������� ���� "ip.txt" � ������ ���������� ���� IP-����
	FILE *fileH = nullptr;
	fopen_s(&fileH, "ip.txt", "w");
	if (fileH) {
		fputs(buffer, fileH);
		fclose(fileH);
	}

	return 0; // �����!
}

#endif	// #ifdef WINDOWS