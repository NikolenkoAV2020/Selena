// SelenaStrings.h
// Walter Artyomenko, August 2019
//
#ifndef REPLACE_STD_SUBSTR
#define REPLACE_STD_SUBSTR
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

//-----------------------------------------------------------------------------
// �������� ������
bool IsExitCommand     (const std::string& command);
bool IsRestartCommand  (const std::string& command);
bool IsInterruptCommand(const std::string& command);
bool IsConnectCommand  (const std::string& command);
bool IsMasscanCommand  (const std::string& command);
bool IsJobListCommand  (const std::string& command);
bool IsJobCountCommand (const std::string& command);
bool IsJobRemoveCommand(const std::string& command);
bool IsSendCommand     (const std::string& command);

//-----------------------------------------------------------------------------
// ������ � ������ ����� �������� �������� s1 �� ������ s2.
// ��������� ����������� � ������ �� �������� ������ source.
int ReplaceStdSubstr(
	// �������� ������
	std::string& source,
	// ��� ������ 
	const std::string s1,
	// � �� ��� ������
	const std::string s2);

//-----------------------------------------------------------------------------
// �������� ����� � ������-�������
int CommandValueOptions(
	// �������� �������
	const std::string command,
	// ��� ����� �������� ������� ���������� ��������
	const std::string opt,
	// �������� �����
	std::string& value);

//-----------------------------------------------------------------------------
// ������ �������� ����� � ������-�������
int CommandReplaceOptions(
	// �������� �������
	std::string& command,
	// ��� ����� �������� ������� ���������� ��������
	const std::string opt,
	// ����� �������� �����
	const std::string newval);

//-----------------------------------------------------------------------------
// ����� ��������� ����� word � ������ command
int CommandFindWord(
	// �������� �������
	const std::string command,
	// ��� ����� �������� ������� ���������� ��������
	const std::string word);
//-----------------------------------------------------------------------------
// ������ ����� word � ������ command
int CommandGetFirstWord(
	// �������� �������
	const std::string command,
	// ��������� ������ ����� � ������
	std::string& word);
//-----------------------------------------------------------------------------
// Extracting output file name from the command string
int CommandFindOutFileName(
	// string of command
	const std::string command,
	// sought-for file name
	std::string& fileName);

//-----------------------------------------------------------------------------
// Extracting input file name from the command string
int CommandFindInFileName(
	// string of command
	const std::string command,
	// sought-for file name
	std::string& fileName);

//-----------------------------------------------------------------------------
// Extracting ip-address from given file
int ExtractIPfromFile(
	// source file
	const std::string sourcef,
	// result file
	const std::string resultf);

#endif // #ifndef REPLACE_STD_SUBSTR
