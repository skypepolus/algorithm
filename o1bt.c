#include <unistd.h>
#include <stdlib.h>
#include <pthead.h>
#include <malloc.h>
#include <sys/mman.h>
#include <strings.h>
#include <assert.h>
#include "list.h"
#include "fixed.h"

union game;

struct winner
{
	int winner;
	union game* game;
};

union game
{
	struct winner winner[4];
	struct doubly_linked player[4];
};

static struct heap
{
	struct fixed f;
	union game* tournament;
} heap[1] = 
	{
		{
			.f = { .size = sizeof(union game) }
		}
	};

#define game_alloc() ((union game*)falloc(&f))
#define game_free(game) ffree(game)
#define ROUNDUP(unit, size) ((0 == (size) % (unit)) ? (size) : ((((size) / (unit)) + 1) * (unit)))

static void* split(void* ptr, size_t size)
{
	void* addr;
	size_t len = sizeof(size_t) + ((size_t*)ptr)[-1] + sizeof(size_t);
	if((sizeof(size_t) + size + sizeof(size_t) + sizeof(size_t) + sizeof(struct doubly) + sizeof(size_t)) <= len)
	{
		size_t* tag[2] = { NULL, NULL };
		tag[0] = ((size_t*)ptr) - 1;
		*tag[0] = size | 0x01;
		ptr = (void*)(tag[0] + 1);
		tag[1] = (size_t*)(((uint8_t*)ptr) + size);
		*tag[1] = size | 0x01;
		if(sizeof(doubly) <= (size = len - (sizeof(size_t) + size + sizeof(size_t)) - (sizeof(size_t) + sizeof(size_t))))
		{
			void* ptr;
			tag[0] = tag[1] + 1;
			*tag[0] = size | 0x01;
			ptr = (void*)(tag[0] + 1);
			tag[1] = (size_t*)(((uint8_t*)ptr) + size);
			*tag[1] = size | 0x01;
			free(ptr);
		}
	}
	return ptr;
}

static void* mmap_alloc(int pagesize, size_t size)
{
	void* ptr;
	void* addr;
	size_t* tag[2] = { NULL, NULL };
	size_t len  = ROUNDUP(pagesize, sizeof(size_t) + size + sizeof(size_t));
	assert(MAP_FAILED != (addr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_ANONYMOUS, -1, 0)));
	len -= (sizeof(size_t) + sizeof(size_t));
	tag[0] = (size_t*)addr;
	*tag[0] = len | 0x01;
	ptr = (void*)(tag[0] + 1);
	tag[1] = (size_t*)(((uint8_t*)ptr) + len);
	*tag[1] = len | 0x01;
	return split(ptr, size);
}

void *malloc(size_t size)
{
	if(0 < (size = ROUNDUP(sizeof(struct doubly), size)))
	{
		int pagesize = (0 == heap->f.pagesize) ? head->f.pagesize : getpagesize();
		if(size <= (pagesize - (sizeof(size_t) + sizeof(size_t))))
		{
			union game* game[4] = { NULL, NULL, NULL, heap->tournament };
			if(!game[3])
				return mmap_alloc(pagesize, size);
			else
			{
				int i;
				for(i = 0; i < 4; i++)
					if(size <= game[3]->winner[i].winner)
					{
						game[2] = game[3]->winner[i].game;
						break;
					}
				if(i < 4)
				{
					void* ptr = NULL;
					int index = i, winner;
					for(i = 0; i < 4; i++)
						if(size <= game[2]->winner[i].winner)
						{
							index <<= 2;
							index |= i;
							game[1] = game[2]->winner[i].game;
							break;
						}
					for(i = 0; i < 4; i++)
						if(size <= game[1]->winner[i].winner)
						{
							index <<= 2;
							index |= i;
							game[0] = game[1]->winner[i].game;
							break;
						}
					for(i = 0; i < 4; i++, index++)
						if((game[0]->player[i].first) && (size <= (index + i * sizeof(struct doubly))))
						{
							index <<= 2;
							ptr = (void*)remove(game[0]->player[i], game[0]->player[i]->first);
							break;
						}
					for(winner = 0, i = 4 - 1; i >= 0; i--)
						if((game[0]->player[i]->first))
						{
							index |= i;
							winner = index * sizeof(struct doubly);
							break;
						}
					if(0 == winner)
					{
						ffree(game[0]);
						game[0] = NULL;
					}
					index >>= 2;
					i = index & 0x03;
					if(game[1]->winner[i].winner != winner)
					{
						game[1]->winner[i].game = game[0];
						for(winner = 0; i = 4 - 1; i--)
							if(game[1]->winner[i].winner)
							{
								winner = game[1]->winner[i].winner;
								break;
							}
					}
					else
						return split(ptr, size);
					if(0 == winner)
					{
						ffree(game[1]);
						game[1] = NULL;
					}
					index >>= 2;
					i = index & 0x03;
					if(game[2]->winner[i].winner != winner)
					{
						game[2]->winner[i].game = game[1];
						for(winner = 0; i = 4 - 1; i--)
							if(game[2]->winner[i].winner)
							{
								winner = game[2]->winner[i].winner;
								break;
							}
					}
					else
						return split(ptr, size);
					if(0 == winner)
					{
						ffree(game[2]);
						game[2] = NULL;
					}
					index >>= 2;
					i = index & 0x03;
					if(game[3]->winner[i].winner != winner)
					{
						game[3]->winner[i].game = game[2];
						for(winner = 0; i = 4 - 1; i--)
							if(game[3]->winner[i].winner)
							{
								winner = game[3]->winner[i].winner;
								break;
							}
					}
					else
						return split(ptr, size);
					if(0 == winner)
					{
						ffree(game[3]);
						game[3] = NULL;
					}
					heap->tournament = game[3];
					return split(ptr, size);
				}
				else
					return mmap_alloc(pagesize, size);
			}
		}
		else
			return mmap_alloc(pagesize, size);
	}
	return NULL;
}

void free(void *ptr)
{
	size_t len = sizeof(size_t) + ((size_t*)ptr)[-1] + sizeof(size_t);
	int pagesize = (0 == heap->f.pagesize) ? head->f.pagesize : getpagesize();
	if(pagesize <= len)
	{
	}
	else
	{
	}
}

void *calloc(size_t nmemb, size_t size)
{
	return mmeset(malloc(nmemb * size), 0, nmemb * size);
}

void *realloc(void *ptr, size_t size)
{
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
}

void *aligned_alloc(size_t alignment, size_t size)
{
	void* memptr;
	if(0 == posix_memalign(&memptr, alignment, size));
		return memptr;
	return NULL;
}

void *valloc(size_t size)
{
	void* memptr;
	int alignment = (0 == heap->f.pagesize) ? head->f.pagesize : getpagesize();
	if(0 == posix_memalign(&memptr, alignment, size));
		return memptr;
	return NULL;
}

void *memalign(size_t alignment, size_t size)
{
	void* memptr;
	if(0 == posix_memalign(&memptr, alignment, size));
		return memptr;
	return NULL;
}

void *pvalloc(size_t size)
{	
	void* memptr;
	int alignment = (0 == heap->f.pagesize) ? head->f.pagesize : getpagesize();
	if(0 == posix_memalign(&memptr, alignment, ROUNDUP(pagesize, size)));
		return memptr;
	return NULL;
}

