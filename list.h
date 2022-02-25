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
