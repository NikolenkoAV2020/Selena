// DecodeCommandStrings.h

#ifndef DECODE_COMMAND_STRING_H
#define DECODE_COMMAND_STRING_H

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

	int DecodeCommandString(const char* Command, int *countSubString, char*** SubStrings);
	int DecodeCommandStringFree(int* countSubString, char*** SubStrings);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef DECODE_COMMAND_STRING_H
