#ifndef _SUMMARY_PARTITION_HPP
#define _SUMMARY_PARTITION_HPP

#include <slurm/slurm.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_partition_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                             node_info_msg_t *node_buffer_ptr,
                             partition_info_msg_t *part_info_ptr);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_PARTITION_HPP */
