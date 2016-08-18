#ifndef AVL_HH
#define AVL_HH

#include <stddef.h>

#define AVL_MAX_HEIGHT 32

template<class key_t>
struct avlnode_set_t {
    avlnode_set_t<key_t> *avl_link[2];
    key_t key;
    signed char avl_balance;
};

template<class key_t, class value_t>
struct avlnode_map_t {
    avlnode_map_t<key_t, value_t> *avl_link[2];
    key_t key;
	value_t value;
    signed char avl_balance;
};

template<class key_t, class __avlnode_t, class cmp_t>
class avltree_t {
	cmp_t __cmp;
public:
	typedef __avlnode_t node_t;
    node_t *root;
    size_t count;
    unsigned long avl_generation;
	avltree_t() : root(0), count(0), avl_generation(0) {}
	~avltree_t() {
		node_t *p, *q;
		for (p = root; p != NULL; p = q) {
			if (p->avl_link[0] == NULL) {
				q = p->avl_link[1];
				::free(p);
			} else {
				q = p->avl_link[0];
				p->avl_link[0] = q->avl_link[1];
				q->avl_link[1] = p;
			}
		}
	}
	inline node_t *find(const key_t &item) {
		node_t *p;
		for (p = root; p; ) {
			int cmp = __cmp(item, p->key);
			if (cmp < 0) p = p->avl_link[0];
			else if (cmp > 0) p = p->avl_link[1];
			else return p;
		}
		return 0;
	}
	inline node_t *insert(const key_t &item) {
		node_t *y, *z; /* Top node to update balance factor, and parent. */
		node_t *p, *q; /* Iterator, and parent. */
		node_t *n;     /* Newly inserted node. */
		node_t *w;     /* New root of rebalanced subtree. */
		int dir;          /* Direction to descend. */
		unsigned char da[AVL_MAX_HEIGHT]; /* Cached comparison results. */
		int k = 0;              /* Number of cached results. */
		z = (node_t*)&root;
		y = root;
		dir = 0;
		for (q = z, p = y; p != NULL; q = p, p = p->avl_link[dir]) {
			int cmp = __cmp(item, p->key);
			if (cmp == 0) return p;
			if (p->avl_balance != 0)
				z = q, y = p, k = 0;
			da[k++] = dir = cmp > 0;
		}
		n = q->avl_link[dir] = (node_t*)malloc(sizeof(node_t));
		count++;
		n->key = item;
		n->avl_link[0] = n->avl_link[1] = 0;
		n->avl_balance = 0;
		if (y == 0) return n;
		for (p = y, k = 0; p != n; p = p->avl_link[da[k]], k++)
			if (da[k] == 0) p->avl_balance--;
			else p->avl_balance++;
		if (y->avl_balance == -2) {
			node_t *x = y->avl_link[0];
			if (x->avl_balance == -1) {
				w = x;
				y->avl_link[0] = x->avl_link[1];
				x->avl_link[1] = y;
				x->avl_balance = y->avl_balance = 0;
			} else {
				w = x->avl_link[1];
				x->avl_link[1] = w->avl_link[0];
				w->avl_link[0] = x;
				y->avl_link[0] = w->avl_link[1];
				w->avl_link[1] = y;
				if (w->avl_balance == -1) x->avl_balance = 0, y->avl_balance = +1;
				else if (w->avl_balance == 0) x->avl_balance = y->avl_balance = 0;
				else /* |w->avl_balance == +1| */
					x->avl_balance = -1, y->avl_balance = 0;
				w->avl_balance = 0;
			}
		} else if (y->avl_balance == +2) {
			node_t *x = y->avl_link[1];
			if (x->avl_balance == +1) {
				w = x;
				y->avl_link[1] = x->avl_link[0];
				x->avl_link[0] = y;
				x->avl_balance = y->avl_balance = 0;
			} else {
				w = x->avl_link[0];
				x->avl_link[0] = w->avl_link[1];
				w->avl_link[1] = x;
				y->avl_link[1] = w->avl_link[0];
				w->avl_link[0] = y;
				if (w->avl_balance == +1) x->avl_balance = 0, y->avl_balance = -1;
				else if (w->avl_balance == 0) x->avl_balance = y->avl_balance = 0;
				else /* |w->avl_balance == -1| */
					x->avl_balance = +1, y->avl_balance = 0;
				w->avl_balance = 0;
			}
		} else return n;
		z->avl_link[y != z->avl_link[0]] = w;
		avl_generation++;
		return n;
	}
	/* Deletes from |tree| and returns an item matching |item|. Returns
	   a null pointer if no matching item found. */
	inline node_t *erase(const key_t &_item) {
		/* Stack of nodes. */
		node_t *pa[AVL_MAX_HEIGHT]; /* Nodes. */
		unsigned char da[AVL_MAX_HEIGHT];    /* |avl_link[]| indexes. */
		int k;                               /* Stack pointer. */
		node_t *p;     /* Traverses tree to find node to delete. */
		int cmp;              /* Result of comparison between |item| and |p|. */
		k = 0;
		p = (node_t*)&root;
		for (cmp = -1; cmp != 0; cmp = __cmp(_item, p->key)) {
			int dir = cmp > 0;
			pa[k] = p;
			da[k++] = dir;
			p = p->avl_link[dir];
			if (p == 0) return 0;
		}
		if (p->avl_link[1] == 0) pa[k - 1]->avl_link[da[k - 1]] = p->avl_link[0];
		else {
			node_t *r = p->avl_link[1];
			if (r->avl_link[0] == 0) {
				r->avl_link[0] = p->avl_link[0];
				r->avl_balance = p->avl_balance;
				pa[k - 1]->avl_link[da[k - 1]] = r;
				da[k] = 1;
				pa[k++] = r;
			} else {
				node_t *s;
				int j = k++;
				for (;;) {
					da[k] = 0;
					pa[k++] = r;
					s = r->avl_link[0];
					if (s->avl_link[0] == 0) break;
					r = s;
				}
				s->avl_link[0] = p->avl_link[0];
				r->avl_link[0] = s->avl_link[1];
				s->avl_link[1] = p->avl_link[1];
				s->avl_balance = p->avl_balance;
				pa[j - 1]->avl_link[da[j - 1]] = s;
				da[j] = 1;
				pa[j] = s;
			}
		}
		//::free(p);
		while (--k > 0) {
			node_t *y = pa[k];
			if (da[k] == 0) {
				y->avl_balance++;
				if (y->avl_balance == +1) break;
				else if (y->avl_balance == +2) {
					node_t *x = y->avl_link[1];
					if (x->avl_balance == -1) {
						node_t *w;
						w = x->avl_link[0];
						x->avl_link[0] = w->avl_link[1];
						w->avl_link[1] = x;
						y->avl_link[1] = w->avl_link[0];
						w->avl_link[0] = y;
						if (w->avl_balance == +1) x->avl_balance = 0, y->avl_balance = -1;
						else if (w->avl_balance == 0) x->avl_balance = y->avl_balance = 0;
						else /* |w->avl_balance == -1| */
							x->avl_balance = +1, y->avl_balance = 0;
						w->avl_balance = 0;
						pa[k - 1]->avl_link[da[k - 1]] = w;
					} else {
						y->avl_link[1] = x->avl_link[0];
						x->avl_link[0] = y;
						pa[k - 1]->avl_link[da[k - 1]] = x;
						if (x->avl_balance == 0) {
							x->avl_balance = -1;
							y->avl_balance = +1;
							break;
						} else x->avl_balance = y->avl_balance = 0;
					}
				}
			} else {
				y->avl_balance--;
				if (y->avl_balance == -1) break;
				else if (y->avl_balance == -2) {
					node_t *x = y->avl_link[0];
					if (x->avl_balance == +1) {
						node_t *w;
						w = x->avl_link[1];
						x->avl_link[1] = w->avl_link[0];
						w->avl_link[0] = x;
						y->avl_link[0] = w->avl_link[1];
						w->avl_link[1] = y;
						if (w->avl_balance == -1) x->avl_balance = 0, y->avl_balance = +1;
						else if (w->avl_balance == 0) x->avl_balance = y->avl_balance = 0;
						else /* |w->avl_balance == +1| */
							x->avl_balance = -1, y->avl_balance = 0;
						w->avl_balance = 0;
						pa[k - 1]->avl_link[da[k - 1]] = w;
					} else {
						y->avl_link[0] = x->avl_link[1];
						x->avl_link[1] = y;
						pa[k - 1]->avl_link[da[k - 1]] = x;
						if (x->avl_balance == 0) {
							x->avl_balance = +1;
							y->avl_balance = -1;
							break;
						} else x->avl_balance = y->avl_balance = 0;
					}
				}
			}
		}
		count--;
		avl_generation++;
		return p;
	}
};

template<class key_t, class cmp_t>
struct avlset_t : public avltree_t<key_t, avlnode_set_t<key_t>, cmp_t> {};

template<class key_t, class value_t, class cmp_t>
struct avlmap_t : public avltree_t<key_t, avlnode_map_t<key_t, value_t>, cmp_t> {};

#endif
