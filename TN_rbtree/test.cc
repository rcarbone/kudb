#include <string.h>
#include <stdio.h>
#include "benchmark.h"
#include "rb_tree.hh"

typedef struct {
	unsigned key;
	int value;
} intmap_t;

typedef struct {
	char *key;
	int value;
} strmap_t;

struct int_cmp {
	inline int operator () (const intmap_t &a, const intmap_t &b) {
		return (a.key > b.key) - (a.key < b.key);
	}
};

struct str_cmp {
	inline int operator () (const strmap_t &a, const strmap_t &b) {
		return strcmp(a.key, b.key);
	}
};

typedef rb_tree<intmap_t, int_cmp> inttree_t;
typedef rb_tree<strmap_t, str_cmp> strtree_t;

int test_int(int N, const unsigned *data)
{
	int i, n = 0;
	intmap_t d;
	inttree_t *h = new inttree_t;
	printf("[test_int] node size: %lu\n", sizeof(inttree_t::node_t));
	for (i = 0; i < N; ++i) {
		d.key = data[i]; d.value = i;
		if (h->search(d) == 0) { h->insert(d); ++n; }
		else { h->erase(d); --n; }
	}
	delete h;
	return n;
}
int test_str(int N, char * const *data)
{
	int i, n = 0;
	strmap_t d;
	strtree_t *h = new strtree_t;
	printf("[test_str] node size: %lu\n", sizeof(strtree_t::node_t));
	for (i = 0; i < N; ++i) {
		d.key = data[i]; d.value = i;
		if (h->search(d) == 0) { h->insert(d); ++n; }
		else { h->erase(d); --n; }
	}
	delete h;
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
