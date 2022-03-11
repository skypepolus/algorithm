/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "list.h"

/* Linked list */

struct Singly* reverse(struct Singly* head)
{
	struct Singly* previous, * node, * next;
	for(previous = NULL, node = head, next = node ? node->next : NULL;
		node;
		previous = node, node = next, next = next ? next->next : NULL)
			node->next = previous;
	return previous;
}

void for_each(struct Singly* head, void (*callback)(struct Singly*, void*), void* data)
{
	struct Singly* node, * next;
	for(node = head, next = node ? node->next : NULL;
		node;
		node = next, next = next ? next->next : NULL)
			callback(node, data);
}

void enqueue(struct Queue* queue, struct Singly* node)
{
	node->next = NULL;
	if(queue->head)
		queue->tail->next = node;
	else
		queue->head = node;
	queue->tail = node;
}

struct Singly* dequeue(struct Queue* queue)
{
	struct Singly* node;
	if((node = queue->head))
	{
		queue->head = node->next;
		node->next = NULL;
	}
	return node;
}

/* Doubly linked list */

void add(struct DoublyLinked* list, struct Doubly* previous, struct Doubly* node)
{
	struct Doubly* next;
	if(previous)
	{
		next = previous->next;
		previous->next = node;		
	}
	else
	{
		next = list->first;
		list->first = node;
	}
	if(next)
		next->previous = node;
	else
		list->last = node;
	node->previous = previous;
	node->next = next;
}

void insert(struct DoublyLinked* list, struct Doubly* node, struct Doubly* next)
{
	struct Doubly* previous;
	if(next)
	{
		previous = next->previous;
		next->previous = node;
	}
	else
	{
		previous = list->last;
		list->last = node;
	}
	if(previous)
		previous->next = node;
	else
		list->first = node;
	node->next = next;
	node->previous = previous;
}

void remove(struct DoublyLinked* list, struct Doubly* node)
{
	struct Doubly* previous = node->previous, * next = node->next;
	if(previous)
		previous->next = next;
	else
		list->first = next;
	if(next)
		next->previous = previous;
	else
		list->last = previous;
	node->previous = node->next = NULL;
}
