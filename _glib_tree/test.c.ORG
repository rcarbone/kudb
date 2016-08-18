#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"

#include <glib.h>

static inline int intcmp(const void *a, const void *b)
{
	return (*(unsigned*)a > *(unsigned*)b) - (*(unsigned*)a < *(unsigned*)b);
}

int test_int(int N, const unsigned *data)
{
	int i, *p, n = 0;
	GTree *h;
	h = g_tree_new_full(intcmp, 0, 0, free);
	for (i = 0; i < N; ++i) {
		if (!g_tree_remove(h, &data[i])) {
			p = (int*)malloc(sizeof(int));
			*p = i;
			g_tree_insert(h, &data[i], p);
			++n;
		} else --n;
	}
	g_tree_destroy(h);
	return n;
}
int test_str(int N, char * const *data)
{
	int i, *p, n = 0;
	GTree *h;
	h = g_tree_new_full(strcmp, 0, 0, free);
	for (i = 0; i < N; ++i) {
		if (!g_tree_remove(h, data[i])) {
			p = (int*)malloc(sizeof(int));
			*p = i;
			g_tree_insert(h, data[i], p);
			++n;
		} else --n;
	}
	g_tree_destroy(h);
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
