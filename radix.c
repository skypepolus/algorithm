/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include "radix.h"
#include "fixed.h"

union size { struct radix radix; void* trie[4]; };
static struct fixed f = { .size = sizeof(union size) };
#define radix_alloc() ((struct radix*)falloc(&f))

static void** trie_alloc(void)
{
	void** trie = (void**)falloc(&f);
	int i;
	for(i = 0; i < 4; i++)
		trie[i] = NULL;
	return trie;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct radix* radix_insert(struct radix* root, uint8_t* key, size_t key_len, void* data)
{
	struct radix** radix = &root;
	while(*radix)
	{
		int len, max = MIN((*radix)->key_len, key_len);
		for(len = 0; len < max && (*radix)->key[len] == key[len]; len++);
		if((*radix)->key_len > len)
		{
			struct radix* split = *radix;
			int shift;
			void*** trie;
			*radix = radix_alloc();
			(*radix)->key = split->key;
			(*radix)->key_len = len;
			(*radix)->data = NULL;
			split->key += len;
			split->key_len -= len;
			for(shift = 8, trie = &(*radix)->trie;
				shift > 0;
				shift -= 2, trie = (void***)&(*trie)[0x03 & *split->key >> shift])
					*trie = trie_alloc();	
			split->key++;
			split->key_len--;
			(*trie) = (void**)split;
		}
		if(key_len > len)
		{
			int shift;
			void*** trie;
			key += len;
			key_len -= len;
			for(shift = 8, trie = &(*radix)->trie;
				shift > 0;
				shift -= 2, trie = (void***)&(*trie)[0x03 & *key >> shift])
					if(!*trie)
						*trie = trie_alloc();
			key++;
			key_len--;
			radix = (struct radix**)trie;
			continue;
		}
		if(!(*radix)->data)
			(*radix)->data = data;
		return root;
	}
	*radix = radix_alloc();
	(*radix)->key = key;
	(*radix)->key_len = key_len;
	(*radix)->data = data;
	(*radix)->trie = NULL;
	return root;
}

void* radix_search(struct radix* radix, uint8_t* key, size_t key_len)
{
	while(radix)
	{
		int len, max = MIN(radix->key_len, key_len);
		for(len = 0; len < max && radix->key[len] == key[len]; len++);
		if(radix->key_len == len)
		{
			if(key_len > len)
			{
				int shift;
				void** trie;
				key += len;
				key_len -= len;
				for(shift = 8, trie = radix->trie;
					shift > 0 && trie;
					shift -= 2, trie = (void**)trie[0x03 & *key >> shift]);
				key++;
				key_len--;
				radix = (struct radix*)trie;
				continue;
			}
			return radix->data;
		}
		break;
	}
	return NULL;
}

struct radix_queue
{
	struct radix* head;
	struct radix* tail;
};

static void enqueue(struct radix_queue* q, struct radix* radix)
{
	radix->data = NULL;
	if(q->head)
		q->tail->data = (void*)radix;
	else
		q->head = radix;
	q->tail = radix;
}

static struct radix* dequeue(struct radix_queue* q)
{
	struct radix* radix = q->head;
	if(q->head)
	{
		q->head = (struct radix*)radix->data;
		radix->data = NULL;
	}
	return radix;
}

static void trie_free(struct radix_queue* q, void** trie, int i, void (*free)(void*))
{
	int j;
	if(3 == i)
	{
		for(j = 0; j < 4; j++)
		{
			struct radix* radix;
			if((radix = (struct radix*)trie[j]))
			{
				if(radix->data && free)
					free(radix->data);
				enqueue(q, radix);
			}
		}
	}
	else
	{
		for(j = 0; j < 4; j++)
			if(trie[j])
				trie_free(q, (void**)trie[j], i + 1, free);
	}
	ffree(trie);
}

void radix_free(struct radix* radix, void (*free)(void*))
{
	if(radix)
	{
		struct radix_queue q = { .head = NULL };
		if(radix->data && free)
			free(radix->data);
		enqueue(&q, radix);
		while((radix = dequeue(&q)))
		{
			if(radix->trie)
				trie_free(&q, radix->trie, 0, free);
			ffree(radix);
		}
	}
}
