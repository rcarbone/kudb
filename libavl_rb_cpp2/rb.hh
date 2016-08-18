#ifndef RB_H
#define RB_H 1

#include <stdint.h>

#define RB_MAX_HEIGHT 48

#define RB_COMPACT

template<class key_t>
struct rbnode_set_t {
    rbnode_set_t<key_t> *link[2];
    key_t key;
#ifndef RB_COMPACT
    int color;
#endif
};

template<class key_t, class value_t>
struct rbnode_map_t {
    rbnode_map_t<key_t, value_t> *link[2];
    key_t key;
	value_t value;
#ifndef RB_COMPACT
	int color;
#endif
};

#ifdef RB_COMPACT
template<class node_t>
inline node_t *__rb_link(const node_t *p) { return (node_t*)(intptr_t(p) & ssize_t(-2)); }
template<class node_t>
inline void __rb_set_link(node_t *&p, const node_t *q) { p = (node_t*)((uintptr_t(p) & size_t(1)) | size_t(q)); }
template<class node_t>
inline int __rb_color(const node_t *p) { return int(uintptr_t(p->link[1]) & size_t(1)); }
template<class node_t>
inline void __rb_set_red(node_t *p) { p->link[1] = (node_t*)(uintptr_t(p->link[1]) | size_t(1)); }
template<class node_t>
inline void __rb_set_black(node_t *p) { p->link[1] = (node_t*)(intptr_t(p->link[1]) & ssize_t(-2)); }
template<class node_t>
inline void __rb_set_color(node_t *p, int c) { p->link[1] = (node_t*)((intptr_t(p->link[1]) & ssize_t(-2)) | ssize_t(c)); }
#else
template<class node_t>
inline node_t *__rb_link(node_t *p) { return p; }
template<class node_t>
inline void __rb_set_link(node_t *&p, node_t *q) { p = q; }
template<class node_t>
inline int __rb_color(const node_t *p) { return p->color; }
template<class node_t>
inline void __rb_set_red(node_t *p) { p->color = 1; }
template<class node_t>
inline void __rb_set_black(node_t *p) { p->color = 0; }
template<class node_t>
inline void __rb_set_color(node_t *p, int c) { p->color = c; }
#endif

