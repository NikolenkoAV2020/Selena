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
// Проврека команд
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
// Замена в строке одних заданных подстрок s1 на другие s2.
// Результат сохраняется в ссылке на исходную строку source.
int ReplaceStdSubstr(
	// Исходная строка
	std::string& source,
	// что меняем 
	const std::string s1,
	// и на что меняем
	const std::string s2);

//-----------------------------------------------------------------------------
// Значения опции в строке-команде
int CommandValueOptions(
	// Исходная команда
	const std::string command,
	// Имя опции значение которой необходимо получить
	const std::string opt,
	// Значение опции
	std::string& value);

//-----------------------------------------------------------------------------
// Замена значения опции в строке-команде
int CommandReplaceOptions(
	// Исходная команда
	std::string& command,
	// Имя опции значение которой необходимо заменить
	const std::string opt,
	// Новое значение опции
	const std::string newval);

//-----------------------------------------------------------------------------
// Поиск ключевого слова word в строке command
int CommandFindWord(
	// Исходная команда
	const std::string command,
	// Имя опции значение которой необходимо получить
	const std::string word);
//-----------------------------------------------------------------------------
// Первое слово word в строке command
int CommandGetFirstWord(
	// Исходная команда
	const std::string command,
	// найденное первое слово в строке
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
