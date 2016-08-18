#include "benchmark.h"

#define STB_DEFINE
#define AC_REVISION
#include "stb.h"

int test_int(int N, const unsigned *data)
{
	int i, ret, *x, n = 0;
	stb_idict *h;
	h = stb_idict_create();
	for (i = 0; i < N; ++i) {
		ret = stb_idict_set(h, data[i], i);
		if (!ret) {
			--n;
			stb_idict_remove(h, data[i], x);
		} else ++n;
	}
	ret = n;
	stb_idict_destroy(h);
	return ret;
}
int test_str(int N, char * const *data)
{
	int i, ret, *x, n = 0;
	stb_sdict *h;
	h = stb_sdict_create();
	for (i = 0; i < N; ++i) {
		ret = stb_sdict_set(h, data[i], 0);
		if (!ret) {
			--n;
			stb_sdict_remove(h, data[i], x);
		} else ++n;
	}
	ret = n;
	stb_sdict_destroy(h);
	return ret;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}
