// GetIPBeadRoll.h
// Walter Artyomenko, August 2019
//
#pragma once

#ifndef GET_IP_BEAD_ROLL_H
#define GET_IP_BEAD_ROLL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <Stun.h>

struct IpBeadRoll {
public:
	std::string    localName = "";
	std::string    globalName = "";
	std::string*   whitetIp = nullptr;
	std::string*   greyIp = nullptr;
	unsigned short countWhite = 0;
	unsigned short countGrey = 0;

	IpBeadRoll() {
	}
	~IpBeadRoll() {
		if (greyIp) delete[] greyIp;
		if (whitetIp) delete [] whitetIp;
	}
};

struct HostIp {
public:
	std::string    hostName = "";
	std::string*   ipAddr = nullptr;
	unsigned short countAddr = 0;

	HostIp() {
	}
	~HostIp() {
		if (ipAddr) delete[] ipAddr;
	}
};

int GetGreyIPList(IpBeadRoll& IpR);
int GetIPBeadRoll(IpBeadRoll& ipR);

int IpFromHostName(const char* HOST_NAME, HostIp& H, bool isEcho=true);

int doit();
int GlobalIP();

#endif // GET_IP_BEAD_ROLL_H

