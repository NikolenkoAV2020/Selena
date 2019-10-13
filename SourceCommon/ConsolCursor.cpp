// ConsolCursor.cpp
// Walter Artyomenko, August 2019
//

#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)
	#include <windows.h>
#else

#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

void ShowConsolCursor(bool isVisible)
{
#if defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle, &structCursorInfo);
	structCursorInfo.bVisible = isVisible;
	SetConsoleCursorInfo(handle, &structCursorInfo);
#else


#endif // #if defined(WIN32) ...
}