#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "runlib.h"

static int data_size = 5000000;
static unsigned *int_data;
static char **str_data;

void udb_init_data()
{
	int i;
	char buf[256];
	printf("[benchmark] generating data... ");
	srand48(11);
	int_data = (unsigned*)calloc(data_size, sizeof(unsigned));
	str_data = (char**)calloc(data_size, sizeof(char*));
	for (i = 0; i < data_size; ++i) {
		int_data[i] = (unsigned)(data_size * drand48() / 4) * 271828183u;
		sprintf(buf, "%x", int_data[i]);
		str_data[i] = strdup(buf);
	}
	printf("done!\n");
}
void udb_destroy_data()
{
	int i;
	for (i = 0; i < data_size; ++i) free(str_data[i]);
	free(str_data); free(int_data);
}
static void udb_timing_int(int (*func)(int, const unsigned*))
{
	RunProcDyn rpd0, rpd1;
	double ut, st;
	int ret;
	pid_t pid = getpid();
	run_get_dynamic_proc_info(pid, &rpd0);
	ret = func(data_size, int_data);
	run_get_dynamic_proc_info(pid, &rpd1);
	printf("[int data] # elements: %d\n", ret);
	ut = rpd1.utime - rpd0.utime;
	st = rpd1.stime - rpd0.stime;
	printf("[int data] CPU time: %.3lf (= %.3lf + %.3lf)\n", ut + st, ut, st);
}
static void udb_timing_str(int (*func)(int, char *const*))
{
	RunProcDyn rpd0, rpd1;
	double ut, st;
	int ret;
	pid_t pid = getpid();
	run_get_dynamic_proc_info(pid, &rpd0);
	ret = func(data_size, str_data);
	run_get_dynamic_proc_info(pid, &rpd1);
	printf("[str data] # elements: %d\n", ret);
	ut = rpd1.utime - rpd0.utime;
	st = rpd1.stime - rpd0.stime;
	printf("[str data] CPU time: %.3lf (= %.3lf + %.3lf)\n", ut + st, ut, st);
}
int udb_benchmark(int argc, char *argv[], int (*func_int)(int, const unsigned*), int (*func_str)(int, char*const*))
{
	int c, flag = 0;
	size_t init_rss;
	RunProcDyn rpd;
	pid_t pid = getpid();
	while ((c = getopt(argc, argv, "isn:")) >= 0) {
		switch (c) {
		case 'i': flag |= 1; break;
		case 's': flag |= 2; break;
		case 'n': data_size = atoi(optarg); break;
		}	
	}
	if (flag == 0) {
		fprintf(stdout, "Usage: %s [-is] [-n %d]\n", argv[0], data_size);
		return 1;
	}
	udb_init_data();
	run_get_dynamic_proc_info(pid, &rpd);
	init_rss = rpd.rss;
	printf("[benchmark] initial rss: %.3lf kB\n", rpd.rss/1024.0);
	if (flag & 1) udb_timing_int(func_int);
	if (flag & 2) udb_timing_str(func_str);
	run_get_dynamic_proc_info(pid, &rpd);
	printf("[benchmark] rss diff: %.3lf kB\n", (rpd.rss - init_rss) / 1024.0);
	udb_destroy_data();
	printf("[benchmark] finished!\n\n");
	return 0;
}
