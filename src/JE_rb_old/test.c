#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "benchmark.h"
#include "rb.h"

typedef struct intnode_s intnode_t;
struct intnode_s {
	rb_node(intnode_t) link;
	unsigned key;
	int val;
};
typedef rb_tree(intnode_t) inttree_t;
static inline int int_cmp(intnode_t *a, intnode_t *b) { return (a->key > b->key) - (a->key < b->key); }

typedef struct strnode_s strnode_t;
struct strnode_s {
	rb_node(strnode_t) link;
	char *key;
	int val;
};
typedef rb_tree(strnode_t) strtree_t;
static inline int str_cmp(strnode_t *a, strnode_t *b) { return strcmp(a->key, b->key); }

int test_int(int N, const unsigned *data)
{
	inttree_t h;
	intnode_t d, *node;
	int i, n = 0;
	rb_tree_new(&h, link);
	for (i = 0; i < N; ++i) {
		rb_node_new(&h, &d, link);
		d.key = data[i];
		rb_search(&h, &d, int_cmp, link, node);
		if (node != rb_tree_nil(&h)) {
			rb_remove(&h, node, intnode_t, link);
			free(node);
			--n;
		} else {
			node = (intnode_t*)malloc(sizeof(intnode_t));
			rb_node_new(&h, node, link);
			node->key = data[i]; node->val = i;
			rb_insert(&h, node, int_cmp, intnode_t, link);
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
	rb_tree_new(&h, link);
	for (i = 0; i < N; ++i) {
		rb_node_new(&h, &d, link);
		d.key = data[i];
		rb_search(&h, &d, str_cmp, link, node);
		if (node != rb_tree_nil(&h)) {
			rb_remove(&h, node, strnode_t, link);
			free(node);
			--n;
		} else {
			node = (strnode_t*)malloc(sizeof(strnode_t));
			rb_node_new(&h, node, link);
			node->key = data[i]; node->val = i;
			rb_insert(&h, node, str_cmp, strnode_t, link);
			++n;
		}
	}
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
