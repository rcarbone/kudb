#ifndef RB_H
#define RB_H 1

#define RB_MAX_HEIGHT 48
#define RB_BLACK 0
#define RB_RED 1

template<class key_t>
struct rbnode_set_t {
    rbnode_set_t<key_t> *rb_link[2];
    key_t key;
    signed char color;
};

template<class key_t, class value_t>
struct rbnode_map_t {
    rbnode_map_t<key_t, value_t> *rb_link[2];
    key_t key;
	value_t value;
	signed char color;
};

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
		for (p = root; p != NULL; p = q) {
			if (p->rb_link[0] == NULL) {
				q = p->rb_link[1];
				::free(p);
			} else {
				q = p->rb_link[0];
				p->rb_link[0] = q->rb_link[1];
				q->rb_link[1] = p;
			}
		}
	}
	inline node_t *find(const key_t &item) {
		node_t *p;
		for (p = root; p;) {
			int cmp = __cmp(item, p->key);
			if (cmp < 0) p = p->rb_link[0];
			else if (cmp > 0) p = p->rb_link[1];
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
		for (p = root; p; p = p->rb_link[dir]) {
			int cmp = __cmp(item, p->key);
			if (cmp == 0) {
				*is_present = 1;
				return p;
			}
			pa[k] = p;
			da[k++] = dir = cmp > 0;
		}
		n = pa[k - 1]->rb_link[da[k - 1]] = (node_t*)malloc(sizeof(node_t));
		n->key = item;
		n->rb_link[0] = n->rb_link[1] = 0;
		n->color = RB_RED;
		++count;
		++generation;
		while (k >= 3 && pa[k - 1]->color == RB_RED) {
			if (da[k - 2] == 0) {
				node_t *y = pa[k - 2]->rb_link[1];
				if (y && y->color == RB_RED) {
					pa[k - 1]->color = y->color = RB_BLACK;
					pa[k - 2]->color = RB_RED;
					k -= 2;
				} else {
					node_t *x;
					if (da[k - 1] == 0) y = pa[k - 1];
					else {
						x = pa[k - 1];
						y = x->rb_link[1];
						x->rb_link[1] = y->rb_link[0];
						y->rb_link[0] = x;
						pa[k - 2]->rb_link[0] = y;
					}
					x = pa[k - 2];
					x->color = RB_RED;
					y->color = RB_BLACK;
					x->rb_link[0] = y->rb_link[1];
					y->rb_link[1] = x;
					pa[k - 3]->rb_link[da[k - 3]] = y;
					break;
				}
			} else {
				node_t *y = pa[k - 2]->rb_link[0];
				if (y && y->color == RB_RED) {
					pa[k - 1]->color = y->color = RB_BLACK;
					pa[k - 2]->color = RB_RED;
					k -= 2;
				} else {
					node_t *x;
					if (da[k - 1] == 1) y = pa[k - 1];
					else {
						x = pa[k - 1];
						y = x->rb_link[0];
						x->rb_link[0] = y->rb_link[1];
						y->rb_link[1] = x;
						pa[k - 2]->rb_link[1] = y;
					}
					x = pa[k - 2];
					x->color = RB_RED;
					y->color = RB_BLACK;
					x->rb_link[1] = y->rb_link[0];
					y->rb_link[0] = x;
					pa[k - 3]->rb_link[da[k - 3]] = y;
					break;
				}
			}
		}
		root->color = RB_BLACK;
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
			p = p->rb_link[dir];
			if (p == 0) return 0;
		}
		if (p->rb_link[1] == 0) pa[k - 1]->rb_link[da[k - 1]] = p->rb_link[0];
		else {
			int t;
			node_t *r = p->rb_link[1];
			if (r->rb_link[0] == 0) {
				r->rb_link[0] = p->rb_link[0];
				t = r->color;
				r->color = p->color;
				p->color = t;
				pa[k - 1]->rb_link[da[k - 1]] = r;
				da[k] = 1;
				pa[k++] = r;
			} else {
				node_t *s;
				int j = k++;
				for (;;) {
					da[k] = 0;
					pa[k++] = r;
					s = r->rb_link[0];
					if (s->rb_link[0] == 0) break;
					r = s;
				}
				da[j] = 1;
				pa[j] = s;
				pa[j - 1]->rb_link[da[j - 1]] = s;
				s->rb_link[0] = p->rb_link[0];
				r->rb_link[0] = s->rb_link[1];
				s->rb_link[1] = p->rb_link[1];
				t = s->color;
				s->color = p->color;
				p->color = t;
			}
		}
		if (p->color == RB_BLACK) {
			for (;;) {
				node_t *x = pa[k - 1]->rb_link[da[k - 1]];
				if (x && x->color == RB_RED) {
					x->color = RB_BLACK;
					break;
				}
				if (k < 2) break;
				if (da[k - 1] == 0) {
					node_t *w = pa[k - 1]->rb_link[1];
					if (w->color == RB_RED) {
						w->color = RB_BLACK;
						pa[k - 1]->color = RB_RED;
						pa[k - 1]->rb_link[1] = w->rb_link[0];
						w->rb_link[0] = pa[k - 1];
						pa[k - 2]->rb_link[da[k - 2]] = w;
						pa[k] = pa[k - 1];
						da[k] = 0;
						pa[k - 1] = w;
						++k;
						w = pa[k - 1]->rb_link[1];
					}
					if ((w->rb_link[0] == 0 || w->rb_link[0]->color == RB_BLACK)
						&& (w->rb_link[1] == 0 || w->rb_link[1]->color == RB_BLACK))
					{
						w->color = RB_RED;
					} else {
						if (w->rb_link[1] == 0 || w->rb_link[1]->color == RB_BLACK) {
							node_t *y = w->rb_link[0];
							y->color = RB_BLACK;
							w->color = RB_RED;
							w->rb_link[0] = y->rb_link[1];
							y->rb_link[1] = w;
							w = pa[k - 1]->rb_link[1] = y;
						}
						w->color = pa[k - 1]->color;
						pa[k - 1]->color = RB_BLACK;
						w->rb_link[1]->color = RB_BLACK;
						pa[k - 1]->rb_link[1] = w->rb_link[0];
						w->rb_link[0] = pa[k - 1];
						pa[k - 2]->rb_link[da[k - 2]] = w;
						break;
					}
				} else {
					node_t *w = pa[k - 1]->rb_link[0];
					if (w->color == RB_RED) {
						w->color = RB_BLACK;
						pa[k - 1]->color = RB_RED;
						pa[k - 1]->rb_link[0] = w->rb_link[1];
						w->rb_link[1] = pa[k - 1];
						pa[k - 2]->rb_link[da[k - 2]] = w;
						pa[k] = pa[k - 1];
						da[k] = 1;
						pa[k - 1] = w;
						k++;
						w = pa[k - 1]->rb_link[0];
					}
					if ((w->rb_link[0] == 0 || w->rb_link[0]->color == RB_BLACK)
						&& (w->rb_link[1] == 0 || w->rb_link[1]->color == RB_BLACK))
					{
						w->color = RB_RED;
					} else {
						if (w->rb_link[0] == 0 || w->rb_link[0]->color == RB_BLACK) {
							node_t *y = w->rb_link[1];
							y->color = RB_BLACK;
							w->color = RB_RED;
							w->rb_link[1] = y->rb_link[0];
							y->rb_link[0] = w;
							w = pa[k - 1]->rb_link[0] = y;
						}
						w->color = pa[k - 1]->color;
						pa[k - 1]->color = RB_BLACK;
						w->rb_link[0]->color = RB_BLACK;
						pa[k - 1]->rb_link[0] = w->rb_link[1];
						w->rb_link[1] = pa[k - 1];
						pa[k - 2]->rb_link[da[k - 2]] = w;
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
