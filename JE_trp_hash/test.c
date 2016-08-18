#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include "benchmark.h"
#include "trp.h"

typedef struct intnode_s intnode_t;
struct intnode_s {
	trp_node(intnode_t) link;
	unsigned key;
	int val;
};
static int int_cmp(intnode_t *a, intnode_t *b) { return (a->key > b->key) - (a->key < b->key); }
typedef trp(intnode_t) inttree_t;
trp_gen(static, inttree_, inttree_t, intnode_t, link, int_cmp);

typedef struct strnode_s strnode_t;
struct strnode_s {
	trp_node(strnode_t) link;
	char *key;
	int val;
};
typedef trp(strnode_t) strtree_t;
static int str_cmp(strnode_t *a, strnode_t *b) { return strcmp(a->key, b->key); }
trp_gen(static, strtree_, strtree_t, strnode_t, link, str_cmp);

int test_int(int N, const unsigned *data)
{
	inttree_t h;
	intnode_t d, *node;
	int i, n = 0;
	inttree_new(&h, 11);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		node = inttree_search(&h, &d);
		if (node) {
			inttree_remove(&h, node);
			free(node);
			--n;
		} else {
			node = (intnode_t*)malloc(sizeof(intnode_t));
			node->key = data[i]; node->val = i;
			inttree_insert(&h, node);
			++n;
		}
	}
	return n;
}
int test_str(int N, char * const *data)
{
	strtree_t h;
	strnode_t d, *node;
	int i, n = 0;
	strtree_new(&h, 11);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		node = strtree_search(&h, &d);
		if (node) {
			strtree_remove(&h, node);
			free(node);
			--n;
		} else {
			node = (strnode_t*)malloc(sizeof(strnode_t));
			node->key = data[i]; node->val = i;
			strtree_insert(&h, node);
			++n;
		}
	}
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
