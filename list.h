/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __list_h__
#define __list_h__ __list_h__

#include <stdlib.h>
#include <stdint.h>

/* Linked list */

struct singly
{
	struct singly* next;
	uint8_t data[0];
};

typedef void (*singly_callback_t)(struct singly*, void*);

struct queue
{
	struct singly* head;
	struct singly* tail;
};

struct singly* reverse(struct singly* head);
void iterate(struct singly* head, void (*callback)(struct singly*, void*), void* data);
void enqueue(struct queue* queue, struct singly* node);
struct singly* dequeue(struct queue* queue);

/* Doubly linked list */

struct doubly
{
	struct doubly* previous;
	struct doubly* next;
	uint8_t data[0];
};

struct doubly_linked
{
	struct doubly* first;
	struct doubly* last;
};

void add(struct doubly_linked* list, struct doubly* previous, struct doubly* node);
void insert(struct doubly_linked* list, struct doubly* node, struct doubly* next);
void remove(struct doubly_linked* list, struct doubly* node);

#endif/*__list_h__*/
