#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark.h"

#include <glib.h>

/*
int test_int(int N, const unsigned *data)
{
	int i, val = 11, ret;
	GHashTable *h;
	h = g_hash_table_new(g_int_hash, g_int_equal);
	for (i = 0; i < N; ++i) {
		if (!g_hash_table_steal(h, &data[i])) {
			g_hash_table_insert(h, &data[i], &val);
		}
	}
	ret = g_hash_table_size(h);
	g_hash_table_destroy(h);
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, val = 11, ret;
	GHashTable *h;
	h = g_hash_table_new(g_str_hash, g_str_equal);
	for (i = 0; i < N; ++i) {
		if (!g_hash_table_steal(h, data[i])) {
			g_hash_table_insert(h, data[i], &val);
		}
	}
	ret = g_hash_table_size(h);
	g_hash_table_destroy(h);
	return ret;
}
*/
int test_int(int N, const unsigned *data)
{
	int i, ret, *p;
	GHashTable *h;
	h = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free);
	for (i = 0; i < N; ++i) {
		if (!g_hash_table_remove(h, &data[i])) {
			p = (int*)malloc(sizeof(int));
			*p = i;
			g_hash_table_insert(h, (unsigned *) &data[i], p);
		}
	}
	ret = g_hash_table_size(h);
	g_hash_table_destroy(h);
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, ret, *p;
	GHashTable *h;
	h = g_hash_table_new_full(g_str_hash, g_str_equal, 0, free);
	for (i = 0; i < N; ++i) {
		if (!g_hash_table_remove(h, data[i])) {
			p = (int*)malloc(sizeof(int));
			*p = i;
			g_hash_table_insert(h, data[i], p);
		}
	}
	ret = g_hash_table_size(h);
	g_hash_table_destroy(h);
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
