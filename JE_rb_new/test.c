#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
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
	rb_new(intnode_t, link, &h);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		rb_search(intnode_t, link, int_cmp, &h, &d, node);
		if (node) {
			rb_remove(intnode_t, link, int_cmp, &h, node);
			free(node);
			--n;
		} else {
			node = (intnode_t*)malloc(sizeof(intnode_t));
			node->key = data[i]; node->val = i;
			rb_insert(intnode_t, link, int_cmp, &h, node);
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
	rb_new(strnode_t, link, &h);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		rb_search(strnode_t, link, str_cmp, &h, &d, node);
		if (node) {
			rb_remove(strnode_t, link, str_cmp, &h, node);
			free(node);
			--n;
		} else {
			node = (strnode_t*)malloc(sizeof(strnode_t));
			node->key = data[i]; node->val = i;
			rb_insert(strnode_t, link, str_cmp, &h, node);
			++n;
		}
	}
	return n;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
