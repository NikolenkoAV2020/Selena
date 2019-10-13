// ConsolColor.cpp
// Walter Artyomenko, August 2019
//
// Change color and text attributes in the console
#include <NetWinUnixCompat.h>

// Ёскейп-последовательность с заданными параметрами 
// ѕример colors: "33;40;1"
char* ConsolEscColors(char* colors)
{
	static char e[32];
	sprintf_s(e, 32, "\x1b[%sm", colors);
	return e;
}

void SetConsolColors(int clText, int clBk)
{
#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)
	HANDLE hwnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hwnd, (WORD)((clText << 4) | clBk));
#else

#endif // #if defined(WIN32) ...
}
