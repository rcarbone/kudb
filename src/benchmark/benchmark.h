#ifndef UDB_BENCHMARK_H
#define UDB_BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

	int udb_benchmark(int argc, char *argv[], int (*func_int)(int, const unsigned*), int (*func)(int, char *const*));

#ifdef __cplusplus
}
#endif

#endif
