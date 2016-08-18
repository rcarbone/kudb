/* Author: attractivechaos */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include "runlib.h"

/* LINUX */

#ifdef __linux__
int run_get_static_sys_info(RunSysStatic *rss)
{
	FILE *fp;
	char buffer[64];
	unsigned flag = 0;

	rss->page_size = sysconf(_SC_PAGESIZE);
	fp = fopen("/proc/meminfo", "r");
	if (fp == 0) return RUN_ERR_WRONG_OS;
	while (fscanf(fp, "%s", buffer) > 0) {
		if (strstr(buffer, "MemTotal") == buffer) {
			fscanf(fp, "%lu", &(rss->mem_total));
			rss->mem_total *= 1024;
			flag |= 0x1;
		} else if (strstr(buffer, "SwapTotal") == buffer) {
			fscanf(fp, "%lu", &(rss->swap_total));
			rss->swap_total *= 1024;
			flag |= 0x2;
		}
	}
	fclose(fp);
	return (flag == 0x3)? 0 : RUN_ERR_MISSING_INFO;
}
int run_get_dynamic_sys_info(RunSysDyn *rsd)
{
	FILE *fp;
	size_t mem_buffer, mem_cache;
	char buffer[64];
	unsigned flag = 0;
	struct timeval tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);
	rsd->wall_clock = tp.tv_sec + tp.tv_usec * 1e-6;
	fp = fopen("/proc/meminfo", "r");
	if (fp == 0) return RUN_ERR_WRONG_OS;
	while (fscanf(fp, "%s", buffer) > 0) {
		if (strstr(buffer, "MemFree") == buffer) {
			fscanf(fp, "%lu", &(rsd->mem_free));
			flag |= 0x1;
		} else if (strstr(buffer, "Buffers") == buffer) {
			fscanf(fp, "%lu", &mem_buffer);
			flag |= 0x2;
		} else if (strstr(buffer, "Cached") == buffer) {
			fscanf(fp, "%lu", &mem_cache);
			flag |= 0x4;
		}
	}
	rsd->mem_available = rsd->mem_free + mem_buffer + mem_cache;
	rsd->mem_free *= 1024;
	rsd->mem_available *= 1024;
	fclose(fp);
	return (flag == 0x7)? 0 : RUN_ERR_MISSING_INFO;
}
int run_get_dynamic_proc_info(pid_t pid, RunProcDyn *rpd)
{
	int c, n_spc;
	char str[64];
	FILE *fp;
	unsigned long tmp, tmp2;
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE);
	sprintf(str, "/proc/%u/stat", pid);
	fp = fopen(str, "r");
	if (fp == 0) return RUN_ERR_PROC_FINISHED;
	n_spc = 0;
	while ((c = fgetc(fp)) != EOF) {
		if (c == ' ') ++n_spc;
		if (n_spc == 13) break;
	}
	fscanf(fp, "%lu%lu", &tmp, &tmp2);
	rpd->utime = tmp / 100.0;
	rpd->stime = tmp2 / 100.0;
	++n_spc;
	while ((c = fgetc(fp)) != EOF) {
		if (c == ' ') ++n_spc;
		if (n_spc == 22) break;
	}
	fscanf(fp, "%lu%lu", &tmp, &tmp2);
	fclose(fp);
	rpd->vsize = tmp / 1024;
	rpd->rss = tmp2 * (page_size / 1024);
	rpd->rss *= 1024;
	rpd->vsize *= 1024;
	return 0;
}
#endif /* __linux */

/* MAC OS X */

#ifdef __APPLE__

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach_port.h>
#include <mach/mach_traps.h>
#include <mach/task_info.h>
#include <mach/thread_info.h>
#include <mach/thread_act.h>
#include <mach/vm_region.h>
#include <mach/vm_map.h>
#include <mach/task.h>
#include <mach/shared_memory_server.h>

typedef struct vmtotal vmtotal_t;

