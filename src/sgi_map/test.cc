#include "benchmark.h"
#include <utility>
#include <map>
#include <string.h>

using namespace std;

struct ltstr {
	inline bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) < 0;
    }
};

typedef map<unsigned, int> inthash;
typedef map<const char*, int, ltstr> strhash;

int test_int(int N, const unsigned *data)
{
	int i, ret;
	inthash *h = new inthash;
	for (i = 0; i < N; ++i) {
		pair<inthash::iterator, bool> p = h->insert(pair<unsigned, int>(data[i], i));
		if (p.second == false) h->erase(p.first);
	}
	ret = h->size();
	delete h;
	return ret;
}

int test_str(int N, char * const *data)
{
	int i, ret;
	strhash *h = new strhash;
	for (i = 0; i < N; ++i) {
		pair<strhash::iterator, bool> p = h->insert(pair<const char*, int>(data[i], i));
		if (p.second == false) h->erase(p.first);
	}
	ret = h->size();
	delete h;
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
