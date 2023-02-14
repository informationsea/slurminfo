#ifndef _SUMMARY_ACCOUNT_HPP
#define _SUMMARY_ACCOUNT_HPP

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_job_account_summary(FILE *file, const char *job_id,
                               const char *show_user, const char *start_time,
                               const char *end_time, const char *state);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_ACCOUNT_HPP */
