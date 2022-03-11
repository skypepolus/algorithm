/*
Copyright 2022 BTFF LLC, Young Ho Song

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __splay_h__
#define __splay_h__ __splay_h__
/*
	Self-Adjusting Binary Search Trees
	DANIEL DOMINIC SLEATOR AND ROBERT ENDRE TARJAN	
	Journal of the Association for Computing Machinery, Vol. 32, No3, July 1985, pp. 652-686
	implementation of top-down splaying 
	An alternative algorithm of heap to implement priority queue.
*/
#include <stdint.h>
#include <string.h>

struct Node
{
	struct Node* left;
	struct Node* right;
	uint8_t key[0];
};

struct Node* splay(const void* key, struct Node* root, int (*compar) (const void *, const void *));
struct Node* splay_first(struct Node* root);
struct Node* splay_last(struct Node* root);
struct Node* splay_previous(struct Node* root);
struct Node* splay_next(struct Node* root);
struct Node* splay_alloc(const void* key, struct Node* root, size_t width, int (*compar)(const void*, const void*));
struct Node* splay_insert(struct Node* node, struct Node* root, int (*compar)(const void*, const void*));
struct Node* splay_for_each(struct Node* root, void (*callback)(struct Node*));
struct Node* splay_remove(struct Node* root);

#endif/*__splay_h__*/
