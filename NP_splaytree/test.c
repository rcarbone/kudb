#include <stdlib.h>
#include <string.h>
#include "benchmark.h"
#include "tree.h"

typedef struct intnode_s intnode_t;
struct intnode_s {
	SPLAY_ENTRY(intnode_s) link;
	unsigned key;
	int val;
};
typedef struct inttree_s inttree_t;
SPLAY_HEAD(inttree_s, intnode_s);
static int int_cmp(intnode_t *a, intnode_t *b) { return (a->key > b->key) - (a->key < b->key); }
SPLAY_PROTOTYPE(inttree_s, intnode_s, link, int_cmp);
SPLAY_GENERATE(inttree_s, intnode_s, link, int_cmp);

typedef struct strnode_s strnode_t;
struct strnode_s {
	SPLAY_ENTRY(strnode_s) link;
	char *key;
	int val;
};
typedef struct strtree_s strtree_t;
SPLAY_HEAD(strtree_s, strnode_s);
static int str_cmp(strnode_t *a, strnode_t *b) { return strcmp(a->key, b->key); }
SPLAY_PROTOTYPE(strtree_s, strnode_s, link, str_cmp);
SPLAY_GENERATE(strtree_s, strnode_s, link, str_cmp);

int test_int(int N, const unsigned *data)
{
	inttree_t h;
	intnode_t d, *node;
	int i, n = 0;
	SPLAY_INIT(&h);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		node = SPLAY_FIND(inttree_s, &h, &d);
		if (node) {
			SPLAY_REMOVE(inttree_s, &h, node);
			free(node);
			--n;
		} else {
			node = (intnode_t*)malloc(sizeof(intnode_t));
			node->key = data[i]; node->val = i;
			SPLAY_INSERT(inttree_s, &h, node);
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
	SPLAY_INIT(&h);
	for (i = 0; i < N; ++i) {
		d.key = data[i];
		node = SPLAY_FIND(strtree_s, &h, &d);
		if (node) {
			SPLAY_REMOVE(strtree_s, &h, node);
			free(node);
			--n;
		} else {
			node = (strnode_t*)malloc(sizeof(strnode_t));
			node->key = data[i]; node->val = i;
			SPLAY_INSERT(strtree_s, &h, node);
			++n;
		}
	}
	return n;
	return 0;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
