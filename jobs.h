#ifndef _JOBS_H
#define _JOBS_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *get_reason_text(uint16_t state_reason);
void print_jobs(FILE *file);

#ifdef __cplusplus
}
#endif

#endif /* _JOBS_H */
