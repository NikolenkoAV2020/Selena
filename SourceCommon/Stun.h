// Stun.h
// Walter Artyomenko, August 2019
//
#ifndef STUN_HEADER
#define STUN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

#ifdef WIN32
	#include <winsock2.h>
	#include <stdint.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#include <StunMini.h>

// Input: random value to generate the pair (pos, step) for pseudorandom 
// traversal over the server list
// Output: 
//  - mapped: populate struct struct mapped (ipV4 only)
//  - srv: set pointer to server name, which return successful answer
// Retval:
// bits 0-7 = STUN tokens set, 8-32 = attempt number
// Negative return - unable to figure out IP address
int GetExternalIPbySTUN(uint64_t rnd, struct sockaddr_in *mapped, const char **srv, uint16_t src_port);

#endif // #ifndef STUN_HEADER