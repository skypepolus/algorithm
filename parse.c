#include <ctype.h>
#include <stdlib.h>
#include "parse.h"

ssize_t parse_int(char* line, ssize_t len, const char* delimiter, int* integer)
{
	ssize_t i = 0;
	char* begin, * end;
	for(begin = line; begin < line + len; begin = end + 1)
	{
		ssize_t l;
		while((begin < (line + len)) && isspace(*begin))
			begin++;
		l = strtol(begin, &end, 10);
		if(integer)
			integer[i] = l;
		if(begin < end)
		{
			i++;
			if(*delimiter == *end)
				end++;
			else
				break;
		}
		else
			break;
	}
	return i;
}
