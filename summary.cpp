#include "summary.h"

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "common.h"
#include "jobs.h"
#include "nodes.h"
#include "partitions.h"
#include "summary_common.hpp"
#include "summary_job.hpp"
#include "summary_node.hpp"
#include "summary_partitions.hpp"
#include "summary_users.hpp"
#include "term.h"

void print_summary(FILE *file, const char *mode, const char *username) {
  job_info_msg_t *job_buffer_ptr = NULL;
  node_info_msg_t *node_buffer_ptr = NULL;
  partition_info_msg_t *part_info_ptr = NULL;

  /* get and dump some partition information */
  if (slurm_load_partitions((time_t)NULL, &part_info_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_partitions error");
    exit(1);
  }

  /* get and dump some job information */
  if (slurm_load_jobs((time_t)NULL, &job_buffer_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_jobs error");
    exit(1);
  }

  /* get and dump some node information */
  if (slurm_load_node((time_t)NULL, &node_buffer_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_node error");
    exit(1);
  }

  if (strcmp(mode, "all") == 0) {
    print_node_summary(file, job_buffer_ptr, node_buffer_ptr, part_info_ptr,
                       username);
    fprintf(file, "\n");
    print_job_summary(file, job_buffer_ptr, true, username);
    fprintf(file, "\n");
    print_partition_summary(file, job_buffer_ptr, node_buffer_ptr,
                            part_info_ptr);
    fprintf(file, "\n");
    print_users_summary(file, job_buffer_ptr);
  } else if (strcmp(mode, "jobs") == 0) {
    print_job_summary(file, job_buffer_ptr, true, username);
  } else if (strcmp(mode, "nodes") == 0) {
    print_node_summary(file, job_buffer_ptr, node_buffer_ptr, part_info_ptr,
                       username);
  } else if (strcmp(mode, "partitions") == 0) {
    print_partition_summary(file, job_buffer_ptr, node_buffer_ptr,
                            part_info_ptr);
  } else if (strcmp(mode, "users") == 0) {
    print_users_summary(file, job_buffer_ptr);
  } else {
    fprintf(stderr, "unknown summary mode: %s\n", mode);
    fprintf(stderr, "use -h to show help\n");
    exit(1);
  }

  slurm_free_job_info_msg(job_buffer_ptr);
  slurm_free_node_info_msg(node_buffer_ptr);
  slurm_free_partition_info_msg(part_info_ptr);
}
