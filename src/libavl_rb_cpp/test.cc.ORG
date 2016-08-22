#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"

#include "rb.hh"

struct int_cmp {
	inline int operator () (unsigned a, unsigned b) {
		return (a > b) - (a < b);
	}
};

struct str_cmp {
	inline int operator () (const char *a, const char *b) {
		return strcmp(a, b);
	}
};

typedef rbmap_t<unsigned, int, int_cmp> intmap_t;
typedef rbmap_t<const char*, int, str_cmp> strmap_t;

int test_int(int N, const unsigned *data)
{
	int i, ret;
	intmap_t *h;
	intmap_t::node_t *p;
	printf("[test_int] node size: %lu\n", sizeof(intmap_t::node_t));
	h = new intmap_t;
	for (i = 0; i < N; ++i) {
		p = h->find(data[i]);
		if (p == 0) {
			p = h->insert(data[i], &ret);
			p->value = i;
		} else {
			p = h->erase(data[i]);
			free(p);
		}
	}
	ret = h->count;
	delete h;
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, ret;
	strmap_t *h;
	strmap_t::node_t *p;
	printf("[test_int] node size: %lu\n", sizeof(strmap_t::node_t));
	h = new strmap_t;
	for (i = 0; i < N; ++i) {
		p = h->find(data[i]);
		if (p == 0) {
			p = h->insert(data[i], &ret);
			p->value = i;
		} else {
			p = h->erase(data[i]);
			free(p);
		}
	}
	ret = h->count;
	delete h;
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
