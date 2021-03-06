#ifndef _SUMMARY_USERS_HPP
#define _SUMMARY_USERS_HPP

#include <slurm/slurm.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_users_summary(FILE *file, job_info_msg_t *job_buffer_ptr);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_USERS_HPP */
