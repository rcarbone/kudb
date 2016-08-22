#include "benchmark.h"
#include <QtCore/QMap>
#include <stdlib.h>
#include <string.h>

//#define SIMPLE_STR

struct mystr_t {
	const char *s;
};
inline bool operator < (const mystr_t &a, const mystr_t &b) { return strcmp(a.s, b.s) < 0; }

typedef QMap<unsigned, int> inthash;
typedef QMap<mystr_t, int> strhash;

int test_int(int N, const unsigned *data)
{
	int i, ret;
	inthash *h = new inthash;
	for (i = 0; i < N; ++i) {
		if (h->remove(data[i]) == 0) h->insert(data[i], i);
	}
	ret = h->count();
	delete h;
	return ret;
}

int test_str(int N, char *const *data)
{
	int i, ret;
	strhash *h = new strhash;
	mystr_t p;
	for (i = 0; i < N; ++i) {
		p.s = data[i];
		if (h->remove(p) == 0) h->insert(p, i);
	}
	ret = h->count();
	delete h;
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
