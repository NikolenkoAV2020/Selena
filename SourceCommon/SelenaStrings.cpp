// SelenaStrings.cpp 
// Walter Artyomenko, August 2019
//
#include <SelenaStrings.h>
#include <NetWinUnixCompat.h>

//-----------------------------------------------------------------------------
bool IsOnlySpacesFirst(const std::string& command, stdsize p)
{
	if (p == std::string::npos) return false;
	int i = (int)p;
	while (--i >= 0) { if (command[i] != ' ') break; }
	return (i < 0 ? true : false);
}

//-----------------------------------------------------------------------------
// Проврека команд
bool IsExitCommand(const std::string& command)
{
	if (command == "exit" || command == "close" ||
		command == "break" || command == "job over") {
		// Если была команда на завершение, 
		return true;
	}
	return false;
}

bool IsRestartCommand(const std::string& command)
{
	return (command == "restart" ? true : false);
}

bool IsInterruptCommand(const std::string& command)
{
	return (command == "disconnect" ? true : false);
}

bool IsConnectCommand(const std::string& command)
{
	return (command == "connect" ? true : false);
}

bool IsJobListCommand(const std::string& command)
{
	return IsOnlySpacesFirst(command, command.find("job list", 0));
}

bool IsJobCountCommand(const std::string& command)
{
	return IsOnlySpacesFirst(command, command.find("job count", 0));
}

bool IsJobRemoveCommand(const std::string& command)
{
	return IsOnlySpacesFirst(command, command.find("job remove", 0));
}

bool IsMasscanCommand(const std::string& command)
{
	return IsOnlySpacesFirst(command, command.find("masscan", 0));
}

bool IsSendCommand(const std::string& command)
{
	return IsOnlySpacesFirst(command, command.find("send", 0));
}

//-----------------------------------------------------------------------------
// Замена в строке одних заданных подстрок s1 на другие s2.
int ReplaceStdSubstr(
	// Исходная строка
	std::string& source,
	// что меняем 
	const std::string s1,
	// и на что меняем
	const std::string s2)
{
	std::string newstring = "";
	newstring.reserve(source.length() * 2);
	auto l = s1.length();
	auto i = l;
	auto j = l;
	i = 0; j = 0;
	while (1) {
		j = source.find(s1, i);
		if (j == std::string::npos) {
			newstring += source.substr(i, source.length() - i);
			source = newstring;
			break;
		}
		newstring += source.substr(i, j - i);
		newstring += s2;
		i = j + l;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Поиск ключевого слова word в строке command
int CommandFindWord(
	// Исходная команда
	const std::string command,
	// Имя опции значение которой необходимо получить
	const std::string word)
{
	stdsize j = command.find(word, 0);
	return ((j == std::string::npos) ? -1 : (int)j);
}

//-----------------------------------------------------------------------------
// Первое слово word в строке command
int CommandGetFirstWord(
	// Исходная команда
	const std::string command,
	// найденное первое слово в строке
	std::string& word)
{
	word = "";
	int  n = command.length();
	bool e = false;
	for (int i = 0; i < n; i++) {
		if (command[i] == ' ') {
			if (e) break;
			continue;
		}
		word += command[i];
		e = true;
	}
	return word.length();
}

//-----------------------------------------------------------------------------
// Значения опции в строке-команде
int CommandValueOptions(
	// Исходная команда
	const std::string command,
	// Имя опции значение которой необходимо получить
	const std::string opt,
	// Значение опции
	std::string& value)
{
	value = "";

	auto l = command.length();
	auto k = l;
	auto j = l; j = 0;
	j = command.find(opt, j);
	if (j == std::string::npos) {
		// Такой опции в команде нет
		return-1;
	}

	j += opt.length(); if (j >= l) return 0;

	// Пропускаем пробелы
	while (command[j] == ' ' && command[j] != '\0') j++;
	if (j >= l) return-1;

	// Выделяем подстроку - значение опции

	// Указано ли значение опции в кавычках
	bool isQuotes = (command[j] == '"');
	if (isQuotes) j++;
	if (j >= l) return-1;
	k = j;

	// Перебираем символы значения опции
	while (command[j] != (isQuotes ? '"' : ' ') &&
		   command[j] != '\0' && command[j] != ';') {
		j++;
	}
	if (j > l) return-1;

	// Получаем значение
	value = command.substr(k, j - k);
	return (value.length() ? 0 : -1);
}

//-----------------------------------------------------------------------------
// Замена значения опции в строке-команде
int CommandReplaceOptions(
	// Исходная команда
	std::string& command,
	// Имя опции значение которой необходимо заменить
	const std::string opt,
	// Новое значение опции
	const std::string newval) 
{
	auto l = command.length();
	auto k = l;
	auto i = l; i = 0;
	auto j = command.find(opt, i);
	if (j == std::string::npos) {
		// Такой опции в команде нет
		return 0;
	}

	j += opt.length(); if (j >= l) return 0;

	// Пропускаем пробелы
	while (command[j] == ' ') j++;

	// Выделяем подстроку - значение опции

	// Указано ли значение опции в кавычках
	bool isQuotes = (command[j] == '"');
	k = j;
	// Перебираем символы значения опции
	while (command[j] != (isQuotes ? '"' : ' ')) j++;
	if (isQuotes) j++;

	// Меняем подстроку
	std::string newstring;
	newstring.reserve(command.length() + newval.length());
	newstring  = command.substr(0, k);
	newstring += "\""; newstring += newval;	newstring += "\"";
	newstring += command.substr(j, command.length() - j);
	command = newstring;

	return 0;
}

//-----------------------------------------------------------------------------
// Extracting output file name from the command string
int CommandFindOutFileName(
	// string of command
	const std::string command,
	// sought-for file name
	std::string& fileName)
{
	fileName = "";
	const int   keyCount = 6;
	const char* keyList[keyCount] = { 
		"-oL", "-oJ", "-oG", "-oB", "-oX",
		"--output-file"};

	// Sorry, but I just try sort out and read all possible options
	for (int i = 0; i < keyCount; ++i) {
		if (!CommandValueOptions(command, keyList[i], fileName))
			break;
	}

	return (fileName.length()>0 ? 0 : -1);
}

//-----------------------------------------------------------------------------
// Extracting input file name from the command string
int CommandFindInFileName(
	// string of command
	const std::string command,
	// sought-for file name
	std::string& fileName)
{
	fileName = "";
	const int   keyCount = 1;
	const char* keyList[keyCount] = {"-iL"};

	// Sorry, but I just try sort out and read all possible options
	for (int i = 0; i < keyCount; ++i) {
		if (!CommandValueOptions(command, keyList[i], fileName))
			break;
	}

	return (fileName.length()>0 ? 0 : -1);
}

//-----------------------------------------------------------------------------
// Extracting ip-address from given file

#include <IpAddressTrawler.h>

int ExtractIPfromFile(
	// source file
	const std::string sourcef,
	// result file
	const std::string resultf)
{
	// open source file
	std::ifstream ifile;
	ifile.open(sourcef, std::ios::in);
	if (!ifile.is_open()) return 1;
	// open result file
	std::ofstream ofile;
	ofile.open(resultf, std::ios::out | std::ios::trunc);
	if (!ofile.is_open()) return 2;

	IpAddressTrawler T;
	std::string      s;

	while (getline(ifile, s)) {    
		// Find ip-address in string
		char* ips = T((char*)s.c_str(), nullptr);
		if (ips[0] == '\0') continue;



		// write found address to output file
		ofile << ips << std::endl;
	}

	// Close files
	ofile.close();
	ifile.close();	

	return 0;
}

//-----------------------------------------------------------------------------
