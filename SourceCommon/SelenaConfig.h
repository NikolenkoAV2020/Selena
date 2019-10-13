// SelenaConfig.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_CONFIG_HEADER
#define SELENA_CONFIG_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// Загрузка конфигурационного файла
int ReadConfig(char var,
	const std::string&  fileName,
	std::string&  hostName,
	std::string&  ip,
	unsigned int& portNumber);

#endif // #ifndef SELENA_CONFIG_HEADER
