
#ifndef _SUMMARY_JOB
#define _SUMMARY_JOB

#include "slurm/slurm.h"
#include <functional>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

void print_job_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                       bool show_running_jobs, const char *show_username);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_JOB */
