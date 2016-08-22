#include <string.h>
#include "benchmark.h"
#include "rdestl/hash_map.h"

using namespace rde;

struct eqstr {
	inline bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) == 0;
    }
};
struct hasher {
	inline rde::hash_value_t operator()(const char* s) const {
		rde::hash_value_t hash(0);
		char c;
		while ((c = *s++) != 0)
			hash = (hash << 5) + hash + c;
		return hash;
	}
};
 
typedef rde::hash_map<unsigned, int> inthash;
typedef rde::hash_map<const char*, int, hasher, 6, eqstr> strhash;

int test_int(int N, const unsigned *data)
{
	int i, ret;
	inthash *h = new inthash;
	for (i = 0; i < N; ++i) {
		rde::pair<inthash::iterator, bool> p = h->insert(rde::pair<unsigned, int>(data[i], i));
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
		rde::pair<strhash::iterator, bool> p = h->insert(rde::pair<const char*, int>(data[i], i));
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
