#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "partitions.h"

void print_cr(FILE *file, uint16_t cr_type) {
  bool comma_needed = false;
  if (cr_type & CR_CPU) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "CPU");
    comma_needed = true;
  }
  if (cr_type & CR_SOCKET) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "SOCKET");
    comma_needed = true;
  }
  if (cr_type & CR_CORE) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "CORE");
    comma_needed = true;
  }
  if (cr_type & CR_BOARD) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "BOARD");
    comma_needed = true;
  }
  if (cr_type & CR_MEMORY) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "MEMORY");
    comma_needed = true;
  }
  if (cr_type & CR_OTHER_CONS_RES) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "OtherConsRes");
    comma_needed = true;
  }
  if (cr_type & CR_OTHER_CONS_TRES) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "OtherConsTres");
    comma_needed = true;
  }
  if (comma_needed) {
    fprintf(file, ",");
  }
  fprintf(file, "(%u)", cr_type);
}

void print_partition_flag(FILE *file, uint16_t flag) {
  bool comma_needed = false;
  if (flag & PART_FLAG_DEFAULT) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "Default");
    comma_needed = true;
  }
  if (flag & PART_FLAG_HIDDEN) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "Hidden");
    comma_needed = true;
  }
  if (flag & PART_FLAG_NO_ROOT) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "NoRoot");
    comma_needed = true;
  }
  if (flag & PART_FLAG_ROOT_ONLY) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "RootOnly");
    comma_needed = true;
  }
  if (flag & PART_FLAG_REQ_RESV) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "ReservationRequired");
    comma_needed = true;
  }
  if (flag & PART_FLAG_LLN) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "LeastLoadedNodeSelection");
    comma_needed = true;
  }
  if (flag & PART_FLAG_EXCLUSIVE_USER) {
    if (comma_needed) {
      fprintf(file, ",");
    }
    fprintf(file, "ExclusiveUser");
    comma_needed = true;
  }
  fprintf(file, "(%u)", flag);
}

const char *partition_state(uint16_t state) {
  if (state == PARTITION_DOWN) {
    return "Down";
  }
  if (state == PARTITION_UP) {
    return "Up";
  }
  if (state == PARTITION_DRAIN) {
    return "Drain";
  }
  if (state == PARTITION_INACTIVE) {
    return "Inactive";
  }
  return "Unknown";
}

const char *preempt_mode(uint16_t mode) {
  switch (mode) {
  case NO_VAL16:
    return "(null)";
  case PREEMPT_MODE_OFF:
    return "Off";
  case PREEMPT_MODE_SUSPEND:
    return "Suspend";
  case PREEMPT_MODE_REQUEUE:
    return "Requeue";
  case PREEMPT_MODE_CANCEL:
    return "Cancel";
  case PREEMPT_MODE_GANG:
    return "Gang";
  }
  return "Unknown";
}

void print_partitions(FILE *file) {
  partition_info_msg_t *part_info_ptr = NULL;

  /* get and dump some partition information */
  if (slurm_load_partitions((time_t)NULL, &part_info_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_partitions error");
    exit(1);
  }

  node_info_msg_t *node_buffer_ptr = NULL;

  /* get and dump some node information */
  if (slurm_load_node((time_t)NULL, &node_buffer_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_node error");
    exit(1);
  }

  fprintf(file, "ALLOW_ALLOC_NODES\t"
                "ALLOW_ACCOUNTS\t"
                "ALLOW_GROUPS\t"
                "ALLOW_QOS\t"
                "ALTERNATE_PARTITION_NAME\t"
                "BILLING_WEIGHTS\t"
                "CLUSTER_NAME\t"
                "CR_TYPE\t"
                "CPU_BIND\t"
                "DEFAULT_MEM_PER_CPU\t"
                "DEFAULT_TIME\t"
                "DENY_ACCOUNTS\t"
                "DENY_QOS\t"
                "FLAGS\t"
                "GRACE_TIME\t"
                "JOB_DEFAULTS\t"
                "MAX_CPU_PER_NODE\t"
                "MAX_MEM_PER_CPU\t"
                "MAX_NODES\t"
                "MAX_SHARE\t"
                "MAX_TIME\t"
                "MIN_NODES\t"
                "NAME\t"
                "NODES\t"
                "FULL_NODES\t"
                "OVERTIME_LIMIT\t"
                "PREEMPT_MODE\t"
                "JOB_PRIORITY_FACTOR\t"
                "PRIORITY_TIER\t"
                "QOS\t"
                "STATE\t"
                "TOTAL_CPUS\t"
                "TOTAL_NODES\t"
                "TRES\n");

  for (size_t i = 0; i < part_info_ptr->record_count; i++) {
    partition_info_t *info = &part_info_ptr->partition_array[i];
    fprintf(file, "%s\t", info->allow_alloc_nodes);
    fprintf(file, "%s\t", info->allow_accounts);
    fprintf(file, "%s\t", info->allow_groups);
    fprintf(file, "%s\t", info->allow_qos);
    fprintf(file, "%s\t", info->alternate);
    fprintf(file, "%s\t", info->billing_weights_str);
    fprintf(file, "%s\t", info->cluster_name);
    print_cr(file, info->cr_type);
    fprintf(file, "\t");
    fprintf(file, "%u\t", info->cpu_bind);
    print_u64(file, info->def_mem_per_cpu, false);
    print_u32(file, info->default_time, false);
    fprintf(file, "%s\t", info->deny_accounts);
    fprintf(file, "%s\t", info->deny_qos);
    print_partition_flag(file, info->flags);
    fprintf(file, "\t");
    print_u32(file, info->grace_time, false);
    fprintf(file, "%s\t", info->job_defaults_str);
    print_u32(file, info->max_cpus_per_node, false);
    print_u64(file, info->max_mem_per_cpu, false);
    print_u32(file, info->max_nodes, false);
    print_u16(file, info->max_share, false);
    print_u32(file, info->max_time, false);
    print_u32(file, info->min_nodes, false);
    fprintf(file, "%s\t", info->name);
    fprintf(file, "%s\t", info->nodes);
    for (int32_t *pos = info->node_inx; *pos != -1; pos++) {
      if (pos != info->node_inx) {
        fprintf(file, ",");
      }
      int32_t start = *pos++;
      int32_t end = *pos;
      for (int32_t index = start; index <= end; index++) {
        fprintf(file, "%s", node_buffer_ptr->node_array[index].name);
        if (index < end) {
          fprintf(file, ",");
        }
      }
    }
    fprintf(file, "\t");
    print_u16(file, info->over_time_limit, false);
    fprintf(file, "%s\t", preempt_mode(info->preempt_mode));
    print_u16(file, info->priority_job_factor, false);
    print_u16(file, info->priority_tier, false);
    fprintf(file, "%s\t", info->qos_char);
    fprintf(file, "%s\t", partition_state(info->state_up));
    print_u32(file, info->total_cpus, false);
    print_u32(file, info->total_nodes, false);
    fprintf(file, "%s\n", info->tres_fmt_str);
  }

  slurm_free_partition_info_msg(part_info_ptr);
  slurm_free_node_info_msg(node_buffer_ptr);
}
