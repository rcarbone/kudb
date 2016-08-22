#include <stdlib.h>
#include "benchmark.h"
#include "uthash.h"

typedef struct {
	unsigned key;
	int val;
	UT_hash_handle hh;
} intcell_t;

typedef struct {
	char *key;
	int val;
	UT_hash_handle hh;
} strcell_t;

int test_int(int N, const unsigned *data)
{
	int i, n = 0;
	intcell_t *h = 0, *r;
	for (i = 0; i < N; ++i) {
		HASH_FIND_INT(h, &data[i], r);
		if (r == 0) {
			r = (intcell_t*)malloc(sizeof(intcell_t));
			r->key = data[i]; r->val = i;
			HASH_ADD_INT(h, key, r);
			++n;
		} else {
			HASH_DEL(h, r);
			free(r);
			--n;
		}
	}
	return n;
}
int test_str(int N, char * const *data)
{
	int i, n = 0;
	strcell_t *h = 0, *r;
	for (i = 0; i < N; ++i) {
		HASH_FIND_STR(h, data[i], r);
		if (r == 0) {
			r = (strcell_t*)malloc(sizeof(strcell_t));
			r->key = data[i]; r->val = i;
			HASH_ADD_KEYPTR(hh, h, r->key, strlen(r->key), r);
			++n;
		} else {
			HASH_DEL(h, r);
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
