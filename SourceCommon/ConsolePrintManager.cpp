#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32) || defined(_WINDOWS) 
#include <Windows.h>
#endif

#include <vector>
#include <list>
#include <map>
#include <utility>

//-----------------------------------------------------------------------------
struct ScreenRgn {
	unsigned int rb = 0;
	unsigned int re = 0;
	unsigned int cb = 0;
	unsigned int ce = -1;
};
//-----------------------------------------------------------------------------
class ConsolePrintManager {
public:

private:

public:
	ConsolePrintManager();
	~ConsolePrintManager();

	unsigned int CaptureBlock(const ScreenRgn& r);
	void         FreeBlock(unsigned int ID);
	void         SetCursorPos(int X, int Y);
	int			 GetCursorPos(int* X, int* Y);

private:
	unsigned int RID0 = 0;
	std::map<unsigned int, ScreenRgn> R;

};

//-----------------------------------------------------------------------------
ConsolePrintManager::ConsolePrintManager()
{

}

ConsolePrintManager::~ConsolePrintManager()
{

}

unsigned int ConsolePrintManager::CaptureBlock(const ScreenRgn& r)
{
	unsigned int ID = ++RID0;
	R.insert(std::make_pair(ID, r));
	return ID;
}

void ConsolePrintManager::FreeBlock(unsigned int ID)
{
	auto i = R.find(ID);
	if (i != R.end()) R.erase(i);
}

//    \033[#A ����������� ������ ����� �� # �����
//    \033[#B ����������� ������ ���� �� # �����
//    \033[#� ����������� ������ ������ �� # ��������
//    \033[#D ����������� ������ ����� �� # ��������
//    \033[#E ����������� ������ ���� �� # ����� � ��������� � ������ ������
//    \033[#F ����������� ������ ����� �� # ����� � ��������� � ������ ������
//    \033[#G ����������� ������ � ��������� ������� ������� ������
//    \033[#;#H ������ ���������� ���������� ������� (������, �������)
//    \033]2;BLA_BLA\007   ��������� ���� xterm...

void ConsolePrintManager::SetCursorPos(int X, int Y)
{
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
	COORD Coord;
	Coord.X = X;
	Coord.Y = Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);
#else
	printf("\033[%d;%dH", Y + 1, X + 1);
#endif
}

int ConsolePrintManager::GetCursorPos(int* X, int* Y)
{
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;
	if (Y) *Y = csbi.dwCursorPosition.Y;
	if (X) *X = csbi.dwCursorPosition.X;
	return 0;
#else
	return -1;
#endif
}
