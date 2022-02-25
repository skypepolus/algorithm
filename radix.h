#ifndef __radix_h__
#define __radix_h__ __radix_h__

#include <stdint.h>
#include <stdlib.h>

struct radix
{
	uint8_t* key;
	size_t key_len;
	void* data;
	void** trie;
};

void radix_insert(struct radix** radix, uint8_t* key, size_t key_len, void* data);
void* radix_search(struct radix* radix, uint8_t* key, size_t key_len);
void radix_free(struct radix* radix, void (*free)(void*));

#endif/*__radix_h__*/
