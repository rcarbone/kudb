/* This program is adapted from the source codes from Thomas Niemann.
 * See http://www.darkridge.com/~jpr5/archive/alg/node21.html for
 * original code. The original code is not subjected to copyright
 * restrictions and the modified version is distributed under MIT/X11
 * license. */

/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractivechaos@aol.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef RB_TREE_HH_
#define RB_TREE_HH_

#include <stdlib.h>

template <class VTYPE>
struct __ac_rb_tree_node
{
	VTYPE v; // value
	bool c; // color, "true" for red and "false" for black
	__ac_rb_tree_node *l, *r, *p; // left, right, parent
};

template <class VTYPE, class cmp_t>
class rb_tree
{
public:
	typedef __ac_rb_tree_node<VTYPE> node_t;
private:
	node_t sentile;
	cmp_t __cmp;
protected:
	node_t *nil, *root;
	inline void rotate_left(node_t*);
	inline void rotate_right(node_t*);
	inline void insert_fix(node_t*);
	inline node_t *insert_aux(node_t*);
	inline void erase_fix(node_t*);
	inline node_t *erase_aux(node_t*);
public:
	rb_tree() 
	{
		nil = &sentile;
		sentile.l = nil; sentile.r = nil; sentile.p = 0;
		sentile.c = false; sentile.v = VTYPE();
		nil = &sentile; root = nil;
	}
	~rb_tree() { destroy(); };
	void destroy();
	inline node_t *search(const VTYPE &val) {
		node_t *cur = root;
		int x;
		while (cur != nil) {
			x = __cmp(val, cur->v);
			if (x == 0) return cur;
			cur = (x < 0)? cur->l : cur->r;
		}
		return 0;
	}
	inline node_t *search(const VTYPE &val, node_t **l, node_t **r) {
		node_t *cur = root;
		int x;
		*l = *r = 0;
		while (cur != nil) {
			x = __cmp(val, cur->v);
			if (x == 0) {
				*l = *r = cur;
				return cur;
			} else if (x < 0) {
				*r = cur; cur = cur->l;
			} else {
				*l = cur; cur = cur->r;
			}
		}
		return 0;
	}
	inline void insert(const VTYPE &val) {
		node_t *x = (node_t*)malloc(sizeof(node_t));
		x->v = val;
		insert_aux(x);
	}
	inline bool erase(const VTYPE &val) {
		node_t *x = search(val);
		if (x) {
			erase(x);
			return true;
		} else return false;
	}
	inline void erase(node_t *x) {
		node_t *y = erase_aux(x);
		if (y) free(y);
	}
};

