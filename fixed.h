#ifndef __fixed_h__
#define __fiexd_h__ __fixed_h__

#include <stdlib.h>
#include "list.h"

struct fixed
{
	size_t size;
	size_t pagesize;
	int page_count;
	struct doubly_linked page_list;
};

void* falloc(struct fixed* fixed);
void ffree(void* ptr);

#endif/*__fixed_h__*/
