#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"
#include "prb.h"

typedef struct {
	unsigned key;
	int value;
} intmap_t;

typedef struct {
	char *key;
	int value;
} strmap_t;

int int_cmp(const void *_a, const void *_b, void *p)
{
	intmap_t *a = (intmap_t*)_a;
	intmap_t *b = (intmap_t*)_b;
	return (a->key > b->key) - (a->key < b->key);
}
int str_cmp(const void *a, const void *b, void *p)
{
	return strcmp(((strmap_t*)a)->key, ((strmap_t*)b)->key);
}

int test_int(int N, const unsigned *data)
{
	int i, n = 0;
	intmap_t *d, *r;
	struct prb_table *h;
	h = prb_create(int_cmp, 0, 0);
	for (i = 0; i < N; ++i) {
		d = (intmap_t*)malloc(sizeof(intmap_t));
		d->key = data[i]; d->value = i;
		if (prb_find(h, d) == 0) {
			prb_insert(h, d);
			++n;
		} else {
			r = prb_delete(h, d);
			free(d); free(r);
			--n;
		}
	}
	return n;
}
int test_str(int N, char * const *data)
{
	int i, n = 0;
	strmap_t *d, *r;
	struct prb_table *h;
	h = prb_create(str_cmp, 0, 0);
	for (i = 0; i < N; ++i) {
		d = (strmap_t*)malloc(sizeof(strmap_t));
		d->key = data[i]; d->value = i;
		if (prb_find(h, d) == 0) {
			prb_insert(h, d);
			++n;
		} else {
			r = prb_delete(h, d);
			free(d); free(r);
			--n;
		}
	}
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
