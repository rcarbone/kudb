#include "benchmark.h"
#include <utility>
#include <set>

using namespace std;

typedef struct {
	unsigned key;
	int value;
} intmap_t;

typedef struct {
	char *key;
	int value;
} strmap_t;

struct ltint {
	inline bool operator()(void *_a, void *_b) const {
		intmap_t *a = (intmap_t*)_a;
		intmap_t *b = (intmap_t*)_b;
		return a->key < b->key;
    }
};

struct ltstr {
	inline bool operator()(void *a, void *b) {
		return strcmp(((strmap_t*)a)->key, ((strmap_t*)b)->key);
	}
};

typedef set<void*, ltint> inthash;
typedef set<void*, ltstr> strhash;

int test_int(int N, const unsigned *data)
{
	int i, ret;
	intmap_t *d;
	inthash::iterator iter;
	inthash *h = new inthash;
	for (i = 0; i < N; ++i) {
		d = (intmap_t*)malloc(sizeof(intmap_t));
		d->key = data[i]; d->value = i;
		iter = h->find(d);
		if (iter == h->end()) h->insert(d);
		else {
			h->erase(d);
			free(*iter); free(d);
		}
	}
	ret = h->size();
	delete h;
	return ret;
}

int test_str(int N, char * const *data)
{
	return 0;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
