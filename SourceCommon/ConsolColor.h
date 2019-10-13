// ConsolColor.h
// Walter Artyomenko, August 2019
//
// Change color and text attributes in the console
#ifndef CONSOL_COLOR_DEFINE
#define CONSOL_COLOR_DEFINE

//-----------------------------------------------------------------------------
// Изменение атрибутов и цветов при помощи эскейп-последовательности символов
//	атрибуты
//		0	нормальный режим
//		1	жирный
//		4	подчеркнутый
//		5	мигающий
//		7	инвертированные цвета
//		8	невидимый
//	цвет текста
//		30	черный
//		31	красный
//		32	зеленый
//		33	желтый
//		34	синий
//		35	пурпурный
//		36	голубой
//		37	белый
//	цвет фона
//		40	черный
//		41	красный
//		42	зеленый
//		43	желтый
//		44	синий
//		45	пурпурный
//		46	голубой
//		47	белый
//-----------------------------------------------------------------------------

#define SELENA_CCLR_SERV_DEF "\x1b[0m"			// Возврат к параметрам по умолчанию
#define SELENA_CCLR_SERV_MSG "\x1b[33;40;1m"	// Сообщения серверу от клиента
#define SELENA_CCLR_SERV_COM "\x1b[32;40m"		// Команды сервера
#define SELENA_CCLR_SERV_ERR "\x1b[31;40;1m"	// Ошибки
#define SELENA_CCLR_SERV_WAR "\x1b[30;40;1m"	// Предупреждения

// Эскейп-последовательность с заданными параметрами 
// Пример colors: "33;40;1"
char* ConsolEscColors(char* colors);
void  SetConsolColors(int clText, int clBk);

#endif // #ifndef CONSOL_COLOR_DEFINE