#ifndef _SLURM_USERS_H
#define _SLURM_USERS_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void get_username(char *buf, size_t buflen);
void print_account_limit(const char *username, FILE* stdout);

#ifdef __cplusplus
}
#endif

#endif /* _SLURM_USERS_H */
