#include <stdio.h>
#include <stdlib.h>
#include "benchmark.h"

typedef struct intmap_t {
	unsigned key;
	int value;
	char color;
	struct intmap_t *l, *r;
} intmap_t;

typedef struct strmap_t {
	char *key;
	int value;
	char color;
	struct strmap_t *l, *r;
} strmap_t;

#define __intcmp(a, b) (((a)->key > (b)->key) - ((a)->key < (b)->key))
#define __strcmp(a, b) strcmp((a)->key, (b)->key)

#include "sglib.h"
SGLIB_DEFINE_RBTREE_PROTOTYPES(intmap_t, l, r, color, __intcmp);
SGLIB_DEFINE_RBTREE_FUNCTIONS(intmap_t, l, r, color, __intcmp);
SGLIB_DEFINE_RBTREE_PROTOTYPES(strmap_t, l, r, color, __strcmp);
SGLIB_DEFINE_RBTREE_FUNCTIONS(strmap_t, l, r, color, __strcmp);

int test_int(int N, const unsigned *data)
{
	int i, n = 0;
	intmap_t d, *h, *r;
	h = 0;
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		if ((r = sglib_intmap_t_find_member(h, &d)) == 0) {
			r = (intmap_t*)malloc(sizeof(intmap_t));
			r->key = data[i]; r->value = i;
			sglib_intmap_t_add(&h, r);
			++n;
		} else {
			sglib_intmap_t_delete(&h, r);
			free(r);
			--n;
		}
	}
	return n;
}
int test_int_alt(int N, const unsigned *data) // this is slower
{
	int i, n = 0;
	intmap_t *h, *r, *t;
	h = 0;
	for (i = 0; i < N; ++i) {
		r = (intmap_t*)malloc(sizeof(intmap_t));
		r->key = data[i]; r->value = i;
		if (!sglib_intmap_t_add_if_not_member(&h, r, &t)) {
			sglib_intmap_t_delete(&h, t);
			free(r); free(t);
			--n;
		} else ++n;
	}
	return n;
}
int test_str(int N, char * const *data)
{
	int i, n = 0;
	strmap_t d, *h, *r;
	h = 0;
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		if ((r = sglib_strmap_t_find_member(h, &d)) == 0) {
			r = (strmap_t*)malloc(sizeof(strmap_t));
			r->key = data[i]; r->value = i;
			sglib_strmap_t_add(&h, r);
			++n;
		} else {
			sglib_strmap_t_delete(&h, r);
			free(r);
			--n;
		}
	}
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
