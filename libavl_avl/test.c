#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"
#include "avl.h"

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
	int i;
	intmap_t *d, *r;
	struct avl_table *h;
	h = avl_create(int_cmp, 0, 0);
	for (i = 0; i < N; ++i) {
		d = (intmap_t*)malloc(sizeof(intmap_t));
		d->key = data[i]; d->value = i;
		if (avl_find(h, d) == 0) avl_insert(h, d);
		else {
			r = avl_delete(h, d);
			free(d); free(r);
		}
	}
	return avl_count(h);
}
int test_str(int N, char * const *data)
{
	int i;
	strmap_t *d, *r;
	struct avl_table *h;
	h = avl_create(str_cmp, 0, 0);
	for (i = 0; i < N; ++i) {
		d = (strmap_t*)malloc(sizeof(strmap_t));
		d->key = data[i]; d->value = i;
		if (avl_find(h, d) == 0) avl_insert(h, d);
		else {
			r = avl_delete(h, d);
			free(d); free(r);
		}
	}
	return avl_count(h);
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
