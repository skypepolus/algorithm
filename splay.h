#ifndef __splay_h__
#define __splay_h__ __splay_h__

#include <stdint.h>
#include <string.h>

struct node
{
	struct node* left;
	struct node* right;
	uint8_t key[0];
};

struct node* splay(const void* key, struct node* root, int (*compar) (const void *, const void *));
struct node* splay_first(struct node* root);
struct node* splay_last(struct node* root);
struct node* splay_previous(struct node* root);
struct node* splay_next(struct node* root);
struct node* splay_insert(void* key, struct node* root, size_t width, int (*compar)(const void*, const void*));
struct node* splay_insert_node(struct node* node, struct node* root, int (*compar)(const void*, const void*));
struct node* splay_iterate(struct node* root, void (*callback)(struct node*));
struct node* splay_remove(struct node* root);

#endif/*__splay_h__*/