int run_get_static_sys_info(RunSysStatic *rss)
{
	int mib[4];
	size_t len;
	unsigned value;
	/* physical memory */
	mib[0] = CTL_HW; mib[1] = HW_PHYSMEM;
	len = sizeof(unsigned);
	sysctl(mib, 2, &value, &len, 0, 0);
	rss->mem_total = value;
	/* page size */
	mib[1] = HW_PAGESIZE;
	len = sizeof(unsigned);
	sysctl(mib, 2, &value, &len, 0, 0);
	rss->page_size = value;
	/* virtual memory */
	rss->swap_total = 0; /* NOT implemented */
	return 0;
}
int run_get_dynamic_sys_info(RunSysDyn *rsd)
{
	int mib[4];
	size_t len;
	vm_size_t pagesize;
	vmtotal_t value;
	struct timeval tp;
	struct timezone tzp;
	vm_statistics_data_t vm_stat;
	mach_msg_type_number_t count;
	kern_return_t error;

	count = sizeof(vm_stat) / sizeof(natural_t);
	error = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stat, &count);
	assert(error == KERN_SUCCESS && count == sizeof(vm_stat) / sizeof(natural_t));
	host_page_size(mach_host_self(), &pagesize);
	rsd->mem_free = (size_t)vm_stat.free_count * pagesize;
	rsd->mem_available = (size_t)(vm_stat.free_count + vm_stat.inactive_count) * pagesize;
	gettimeofday(&tp, &tzp);
	rsd->wall_clock = tp.tv_sec + tp.tv_usec * 1e-6;
	mib[0] = CTL_VM; mib[1] = VM_METER;
	len = sizeof(vmtotal_t);
	sysctl(mib, 2, &value, &len, 0, 0);
	assert(len == sizeof(vmtotal_t));
	return 0;
}
/* On Mac OS X, the only way to get enough information is to become root. Pretty frustrating!*/
int run_get_dynamic_proc_info(pid_t pid, RunProcDyn *rpd)
{
	task_t task;
	kern_return_t error;
	mach_msg_type_number_t count;
	thread_array_t thread_table;
	thread_basic_info_t thi;
	thread_basic_info_data_t thi_data;
	unsigned table_size;
	struct task_basic_info ti;

	error = task_for_pid(mach_task_self(), pid, &task);
	if (error != KERN_SUCCESS) {
		/* fprintf(stderr, "++ Probably you have to set suid or become root.\n"); */
		rpd->rss = rpd->vsize = 0;
		rpd->utime = rpd->stime = 0;
		return 0;
	}
	count = TASK_BASIC_INFO_COUNT;
	error = task_info(task, TASK_BASIC_INFO, (task_info_t)&ti, &count);
	assert(error == KERN_SUCCESS);
	{ /* adapted from ps/tasks.c */
		vm_region_basic_info_data_64_t b_info;
        vm_address_t address = GLOBAL_SHARED_TEXT_SEGMENT;
        vm_size_t size;
        mach_port_t object_name;
        count = VM_REGION_BASIC_INFO_COUNT_64;
        error = vm_region_64(task, &address, &size, VM_REGION_BASIC_INFO,
							 (vm_region_info_t)&b_info, &count, &object_name);
		if (error == KERN_SUCCESS) {
			if (b_info.reserved && size == (SHARED_TEXT_REGION_SIZE) &&
                ti.virtual_size > (SHARED_TEXT_REGION_SIZE + SHARED_DATA_REGION_SIZE))
			{
				ti.virtual_size -= (SHARED_TEXT_REGION_SIZE + SHARED_DATA_REGION_SIZE);
			}
		}
		rpd->rss = ti.resident_size;
		rpd->vsize = ti.virtual_size;
	}
	{ /* calculate CPU times, adapted from top/libtop.c */
		unsigned i;
		rpd->utime = ti.user_time.seconds + ti.user_time.microseconds * 1e-6;
		rpd->stime = ti.system_time.seconds + ti.system_time.microseconds * 1e-6;
		error = task_threads(task, &thread_table, &table_size);
		assert(error == KERN_SUCCESS);
		thi = &thi_data;
		for (i = 0; i != table_size; ++i) {
			count = THREAD_BASIC_INFO_COUNT;
			error = thread_info(thread_table[i], THREAD_BASIC_INFO, (thread_info_t)thi, &count);
			assert(error == KERN_SUCCESS);
			if ((thi->flags & TH_FLAGS_IDLE) == 0) {
				rpd->utime += thi->user_time.seconds + thi->user_time.microseconds * 1e-6;
				rpd->stime += thi->system_time.seconds + thi->system_time.microseconds * 1e-6;
			}
			if (task != mach_task_self()) {
				error = mach_port_deallocate(mach_task_self(), thread_table[i]);
				assert(error == KERN_SUCCESS);
			}
		}
		error = vm_deallocate(mach_task_self(), (vm_offset_t)thread_table, table_size * sizeof(thread_array_t));
		assert(error == KERN_SUCCESS);
	}
	mach_port_deallocate(mach_task_self(), task);
	return 0;
}

#endif /* __APPLE__ */
