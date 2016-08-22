/* Author: attractivechaos */

/* last modified: 2008-0306 */

#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include "runlib.h"

typedef struct
{
	double cpu_limit;
	double mem_limit;
	int check_interval, interval;
	int timeout;
	size_t memoryout;
	FILE *log_fp;
	char *prefix;
	int out, err;
	int fd_out[2], fd_err[2];
} RunStruct;

typedef struct
{
	unsigned error;
	double utime, stime, rtime;
	size_t max_rss, max_vsize;
	int retval;
	double avg_rss, avg_vsize;
} RunResult;

/* declarations to provide consistent linkage */
extern char *optarg;
extern int optind;
extern int opterr;

int getopt_standard(int nargc, char * const *nargv, const char *ostr);

RunStruct *run_new_RunStruct()
{
	RunStruct *rs = (RunStruct*)malloc(sizeof(RunStruct));
	rs->interval = 10000;
	rs->check_interval = 1;
	rs->log_fp = stderr;
	rs->out = STDOUT_FILENO;
	rs->err = STDERR_FILENO;
	rs->timeout = 0;
	rs->memoryout = 0;
	rs->mem_limit = 0.05;
	rs->cpu_limit = 1.0;
	rs->prefix = 0;
	return rs;
}
void run_upd_RunStruct(RunStruct *rs)
{
	rs->log_fp = stderr;
	if (rs->prefix) {
		char *prefix = rs->prefix;
		char *tmp = (char*)malloc(strlen(prefix) + 5);
		pipe(rs->fd_out); pipe(rs->fd_err);
		strcpy(tmp, prefix); strcat(tmp, ".log");
		rs->log_fp = fopen(tmp, "w");
		strcpy(tmp, prefix); strcat(tmp, ".out");
		rs->out = open(tmp, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		strcpy(tmp, prefix); strcat(tmp, ".err");
		rs->err = open(tmp, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		free(tmp);
	}
}
void run_free_RunStruct(RunStruct *rs)
{
	/* rs->out, rs->err, rs->fd_out[2], rs->fd_err[2] will be closed elsewhere */
	if (rs->log_fp != stderr) fclose(rs->log_fp);
	free(rs->prefix);
	free(rs);
}
int run_fill_RunStruct(int argc, char *argv[], RunStruct *rs)
{
	int c;
	while ((c = getopt_standard(argc, argv, "c:m:p:T:M:")) >= 0) {
		switch (c) {
		case 'c': rs->cpu_limit = atof(optarg); break;
		case 'm': rs->mem_limit = atof(optarg); break;
		case 'T': rs->timeout = atoi(optarg); break;
		case 'M': rs->memoryout = atoi(optarg); break;
		case 'p': rs->prefix = strdup(optarg); break;
		}
	}
	return optind;
}

static void *write_out(void *arg)
{
	RunStruct *rs = (RunStruct*)arg;
	char *buf[1024];
	int len;
	close(rs->fd_out[1]);
	while ((len = read(rs->fd_out[0], buf, 10)) != 0)
		write(rs->out, buf, len);
	close(rs->fd_out[0]);
	close(rs->out);
	return 0;
}
static void *write_err(void *arg)
{
	RunStruct *rs = (RunStruct*)arg;
	char *buf[1024];
	int len;
	close(rs->fd_err[1]);
	while ((len = read(rs->fd_err[0], buf, 10)) != 0)
		write(rs->err, buf, len);
	close(rs->fd_err[0]);
	close(rs->err);
	return 0;
}

RunResult *runit(int argc, char *argv[], RunStruct *rs)
{
	pid_t pid;
	if ((pid = fork()) != 0) {
		struct tms time_used;
		int status, ret, check_time;
		int is_killed;
		double sum_rss, sum_vsize;
		double start_time;
		double last_time;
		RunSysStatic rss;
		RunSysDyn rsd;
		RunProcDyn rpd;
		RunResult *rr;
		pthread_t outid, errid;

		run_get_static_sys_info(&rss);
		run_get_dynamic_sys_info(&rsd);
		start_time = rsd.wall_clock;
		rr = (RunResult*)malloc(sizeof(RunResult));
		rr->max_rss = rr->max_vsize = 0;
		rr->error = 0;
		check_time = 1;
		sum_rss = sum_vsize = 0.0;
		last_time = 0.0;
		is_killed = 1;
		if (rs->prefix) {
			pthread_create(&outid, 0, write_out, rs);
			pthread_create(&errid, 0, write_err, rs);
		}
		while ((ret = waitpid(pid, &status, WNOHANG)) != pid) {
			run_get_dynamic_sys_info(&rsd);
			run_get_dynamic_proc_info(pid, &rpd);
			if (rpd.rss > rr->max_rss) rr->max_rss = rpd.rss;
			if (rpd.vsize > rr->max_vsize) rr->max_vsize = rpd.vsize;
			sum_rss += rpd.rss * (rpd.utime + rpd.stime - last_time);
			sum_vsize += rpd.vsize * (rpd.utime + rpd.stime - last_time);
			last_time = rpd.utime + rpd.stime;
			if (rs->memoryout && rpd.rss > rs->memoryout) {
				kill(pid, SIGKILL);
				fprintf(rs->log_fp, "++ memory out!\n");
				goto retstat;
			}
			if (rs->timeout && rpd.utime + rpd.stime > rs->timeout) {
				kill(pid, SIGKILL);
				fprintf(rs->log_fp, "++ time out!\n");
				goto retstat;
			}
			if ((double)rsd.mem_available/rss.mem_total < rs->mem_limit
					&& (double)rpd.rss/rss.mem_total > rs->mem_limit)
			{
				kill(pid, SIGKILL);
				fprintf(rs->log_fp, "++ memory insufficient: %lu/%lu vs %lu\n", rsd.mem_free, rsd.mem_available, rss.mem_total);
				goto retstat;
			}
			if ((int)((rpd.utime + rpd.stime) / rs->check_interval + 0.5) == check_time) {
				if ((rpd.utime + rpd.stime)/(rsd.wall_clock - start_time) > rs->cpu_limit) {
					double stop_time = rpd.utime + rpd.stime - (rsd.wall_clock - start_time) * rs->cpu_limit;
					if (stop_time > 0.0) {
						kill(pid, SIGSTOP);
						if (stop_time >= 1.0) {
							sleep((int)stop_time);
							stop_time -= (int)stop_time;
						}
						usleep((int)(stop_time*1e6));
						kill(pid, SIGCONT);
					}
				}
				++check_time;
			}
			usleep(rs->interval);
		}
		if (ret < 0) rr->error |= 1;
		is_killed = 0;
retstat:
		if (is_killed) rr->error |= 2;
		times(&time_used);
		run_get_dynamic_sys_info(&rsd);
		rr->retval = WEXITSTATUS(status);
		rr->rtime = rsd.wall_clock - start_time;
		rr->utime = (is_killed)? rpd.utime : time_used.tms_cutime / 100.0;
		rr->stime = (is_killed)? rpd.stime : time_used.tms_cstime / 100.0;
		rr->avg_rss = sum_rss / (rpd.utime + rpd.stime);
		rr->avg_vsize = sum_vsize / (rpd.utime + rpd.stime);
		if (rs->prefix) {
			pthread_join(outid, 0);
			pthread_join(errid, 0);
		}
		return rr;
	} else {
		char **true_argv = (char**)malloc(sizeof(char*) * (argc + 1));
		int i;
		for (i = 0; i < argc; ++i) true_argv[i] = argv[i];
		true_argv[i] = 0;
		usleep(1000);
		if (rs->prefix) {
			dup2(rs->fd_out[1], STDOUT_FILENO);
			close(rs->fd_out[0]); close(rs->fd_out[1]);
			dup2(rs->fd_err[1], STDERR_FILENO);
			close(rs->fd_err[0]); close(rs->fd_err[1]);
		}
		if (execvp(true_argv[0], true_argv) == -1) {
			fprintf(stderr, "** fail to lauch the program '%s'!\n", true_argv[0]);
			free(true_argv);
			exit(-1);
		}
	}
	return 0;
}

static void show_sys_info(FILE *fpout)
{
	RunSysStatic rss;
	RunSysDyn rsd;
	run_get_static_sys_info(&rss);
	run_get_dynamic_sys_info(&rsd);
	fprintf(fpout, "\n");
	fprintf(fpout, "-- totalmem  %16.3f kB\n", rss.mem_total / 1024.0);
	fprintf(fpout, "-- available %16.3f kB\n", rsd.mem_available / 1024.0);
	fprintf(fpout, "-- free      %16.3f kB\n", rsd.mem_free / 1024.0);
/*	fprintf(fpout, "-- page size %16.3f kB\n", rss.page_size / 1024.0); */
/*	fprintf(fpout, "-- virtual   %16.3f kB\n", rss.swap_total / 1024.0); */
}

static int usage()
{
	show_sys_info(stderr);
	fprintf(stderr, "\n");
	fprintf(stderr, "Program: runit (a program launcher)\n");
	fprintf(stderr, "Contact: Attractive Chaos\n");
	fprintf(stderr, "Usage:   runit [run_options] command [command_options]\n\n");
	fprintf(stderr, "Options: -c FLOAT    average CPU usage [1.0]\n");
	fprintf(stderr, "         -m FLOAT    lowest system memory [0.05]\n");
	fprintf(stderr, "         -T INT      timeout in second [0]\n");
	fprintf(stderr, "         -M INT      memoryout in kB [0]\n");
	fprintf(stderr, "         -p STR      log output and send to the background [null]\n\n");
	return 1;
}

int main_core(int argc, char *argv[], int offset, RunStruct *rs)
{
	int error, i;
	RunResult *rr;
	run_upd_RunStruct(rs);
	rr = runit(argc-offset, argv+offset, rs);
	if (rr) {
		fprintf(rs->log_fp, "-- CMD:");
		for (i = 0; i != argc; ++i)
			fprintf(rs->log_fp, " %s", argv[i]);
		fprintf(rs->log_fp, "\n");
		show_sys_info(rs->log_fp);
		fprintf(rs->log_fp, "\n");
		fprintf(rs->log_fp, "-- retval    %16d\n", rr->retval);
		fprintf(rs->log_fp, "-- real      %16.3f sec\n", rr->rtime);
		fprintf(rs->log_fp, "-- user      %16.3f sec\n", rr->utime);
		fprintf(rs->log_fp, "-- sys       %16.3f sec\n", rr->stime);
		fprintf(rs->log_fp, "-- maxrss    %16.3f kB\n", rr->max_rss / 1024.0);
		fprintf(rs->log_fp, "-- avgrss    %16.3f kB\n", rr->avg_rss / 1024.0);
		fprintf(rs->log_fp, "-- maxvsize  %16.3f kB\n", rr->max_vsize / 1024.0);
		fprintf(rs->log_fp, "-- avgvsize  %16.3f kB\n", rr->avg_vsize / 1024.0);
		fprintf(rs->log_fp, "\n");
	}
	error = rr->error;
	free(rr);
	run_free_RunStruct(rs);
	return error;
}

int main(int argc, char *argv[])
{
	int offset;
	RunStruct *rs;
	if (argc == 1) return usage();
	rs = run_new_RunStruct();
	offset = run_fill_RunStruct(argc, argv, rs);
	if (rs->prefix) {
		if (fork() == 0) {
			if (setsid() == -1) perror("Could mot become a session leader");
			return main_core(argc, argv, offset, rs);
		}
		return 0;
	}
	return main_core(argc, argv, offset, rs);
}

/*********************** standard getopt() ***********************************/


/*
 * getopt.c --
 *
 *      Standard UNIX getopt function.  Code is from BSD.
 *
 * Copyright (c) 1987-2002 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * A. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * B. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * C. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* #if !defined(lint)
 * static char sccsid[] = "@(#)getopt.c 8.2 (Berkeley) 4/2/94";
 * #endif
 */

int     opterr = 1,             /* if error message should be printed */
        optind = 1,             /* index into parent argv vector */
        optopt,                 /* character checked for validity */
        optreset;               /* reset getopt */
char    *optarg;                /* argument associated with option */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""

/*
 * getopt --
 *      Parse argc/argv argument vector.
 */
int getopt_standard(int nargc, char * const *nargv, const char *ostr)
{
        static char *place = EMSG;              /* option letter processing */
        char *oli;                              /* option letter list index */

        if (optreset || !*place) {              /* update scanning pointer */
                optreset = 0;
                if (optind >= nargc || *(place = nargv[optind]) != '-') {
                        place = EMSG;
                        return (EOF);
                }
                if (place[1] && *++place == '-') {      /* found "--" */
                        ++optind;
                        place = EMSG;
                        return (EOF);
                }
        }                                       /* option letter okay? */
        if ((optopt = (int)*place++) == (int)':' ||
            !(oli = strchr(ostr, optopt))) {
                /*
                 * if the user didn't specify '-' as an option,
                 * assume it means EOF.
                 */
                if (optopt == (int)'-')
                        return (EOF);
                if (!*place)
                        ++optind;
                if (opterr && *ostr != ':')
                        (void)fprintf(stderr,
                            "%s: illegal option -- %c\n", __FILE__, optopt);
                return (BADCH);
        }
        if (*++oli != ':') {                    /* don't need argument */
                optarg = NULL;
                if (!*place)
                        ++optind;
        }
        else {                                  /* need an argument */
                if (*place)                     /* no white space */
                        optarg = place;
                else if (nargc <= ++optind) {   /* no arg */
                        place = EMSG;
                        if (*ostr == ':')
                                return (BADARG);
                        if (opterr)
                                (void)fprintf(stderr,
                                    "%s: option requires an argument -- %c\n",
                                    __FILE__, optopt);
                        return (BADCH);
                }
                else                            /* white space */
                        optarg = nargv[optind];
                place = EMSG;
                ++optind;
        }
        return (optopt);                        /* dump back option letter */
}
