/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "splay.h"

enum { ZIG, R = ZIG, ZAG, L = ZAG, COMPARE, ASSEMBLE };

#define Zig() root->left = left->right, left->right = root, root = left
#define zig() down[R]->left = root, down[R] = root, root = left
#define Zag() root->right = right->left, right->left = root, root = right
#define zag() down[L]->right = root, down[L] = root, root = right

struct Node* splay(const void* key, struct Node* root, int (*compar) (const void *, const void *))
{
	if(root)
	{
		struct Node top[1] = { { NULL, NULL } }, * down[2] = { top, top };
		int step = COMPARE;
		do
		{
			int integer;
			struct Node* left, * right;
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

struct Node* splay_first(struct Node* root)
{	
	if(root)
	{
		struct Node* left;
		while((left = root->left))
			Zig();
	}
	return root;
}

struct Node* splay_last(struct Node* root)
{	
	if(root)
	{
		struct Node* right;
		while((right = root->right))
			Zag();
	}
	return root;
}

struct Node* splay_previous(struct Node* root)
{
	if(root)
	{
		if(root->left)
		{
			struct Node* left;
			left = root->left = splay_last(root->left);
			left->right = root;
			root->left = NULL;
			root = left;
		}
	}
	return root;
}

struct Node* splay_next(struct Node* root)
{
	if(root)
	{
		if(root->right)
		{
			struct Node* right;
			right = root->right = splay_first(root->right);
			right->left = root;
			root->right = NULL;
			root = right;
		}
	}
	return root;
}

struct Node* splay_alloc(const void* key, struct Node* root, size_t width, int (*compar)(const void*, const void*))
{   
    struct Node* node;
    if((root = splay(key, root, compar)))
    {   
        int integer = compar(key, root->key);
        if(0 != integer)
        {   
            assert((node = (struct Node*)malloc(sizeof(*node) + width)));
            node->left = NULL;
            node->right = NULL;
            memcpy(node->key, key, width);
            if(integer < 0)
            {   
				node->right = root;
				node->left = root->left;
				root->left = NULL;
				root = node;
            }
            else 
            if(0 < integer)
            {
				node->left = root;
				node->right = root->right;
				root->right = NULL;
				root = node;
            }
        }
    }
    else
    {   
        assert((node = (struct Node*)malloc(sizeof(*node) + width)));
        node->left = NULL;
        node->right = NULL;
        memcpy(node->key, key, width);
        root = node;
    }
    return root;
}

struct Node* splay_insert(struct Node* node, struct Node* root, int (*compar)(const void*, const void*))
{
	if((root = splay(node->key, root, compar)))
	{
        int integer = compar(node->key, root->key);
		if(integer < 0)
		{                
			node->right = root;
			node->left = root->left;
			root->left = NULL;
			root = node;
		}
		else
		if(0 < integer)
		{
			node->left = root;
			node->right = root->right;
			root->right = NULL;
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

struct Node* splay_for_each(struct Node* root, void (*callback)(struct Node*))
{
	struct Node* node, * right;
	for(node = root = splay_first(root), right = node ? (node->right = splay_first(node->right)) : NULL;
		node;
		node = right, right = node ? (node->right = splay_first(node->right)) : NULL)
			callback(node);
	return root;
}

struct Node* splay_remove(struct Node* root)
{
	if(root)
	{
		if(!root->left)
			root = root->right;
		else
		if(!root->right)
			root = root->left;
		else
		{
			struct Node* left = splay_last(root->left);
			left->right = root->right;
			root = left;
		}
	}
	return root;
}
