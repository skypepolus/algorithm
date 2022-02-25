/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include "fixed.h"
#include "list.h"

#define ROUNDUP(total, size) (0 == (total) % (size) ? (total) : ((total) / (size) + 1) * (size))

static int pagesize = 0;

struct page
{
	struct doubly node;
	struct fixed* fixed;
	int count;
	struct singly* free;
	uint8_t* bump;
	uint8_t begin[0];
};

#define page_node(ptr) ((struct page*)((uint8_t*)(ptr) - (uint8_t*)&((struct page*)NULL)->node))

void* falloc(struct fixed* fixed)
{
	struct page* page;
	struct singly* node;
	if(fixed->page_list.first)
		page = page_node(fixed->page_list.first);
	else
	{
		if(0 == fixed->page_count)
		{
			if(0 == pagesize)
				pagesize = getpagesize();
			fixed->pagesize = ROUNDUP(sizeof(struct page) + fixed->size, pagesize);
			fixed->page_count = (fixed->pagesize - sizeof(struct page)) / fixed->size;
		}
		assert(MAP_FAILED != (void*)(page = (struct page*)mmap(NULL, fixed->pagesize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)));
		page->fixed = fixed;
		page->count = 0;
		page->free = NULL;
		page->bump = page->begin;
		add(&fixed->page_list, fixed->page_list.last, &page->node);
	}	
	if(page->free)
	{
		node = page->free;
		page->free = node->next;
	}
	else
	{
		node = (struct singly*)page->bump;
		page->bump += fixed->size;
	}	
	page->count++;
	if(fixed->page_count == page->count)
		remove(&fixed->page_list, &page->node);
	return (void*)node;
}

void ffree(void* ptr)
{
	struct page* page = (struct page*)(void*)(((unsigned long)ptr / pagesize) * pagesize);
	struct fixed* fixed = page->fixed;
	struct singly* node = (struct singly*)ptr;
	node->next = page->free;
	page->free = node;
	if(fixed->page_count > page->count)
		remove(&fixed->page_list, &page->node);
	page->count--;
	if(0 == page->count)
		assert(0 == munmap((void*)page, fixed->pagesize));
	else
	if(fixed->page_list.last && page->count <= page_node(fixed->page_list.last)->count)
		add(&fixed->page_list, fixed->page_list.last, &page->node);
	else
	if(fixed->page_list.first && page->count > page_node(fixed->page_list.first)->count)
		insert(&fixed->page_list, &page->node, fixed->page_list.first);
	else
		add(&fixed->page_list, fixed->page_list.first, &page->node);
}