template<class key_t, class __rbnode_t, class cmp_t>
struct rbtree_t {
	cmp_t __cmp;
public:
	typedef __rbnode_t node_t;
	node_t *root;
	size_t count;
	unsigned long generation;
	rbtree_t() : root(0), count(0), generation(0) {};
	~rbtree_t() {
		node_t *p, *q;
		for (p = root; p; p = q) {
			if (!p->link[0]) {
				q = __rb_link(p->link[1]);
				::free(p);
			} else {
				q = p->link[0];
				p->link[0] = __rb_link(q->link[1]);
				__rb_set_link(q->link[1], p);
			}
		}
	}
	inline node_t *find(const key_t &item) {
		node_t *p;
		for (p = root; p;) {
			int cmp = __cmp(item, p->key);
			if (cmp < 0) p = p->link[0];
			else if (cmp > 0) p = __rb_link(p->link[1]);
			else return p;
		}
		return 0;
	}
	inline node_t *insert(const key_t &item, int *is_present) {
		node_t *pa[RB_MAX_HEIGHT], *p, *n;
		unsigned char da[RB_MAX_HEIGHT];
		int k, dir;
		pa[0] = (node_t*)&root;
		da[0] = 0;
		k = 1; *is_present = 0;
		for (p = root; p; p = __rb_link(p->link[dir])) {
			int cmp = __cmp(item, p->key);
			if (cmp == 0) {
				*is_present = 1;
				return p;
			}
			pa[k] = p;
			da[k++] = dir = cmp > 0;
		}
		n = (node_t*)malloc(sizeof(node_t)); __rb_set_link(pa[k - 1]->link[da[k - 1]], n);
		n->key = item;
		n->link[0] = n->link[1] = 0;
		__rb_set_red(n);
		++count;
		++generation;
		while (k >= 3 && __rb_color(pa[k - 1])) {
			if (da[k - 2] == 0) {
				node_t *y = __rb_link(pa[k - 2]->link[1]);
				if (y && __rb_color(y)) {
					__rb_set_black(pa[k - 1]); __rb_set_black(y);
					__rb_set_red(pa[k - 2]);
					k -= 2;
				} else {
					node_t *x;
					if (da[k - 1] == 0) y = pa[k - 1];
					else {
						x = pa[k - 1];
						y = __rb_link(x->link[1]);
						__rb_set_link(x->link[1], y->link[0]);
						y->link[0] = x;
						pa[k - 2]->link[0] = y;
					}
					x = pa[k - 2];
					__rb_set_red(x);
					__rb_set_black(y);
					x->link[0] = __rb_link(y->link[1]);
					__rb_set_link(y->link[1], x);
					__rb_set_link(pa[k - 3]->link[da[k - 3]], y);
					break;
				}
			} else {
				node_t *y = pa[k - 2]->link[0];
				if (y && __rb_color(y)) {
					__rb_set_black(pa[k - 1]); __rb_set_black(y);
					__rb_set_red(pa[k - 2]);
					k -= 2;
				} else {
					node_t *x;
					if (da[k - 1] == 1) y = pa[k - 1];
					else {
						x = pa[k - 1];
						y = x->link[0];
						x->link[0] = __rb_link(y->link[1]);
						__rb_set_link(y->link[1], x);
						__rb_set_link(pa[k - 2]->link[1], y);
					}
					x = pa[k - 2];
					__rb_set_red(x);
					__rb_set_black(y);
					__rb_set_link(x->link[1], y->link[0]);
					y->link[0] = x;
					__rb_set_link(pa[k - 3]->link[da[k - 3]], y);
					break;
				}
			}
		}
		__rb_set_black(root);
		return n;
	}
	inline node_t *erase(const key_t &item) {
		node_t *pa[RB_MAX_HEIGHT], *p;
		unsigned char da[RB_MAX_HEIGHT];
		int k, cmp;
		k = 0;
		p = (node_t*)&root;
		for (cmp = -1; cmp != 0; cmp = __cmp(item, p->key)) {
			int dir = cmp > 0;
			pa[k] = p;
			da[k++] = dir;
			p = __rb_link(p->link[dir]);
			if (p == 0) return 0;
		}
		if (__rb_link(p->link[1]) == 0) __rb_set_link(pa[k - 1]->link[da[k - 1]], p->link[0]);
		else {
			int t;
			node_t *r = __rb_link(p->link[1]);
			if (r->link[0] == 0) {
				r->link[0] = p->link[0];
				t = __rb_color(r);
				__rb_set_color(r, __rb_color(p));
				__rb_set_color(p, t);
				__rb_set_link(pa[k - 1]->link[da[k - 1]], r);
				da[k] = 1;
				pa[k++] = r;
			} else {
				node_t *s;
				int j = k++;
				for (;;) {
					da[k] = 0;
					pa[k++] = r;
					s = r->link[0];
					if (s->link[0] == 0) break;
					r = s;
				}
				da[j] = 1;
				pa[j] = s;
				__rb_set_link(pa[j - 1]->link[da[j - 1]], s);
				s->link[0] = p->link[0];
				r->link[0] = __rb_link(s->link[1]);
				__rb_set_link(s->link[1], __rb_link(p->link[1]));
				t = __rb_color(s);
				__rb_set_color(s, __rb_color(p));
				__rb_set_color(p, t);
			}
		}
		if (!__rb_color(p)) {
			for (;;) {
				node_t *x = __rb_link(pa[k - 1]->link[da[k - 1]]);
				if (x && __rb_color(x)) {
					__rb_set_black(x);
					break;
				}
				if (k < 2) break;
				if (da[k - 1] == 0) {
					node_t *w = __rb_link(pa[k - 1]->link[1]);
					if (__rb_color(w)) {
						__rb_set_black(w);
						__rb_set_red(pa[k - 1]);
						__rb_set_link(pa[k - 1]->link[1], w->link[0]);
						w->link[0] = pa[k - 1];
						__rb_set_link(pa[k - 2]->link[da[k - 2]], w);
						pa[k] = pa[k - 1];
						da[k] = 0;
						pa[k - 1] = w;
						++k;
						w = __rb_link(pa[k - 1]->link[1]);
					}
					if ((!w->link[0] || !__rb_color(w->link[0])) && (!__rb_link(w->link[1]) || !__rb_color(w->link[1]))) {
						__rb_set_red(w);
					} else {
						if (!__rb_link(w->link[1]) || !__rb_color(w->link[1])) {
							node_t *y = w->link[0];
							__rb_set_black(y); __rb_set_red(w);
							w->link[0] = __rb_link(y->link[1]);
							__rb_set_link(y->link[1], w);
							w = y; __rb_set_link(pa[k - 1]->link[1], y);
						}
						__rb_set_color(w, __rb_color(pa[k - 1]));
						__rb_set_black(pa[k - 1]);
						__rb_set_black(__rb_link(w->link[1]));
						__rb_set_link(pa[k - 1]->link[1], w->link[0]);
						w->link[0] = pa[k - 1];
						__rb_set_link(pa[k - 2]->link[da[k - 2]], w);
						break;
					}
				} else {
					node_t *w = pa[k - 1]->link[0];
					if (__rb_color(w)) {
						__rb_set_black(w); __rb_set_red(pa[k - 1]);
						pa[k - 1]->link[0] = __rb_link(w->link[1]);
						__rb_set_link(w->link[1], pa[k - 1]);
						__rb_set_link(pa[k - 2]->link[da[k - 2]], w);
						pa[k] = pa[k - 1];
						da[k] = 1;
						pa[k - 1] = w;
						++k;
						w = pa[k - 1]->link[0];
					}
					if ((!w->link[0] || !__rb_color(w->link[0])) && (!__rb_link(w->link[1]) || !__rb_color(w->link[1]))) {
						__rb_set_red(w);
					} else {
						if (!w->link[0] || !__rb_color(w->link[0])) {
							node_t *y = __rb_link(w->link[1]);
							__rb_set_black(y); __rb_set_red(w);
							__rb_set_link(w->link[1], y->link[0]);
							y->link[0] = w;
							w = pa[k - 1]->link[0] = y;
						}
						__rb_set_color(w, __rb_color(pa[k - 1]));
						__rb_set_black(pa[k - 1]); __rb_set_black(w->link[0]);
						pa[k - 1]->link[0] = __rb_link(w->link[1]);
						__rb_set_link(w->link[1], pa[k - 1]);
						__rb_set_link(pa[k - 2]->link[da[k - 2]], w);
						break;
					}
				}
				--k;
			}
		}
		//::free(p);
		--count;
		++generation;
		return p;
	}
};

template<class key_t, class cmp_t>
struct rbset_t : public rbtree_t<key_t, rbnode_set_t<key_t>, cmp_t> {};

template<class key_t, class value_t, class cmp_t>
struct rbmap_t : public rbtree_t<key_t, rbnode_map_t<key_t, value_t>, cmp_t> {};


#endif
