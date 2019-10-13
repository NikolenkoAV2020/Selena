// DecodeCommandString.cc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <DecodeCommandString.h>

static char* StringSkipSpace(char* p)
{
	while (1) {
		if (*p == '\0') return p;
		if (*p != ' ') return p;
		++p;
	}
	return NULL;
}

static int StringFindSymbol(char*p, const char ch, int* j)
{
	*j = 0;
	while (1) {
		if (*p == '\0' || *p == ch) break;
		++p; ++(*j);
	}
	return (*p == '\0' ? -1 : 0);
}

int DecodeCommandString(const char* Command, int *countSubString, char*** SubStrings)
{
	if (!Command || !countSubString) return-3;

	int  i, r = 0;
	int  l = strlen(Command); if (l <= 0) return-1;
	char *S = (char*)malloc(l + 1);  if (!S) return -2;
	memcpy(S, Command, l);
	S[l] = '\0';

	if (*countSubString > 0) {
		if (*SubStrings) free(*SubStrings);
		*SubStrings = (char**)calloc(*countSubString, sizeof(char*));
	}

	int  count = 0;
	char *p = &S[0];

	while (1) {
		if (r < 0) break;
		if (*countSubString && count == *countSubString) break;

		if (*p == ' ') p = StringSkipSpace(p);

		if (*p == '"') {
			++p;
			r = StringFindSymbol(p, '"', &i);
			if (*countSubString > 0) {
				(*SubStrings)[count] = (char*)malloc(i + 1);
				memcpy((*SubStrings)[count], p, i);
				(*SubStrings)[count][i] = '\0';
			}
			p += i;	*p = '\0';
			++count;
			if (!r) ++p;
			else continue;
		}

		if (*p != '"' && *p != ' ') {
			r = StringFindSymbol(p, ' ', &i);
			if (*countSubString > 0) {
				(*SubStrings)[count] = (char*)malloc(i + 1);
				memcpy((*SubStrings)[count], p, i);
				(*SubStrings)[count][i] = '\0';
			}
			p += i;	*p = '\0';
			++count;
			if (!r) ++p;
			else continue;
		}
	}

	r = 0;
	if (!(*countSubString)) {
		if (!count) {
			free(S); S = NULL;
			return -4;
		}
		*countSubString = count;
		r = DecodeCommandString(Command, countSubString, SubStrings);
	}

	free(S); S = NULL;
	return r;
}
int DecodeCommandStringFree(int* countSubString, char*** SubStrings)
{
	if (!(*SubStrings) || !countSubString) return 0;
	int i;
	for (i = 0; i < *countSubString; i++) {
		free((*SubStrings)[i]);
		(*SubStrings)[i] = NULL;
	}
	*countSubString = 0;

	free(*SubStrings);
	*SubStrings = NULL;

	return 0;
}