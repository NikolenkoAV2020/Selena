// SelenaThreads.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_THREADS_HEADER
#define SELENA_THREADS_HEADER

#if defined(WIN32) || defined(WINDOWS)
	#include <Pthread\pthread.h>
#else 
	#include <pthread.h>
#endif

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS_THREAD        0

#endif // #ifndef SELENA_THREADS_HEADER