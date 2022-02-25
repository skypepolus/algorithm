#include "list.h"

/* Linked list */

struct singly* reverse(struct singly* head)
{
	struct singly* previous, * node, * next;
	for(previous = NULL, node = head, next = node ? node->next : NULL;
		node;
		previous = node, node = next, next = next ? next->next : NULL)
			node->next = previous;
	return previous;
}

void iterate(struct singly* head, void (*callback)(struct singly*, void*), void* data)
{
	struct singly* node, * next;
	for(node = head, next = node ? node->next : NULL;
		node;
		node = next, next = next ? next->next : NULL)
			callback(node, data);
}

void enqueue(struct queue* queue, struct singly* node)
{
	node->next = NULL;
	if(queue->head)
		queue->tail->next = node;
	else
		queue->head = node;
	queue->tail = node;
}

struct singly* dequeue(struct queue* queue)
{
	struct singly* node;
	if((node = queue->head))
	{
		queue->head = node->next;
		node->next = NULL;
	}
	return node;
}

/* Doubly linked list */

void add(struct doubly_linked* list, struct doubly* previous, struct doubly* node)
{
	struct doubly* next;
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

void insert(struct doubly_linked* list, struct doubly* node, struct doubly* next)
{
	struct doubly* previous;
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

void remove(struct doubly_linked* list, struct doubly* node)
{
	struct doubly* previous = node->previous, * next = node->next;
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

