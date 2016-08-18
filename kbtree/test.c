#include <stdio.h>
#include "benchmark.h"

typedef struct {
	unsigned key;
	int value;
} intmap_t;

typedef struct {
	char *key;
	int value;
} strmap_t;

#define __intcmp(a, b) (((a).key > (b).key) - ((a).key < (b).key))
#define __strcmp(a, b) strcmp((a).key, (b).key)

#include "kbtree.h"
KBTREE_INIT(int, intmap_t, __intcmp)
KBTREE_INIT(str, strmap_t, __strcmp)

int test_int(int N, const unsigned *data)
{
	int i, ret;
	intmap_t d;
	kbtree_t(int) *h;
	h = kb_init(int, KB_DEFAULT_SIZE);
	for (i = 0; i < N; ++i) {
		d.key = data[i]; d.value = i;
		if (kb_getp(int, h, &d) == 0) kb_putp(int, h, &d);
		else kb_delp(int, h, &d);
	}
	ret = (int)kb_size(h);
	if (0) {
		unsigned xor = 0;
#define traf(k) (xor ^= (k)->value)
		__kb_traverse(intmap_t, h, traf);
		printf("%u\n", xor);
	}
	kb_destroy(int, h);
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, ret;
	strmap_t d;
	kbtree_t(str) *h;
	h = kb_init(str, KB_DEFAULT_SIZE);
	for (i = 0; i < N; ++i) {
		d.key = data[i]; d.value = i;
		if (kb_getp(str, h, &d) == 0) kb_putp(str, h, &d);
		else kb_delp(str, h, &d);
	}
	ret = (int)kb_size(h);
	if (0) {
		unsigned xor = 0;
		__kb_traverse(strmap_t, h, traf);
		printf("%u\n", xor);
	}
	kb_destroy(str, h);
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
