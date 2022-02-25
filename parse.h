#ifndef __parse_h__
#define __parse_h__ __parse_h__

#include <stdio.h>

ssize_t parse_int(char* input, ssize_t len, const char* delimiter, int* parsed);

#endif/*__parse_h__*/
