#ifndef MAIN_GLOBALS_H
#define MAIN_GLOBALS_H
#include <time.h>

extern unsigned volatile is_tx_done;
extern unsigned volatile is_rx_done;
extern char lastStatus[256];
extern time_t global_now;


#endif
