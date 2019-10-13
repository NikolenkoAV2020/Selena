// ConsolColor.h
// Walter Artyomenko, August 2019
//
// Change color and text attributes in the console
#ifndef CONSOL_COLOR_DEFINE
#define CONSOL_COLOR_DEFINE

//-----------------------------------------------------------------------------
// ��������� ��������� � ������ ��� ������ ������-������������������ ��������
//	��������
//		0	���������� �����
//		1	������
//		4	������������
//		5	��������
//		7	��������������� �����
//		8	���������
//	���� ������
//		30	������
//		31	�������
//		32	�������
//		33	������
//		34	�����
//		35	���������
//		36	�������
//		37	�����
//	���� ����
//		40	������
//		41	�������
//		42	�������
//		43	������
//		44	�����
//		45	���������
//		46	�������
//		47	�����
//-----------------------------------------------------------------------------

#define SELENA_CCLR_SERV_DEF "\x1b[0m"			// ������� � ���������� �� ���������
#define SELENA_CCLR_SERV_MSG "\x1b[33;40;1m"	// ��������� ������� �� �������
#define SELENA_CCLR_SERV_COM "\x1b[32;40m"		// ������� �������
#define SELENA_CCLR_SERV_ERR "\x1b[31;40;1m"	// ������
#define SELENA_CCLR_SERV_WAR "\x1b[30;40;1m"	// ��������������

// ������-������������������ � ��������� ����������� 
// ������ colors: "33;40;1"
char* ConsolEscColors(char* colors);
void  SetConsolColors(int clText, int clBk);

#endif // #ifndef CONSOL_COLOR_DEFINE