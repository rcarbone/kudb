#include "benchmark.h"

#include "khash.h"
KHASH_MAP_INIT_INT(int, int)
KHASH_MAP_INIT_STR(str, int)

int test_int(int N, const unsigned *data)
{
	int i, ret;
	khash_t(int) *h;
	unsigned k;
	h = kh_init(int);
	for (i = 0; i < N; ++i) {
		k = kh_put(int, h, data[i], &ret);
		if (!ret) kh_del(int, h, k);
		else kh_value(h, k) = i;
	}
	ret = (int)kh_size(h);
	kh_destroy(int, h);
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, ret;
	khash_t(str) *h;
	unsigned k;
	h = kh_init(str);
	for (i = 0; i < N; ++i) {
		k = kh_put(str, h, data[i], &ret);
		if (!ret) kh_del(str, h, k);
		else kh_value(h, k) = i;
	}
	ret = (int)kh_size(h);
	kh_destroy(str, h);
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
