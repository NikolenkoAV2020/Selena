// SelenaConfig.cpp 
// Walter Artyomenko, August 2019
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <SelenaConfig.h>

//-----------------------------------------------------------------------------
int StrToKey(std::string& s)
{
	int key = 0;
	if (s == "host") key = 1;
	else if (s == "ip") key = 2;
	else if (s == "port") key = 3;
	return key;
}

//-----------------------------------------------------------------------------
int ReadConfig(char var, 
	const std::string& fileName,
	std::string&  hostName,
	std::string&  ip,
	unsigned int& portNumber)
{
	std::string s;

	portNumber = 0;
	hostName = "";
	ip = "";

	std::ifstream cfgFile;
	cfgFile.open(fileName, std::ofstream::in);
	if (cfgFile.is_open()) {
		std::string sk;
		int k0 = 0, k1 = 0;
		while (1) {
			if (!k1) {
				if (cfgFile.eof()) break;
				cfgFile >> sk;
				k0 = StrToKey(sk);
			}
			else k0 = k1;

			if (cfgFile.eof()) break;
			cfgFile >> s;
			k1 = StrToKey(s);

			if (k0 && !k1) {
				switch (k0) {
					case 1: hostName = s; break;
					case 2: ip = s; break;
					case 3: portNumber = atoi(s.c_str()); break;
		}	}	}
		cfgFile.close();
	} else {
		std::cout
			<< "Warning: configuration file not found" << std::endl;
	}

recon:
	std::cout << std::flush;
	if ((hostName.length() || ip.length()) && portNumber) {
		std::cout
			<< "To connect to the "
			<< (!var ? "client" : "server")
			<< ", use: " << std::endl;
		if (hostName.length()) {
			std::cout << "\tHost name:   " << hostName << std::endl;
		}
		if (ip.length() && !hostName.length()) {
			std::cout << "\tIP adress:   " << ip << std::endl;
		}
		if (portNumber) {
			std::cout << "\tPort number: " << portNumber << std::endl;
		}	
	}
	else {
		std::cout << std::endl;
		while (1) {
			if (!hostName.length()) {
				std::cout
					<< "Input host name for connect whith "
					<< (!var ? "SelenaClient..." : "SelenaServer...")
					<< std::endl;
				std::cout
					<< (var ? "SelenaClient >" : "SelenaServer >");
				std::cin
					>> hostName;
				if (hostName == "exit") return-1;
			}
			if (!hostName.length() && !ip.length()) {
				std::cout
					<< "Input IP adress for connect whith "
					<< (!var ? "SelenaClient..." : "SelenaServer...")
					<< std::endl;
				std::cout
					<< (var ? "SelenaClient >" : "SelenaServer >");
				std::cin >> ip;
				if (ip == "exit") return-1;
			}
			if (!portNumber) {
				std::cout
					<< "Input port for connect whith "
					<< (!var ? "SelenaClient..." : "SelenaServer...")
					<< std::endl;
				std::cout
					<< (var ? "SelenaClient >" : "SelenaServer >");
				std::cin >> s;
				if (s == "exit") return-1;
				portNumber = atoi(s.c_str());
			}
			if ((hostName.length() || ip.length()) && portNumber > 0) break;
		}
		goto recon;
	}
	return 0;
}

//-----------------------------------------------------------------------------