template <class VTYPE, class cmp_t>
void rb_tree<VTYPE, cmp_t>::destroy()
{
	int top = 0, max = 1;
	node_t *p, **stack = (node_t**)malloc(sizeof(node_t*) * max);
	stack[top++] = root;
	while (top) {
		p = stack[--top];
		if (p->l != nil) stack[top++] = p->l;
		if (p->r != nil) {
			if (top == max) {
				max <<= 1;
				stack = (node_t**)realloc(stack, sizeof(node_t*) * max);
			}
			stack[top++] = p->r;
		}
		free(p);
	}
	free(stack);
	root = nil;
}
template <class VTYPE, class cmp_t>
inline void rb_tree<VTYPE, cmp_t>::rotate_left(node_t *x)
{
	node_t *y = x->r;
	// establisk r->r link
	x->r = y->l;
	if (y->l != nil) y->l->p = x;
	// establish y->p link
	if (y != nil) y->p = x->p;
	if (x->p) { // x == root
		if (x == x->p->l) x->p->l = y;
			else x->p->r = y;
	} else root = y;
	//link x and y
	y->l = x;
	if (x != nil) x->p = y;
}
template <class VTYPE, class cmp_t>
inline void rb_tree<VTYPE, cmp_t>::rotate_right(node_t *x)
{
	node_t *y = x->l;
	x->l = y->r;
	if (y->r != nil) y->r->p = x;
	if (y != nil) y->p = x->p;
	if (x->p) { // x == root
		if (x == x->p->r) x->p->r = y;
			else x->p->l = y;
	} else root = y;
	y->r = x;
	if (x != nil) x->p = y;
}
template <class VTYPE, class cmp_t>
inline void rb_tree<VTYPE, cmp_t>::insert_fix(node_t *x)
{
	while (x != root && x->p->c == true) { // check red-black properties
		if (x->p == x->p->p->l) { // a violation
			node_t *y = x->p->p->r;
			if (y->c == true) { // uncle is red
				x->p->c = false;
				y->c = false;
				x->p->p->c = true;
				x = x->p->p;
			} else { // uncle is black
				if (x == x->p->r) { // make x a left child
					x = x->p;
					rotate_left(x);
				}
				// recolor and rotate
				x->p->c = false;
				x->p->p->c = true;
				rotate_right(x->p->p);
			}
		} else {
			// mirror image for above code
			node_t *y = x->p->p->l;
			if (y->c == true) {
				x->p->c = false;
				y->c = false;
				x->p->p->c = true;
				x = x->p->p;
			} else {
				if (x == x->p->l) {
					x = x->p;
					rotate_right(x);
				}
				x->p->c = false;
				x->p->p->c = true;
				rotate_left(x->p->p);
			}
		}
	}
	root->c = false;
}
template <class VTYPE, class cmp_t>
inline __ac_rb_tree_node<VTYPE> *rb_tree<VTYPE, cmp_t>::insert_aux(node_t *x)
{
	node_t *cur, *par;
	int z;
	// find where node belongs
	cur = root; par = 0;
	while (cur != nil) {
		z = __cmp(x->v, cur->v);
		if (z == 0) { cur->v = x->v; return cur; } // even v1==v2, v1 may still different from v2
		par = cur;
		cur = (z < 0)? cur->l : cur->r;
	}
	// setup new node, except for x->v that has been settled before the call.
	x->p = par; x->l = nil; x->r = nil; x->c = true;
	// insert the node into the tree
	if (par) {
		if (__cmp(x->v, par->v) < 0) par->l = x;
			else par->r = x;
	} else root = x;
	insert_fix(x);
	return x;
}
template <class VTYPE, class cmp_t>
inline void rb_tree<VTYPE, cmp_t>::erase_fix(node_t *x)
{
	node_t *xp = x->p;
	while (x != root && x->c == false) {
		if (x == xp->l) {
			node_t *w = xp->r;
			if (w->c == true) {
				w->c = false;
				xp->c = true;
				rotate_left(xp);
				w = xp->r;
			}
			if (w->l->c == false && w->r->c == false) {
				w->c = true;
				x = xp; xp = x->p;
			} else {
				if (w->r->c == false) {
					w->l->c = false;
					w->c = true;
					rotate_right(w);
					w = xp->r;
				}
				w->c = xp->c;
				xp->c = false;
				w->r->c = false;
				rotate_left(xp);
				x = root; break;
			}
		} else { // mirror image for above code
			node_t *w = xp->l;
			if (w->c == true) {
				w->c = false;
				xp->c = true;
				rotate_right(xp);
				w = xp->l;
			}
			if (w->r->c == false && w->l->c == false) {
				w->c = true;
				x = xp; xp = x->p;
			} else {
				if (w->l->c == false) {
					w->r->c = false;
					w->c = true;
					rotate_left(w);
					w = xp->l;
				}
				w->c = xp->c;
				xp->c = false;
				w->l->c = false;
				rotate_right(xp);
				x = root; break;
			}
		}
	}
	x->c = false;
}
template <class VTYPE, class cmp_t>
inline __ac_rb_tree_node<VTYPE> *rb_tree<VTYPE, cmp_t>::erase_aux(node_t *z)
{
	node_t *x, *y;
	
	if (!z || z == nil) return 0;
	if (z->l == nil || z->r == nil) {
		y = z;
	} else { // find tree successor with a null node as child
		y = z->r;
		while (y->l != nil) y = y->l;
	}
	// now y has one child at most. let x be y's only child
	if (y->l != nil) x = y->l;
	else x = y->r;
	// remove y from the parent chain
	x->p = y->p;
	if (y->p) {
		if (y == y->p->l) y->p->l = x;
			else y->p->r = x;
	} else root = x;
	if (y != z) z->v = y->v;
	if (y->c == false) erase_fix(x);
	return y;
}

#endif // RB_TREE_HH_
