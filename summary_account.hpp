#ifndef _SUMMARY_ACCOUNT_HPP
#define _SUMMARY_ACCOUNT_HPP

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_job_account_summary(FILE *file, const char *job_id,
                               const char *start_time, const char *end_time);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_ACCOUNT_HPP */
