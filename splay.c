#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "splay.h"

enum { ZIG, R = ZIG, ZAG, L = ZAG, COMPARE, ASSEMBLE };

#define Zig() root->left = left->right, left->right = root, root = left
#define zig() down[R]->left = root, down[R] = root, root = left
#define Zag() root->right = right->left, right->left = root, root = right
#define zag() down[L]->right = root, down[L] = root, root = right

struct node* splay(const void* key, struct node* root, int (*compar) (const void *, const void *))
{
	if(root)
	{
		struct node top[1] = { { NULL, NULL } }, * down[2] = { top, top };
		int step = COMPARE;
		do
		{
			int integer;
			struct node* left, * right;
			switch(step)
			{
			case ZIG:
				step = ASSEMBLE;
				if((left = root->left))
				{
					integer = compar(key, (const void*)left->key);
					if(integer < 0)
					{
						Zig();
						if((left = root->left))
						{
							step = COMPARE;
							zig();
						}
					}
					else
					{
						if(integer > 0)
							step = ZAG;
						zig();
					}
				}
				break;
			case COMPARE:
				integer = compar(key, (const void*)root->key);
				if(integer < 0)
				{ 
					step = ZIG; 
					break; 
				}
				else
				if(integer == 0)
				{
					step = ASSEMBLE; 
					break; 
				}
			case ZAG:
				step = ASSEMBLE;
				if((right = root->right))
				{
					integer = compar((const void*)right->key, key);
					if(integer < 0)
					{
						Zag();
						if((right = root->right))
						{
							step = COMPARE;
							zag();
						}
					}
					else
					{
						if(integer > 0)
							step = ZIG;
						zag();
					}
				}
				break;
			}
		} while(step < ASSEMBLE);
		down[R]->left = root->right;
		down[L]->right = root->left;
		root->right = top->left;
		root->left = top->right;
	}
	return root;
}

struct node* splay_first(struct node* root)
{	
	if(root)
	{
		struct node top[1] = { { NULL, NULL } }, * down[2] = { top, top };
		for(;;)
		{
			struct node* left;
			if((left = root->left))
			{
				Zig();
				if((left = root->left))
					zig();
				else
					break;
			}
			else
				break;
		} 
		down[R]->left = root->right;
		down[L]->right = root->left;
		root->right = top->left;
		root->left = top->right;
	}
	return root;
}

struct node* splay_last(struct node* root)
{	
	if(root)
	{
		struct node top[1] = { { NULL, NULL } }, * down[2] = { top, top };
		for(;;)
		{
			struct node* right;
			if((right = root->right))
			{
				Zag();
				if((right = root->right))
					zag();
				else
					break;
			}
			else
				break;
		}
		down[R]->left = root->right;
		down[L]->right = root->left;
		root->right = top->left;
		root->left = top->right;
	}
	return root;
}

struct node* splay_previous(struct node* root)
{
	if(root)
	{
		if(root->left)
		{
			struct node* left;
			left = root->left = splay_last(root->left);
			left->right = root;
			root->left = NULL;
			root = left;
		}
	}
	return root;
}

struct node* splay_next(struct node* root)
{
	if(root)
	{
		if(root->right)
		{
			struct node* right;
			right = root->right = splay_first(root->right);
			right->left = root;
			root->right = NULL;
			root = right;
		}
	}
	return root;
}

struct node* splay_insert(void* key, struct node* root, size_t width, int (*compar)(const void*, const void*))
{   
    struct node* node;
    if((root = splay(key, root, compar)))
    {   
        int integer = compar(key, root->key);
        if(0 != integer)
        {   
            assert((node = (struct node*)malloc(sizeof(*node) + width)));
            node->left = NULL;
            node->right = NULL;
            memcpy(node->key, key, width);
            if(integer < 0)
            {   
                root->left = splay_last(root->left);
                node->left = root->left;
                root->left = NULL;
                node->right = root;
                root = node;
            }
            else 
            if(0 < integer)
            {   
                root->right = splay_first(root->right);
                node->right = root->right;
                root->right = NULL;
                node->left = root;
                root = node;
            }
        }
    }
    else
    {   
        assert((node = (struct node*)malloc(sizeof(*node) + width)));
        node->left = NULL;
        node->right = NULL;
        memcpy(node->key, key, width);
        root = node;
    }
    return root;
}

struct node* splay_insert_node(struct node* node, struct node* root, int (*compar)(const void*, const void*))
{
	if((root = splay(node->key, root, compar)))
	{
        int integer = compar(node->key, root->key);
		if(integer < 0)
		{                
			root->left = splay_last(root->left);
			node->left = root->left;
			root->left = NULL;
			node->right = root;
			root = node;
		}
		else
		if(0 < integer)
		{
			root->right = splay_first(root->right);
			node->right = root->right;
			root->right = NULL;
			node->left = root;
			root = node;
		}
	}
	else
	{
		node->left = NULL;
		node->right = NULL;
		root = node;
	}
	return root;
}

struct node* splay_iterate(struct node* root, void (*callback)(struct node*))
{
	struct node* node, * right;
	for(node = root = splay_first(root), right = node ? (node->right = splay_first(node->right)) : NULL;
		node;
		node = right, right = node ? (node->right = splay_first(node->right)) : NULL)
			callback(node);
	return root;
}

struct node* splay_remove(struct node* root)
{
	if(root->left)
	{
		struct node* left = root->left = splay_last(root->left);
		left->right = root->right;
		return left;
	}
	else
	if(root->right)
	{
		struct node* right = root->right = splay_first(root->right);
		right->left = root->left;
		return right;
	}
	return NULL;
}
