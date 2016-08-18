#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"
#include "btree.h"

typedef struct {
	unsigned key;
	int value;
} intmap_t;

typedef struct {
	char *key;
	int value;
} strmap_t;

int int_cmp(void *_a, void *_b)
{
	intmap_t *a = (intmap_t*)_a;
	intmap_t *b = (intmap_t*)_b;
	return (a->key > b->key) - (a->key < b->key);
}
int str_cmp(void *a, void *b)
{
	return strcmp(((strmap_t*)a)->key, ((strmap_t*)b)->key);
}

int test_int(int N, const unsigned *data)
{
	int i, n = 0;
	intmap_t d, *r;
	struct btree *h;
	h = bt_create(int_cmp, BT_SIZEDEF);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		if ((r = bt_find(h, &d)) == 0) {
			r = (intmap_t*)malloc(sizeof(intmap_t));
			r->key = data[i]; r->value = i;
			bt_insert(h, r);
			++n;
		} else {
			bt_delete(h, r);
			free(r);
			--n;
		}
	}
	return n;
}
int test_str(int N, char * const *data)
{
	int i, n = 0;
	strmap_t d, *r;
	struct btree *h;
	h = bt_create(str_cmp, BT_SIZEDEF);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		if ((r = bt_find(h, &d)) == 0) {
			r = (strmap_t*)malloc(sizeof(strmap_t));
			r->key = data[i]; r->value = i;
			bt_insert(h, r);
			++n;
		} else {
			bt_delete(h, r);
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
