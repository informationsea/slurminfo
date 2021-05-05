#ifndef _PARTITIONS_H
#define _PARTITIONS_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_cr(FILE *file, uint16_t cr_type);
void print_partition_flag(FILE *file, uint16_t flag);
const char *partition_state(uint16_t state);
const char *preempt_mode(uint16_t mode);
void print_partitions(FILE *file);

#ifdef __cplusplus
}
#endif

#endif /* _PARTITIONS_H */
