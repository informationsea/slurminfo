#ifndef _SUMMARY_H
#define _SUMMARY_H

#include "slurm/slurm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_summary(FILE *file, const char *mode, const char *username);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_H */
