#ifndef _SUMMARY_NODE_HPP
#define _SUMMARY_NODE_HPP

#include <slurm/slurm.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_node_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                        node_info_msg_t *node_buffer_ptr,
                        partition_info_msg_t *part_info_ptr,
                        const char *show_username);

#ifdef __cplusplus
}
#endif

#endif /* _SUMMARY_NODE_HPP */
