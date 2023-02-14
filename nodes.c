#include "nodes.h"
#include "common.h"
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *node_state(uint32_t state) {
  switch (state & NODE_STATE_BASE) {
  case NODE_STATE_UNKNOWN:
    return "Unknown";
  case NODE_STATE_DOWN:
    return "Down";
  case NODE_STATE_IDLE:
    return "Idle";
  case NODE_STATE_ALLOCATED:
    return "Allocated";
  case NODE_STATE_ERROR:
    return "Error";
  case NODE_STATE_MIXED:
    return "Mixed";
  case NODE_STATE_FUTURE:
    return "(Future)";
  case NODE_STATE_END:
    return "(End)";
  }
  return "(Unknown)";
}

const char *node_state_short(uint32_t state) {
  if (state & NODE_STATE_MAINT) {
    return "Mante";
  }
  if (state & NODE_STATE_DRAIN) {
    return "Drain";
  }
  switch (state & NODE_STATE_BASE) {
  case NODE_STATE_UNKNOWN:
    return "Unkwn";
  case NODE_STATE_DOWN:
    return "Down ";
  case NODE_STATE_IDLE:
    return "Idle ";
  case NODE_STATE_ALLOCATED:
    return "Alloc";
  case NODE_STATE_ERROR:
    return "Error";
  case NODE_STATE_MIXED:
    return "Mixed";
  case NODE_STATE_FUTURE:
    return "(Future)";
  case NODE_STATE_END:
    return "(End)";
  }
  return "(Unknown)";
}

void print_node_state(FILE *file, uint32_t state) {
  if (state == NO_VAL) {
    fprintf(file, "(null)");
    return;
  }
  if (state & NODE_STATE_RES) {
    fprintf(file, "Reserved,");
  }
  if (state & NODE_STATE_DRAIN) {
    fprintf(file, "Drain,");
  }
  if (state & NODE_STATE_COMPLETING) {
    fprintf(file, "Completing,");
  }
  if (state & NODE_STATE_NO_RESPOND) {
    fprintf(file, "NoRespond,");
  }
  if (state & NODE_STATE_POWERED_DOWN) {
    fprintf(file, "PoweredDown,");
  }
  if (state & NODE_STATE_FAIL) {
    fprintf(file, "Fail,");
  }
  if (state & NODE_STATE_POWERING_UP) {
    fprintf(file, "PoweringUp,");
  }
  if (state & NODE_STATE_MAINT) {
    fprintf(file, "Maintenance,");
  }
  if (state & NODE_STATE_REBOOT_REQUESTED) {
    fprintf(file, "RebootRequested,");
  }
  if (state & NODE_STATE_REBOOT_CANCEL) {
    fprintf(file, "RebootCancel,");
  }
  if (state & NODE_STATE_POWERING_DOWN) {
    fprintf(file, "PoweringDown,");
  }
  if (state & NODE_STATE_DYNAMIC) {
    fprintf(file, "Dynamic,");
  }
  if (state & NODE_STATE_REBOOT_ISSUED) {
    fprintf(file, "RebootIssued,");
  }
  if (state & NODE_STATE_PLANNED) {
    fprintf(file, "Planned,");
  }
  if (state & NODE_STATE_INVALID_REG) {
    fprintf(file, "InvalidReg,");
  }
  if (state & NODE_STATE_POWER_DOWN) {
    fprintf(file, "PowerDown,");
  }
  if (state & NODE_STATE_POWER_UP) {
    fprintf(file, "PowerUp,");
  }
  if (state & NODE_STATE_POWER_DRAIN) {
    fprintf(file, "PowerDrain,");
  }
  fprintf(file, "%s", node_state(state));
}

void print_nodes(FILE *file) {
  node_info_msg_t *node_buffer_ptr = NULL;

  /* get and dump some node information */
  if (slurm_load_node((time_t)NULL, &node_buffer_ptr, SHOW_ALL)) {
    slurm_perror("slurm_load_node error");
    exit(1);
  }

  fprintf(file, "ARCH\t"
                "BOARDS\t"
                "BOOT_TIME\t"
                "CLUSTER_NAME\t"
                "CORES_PER_SOCKET\t"
                "CPU_BIND\t"
                "CPU_LOAD\t"
                "FREEMEM\t"
                "CPUS\t"
                "FEATURES\t"
                "FEATURES_ACTIVE\t"
                "GRES\t"
                "GRES_DRAIN\t"
                "GRES_USED\t"
                "MCS_LABEL\t"
                "MEM_SPEC_LIMIT\t"
                "NAME\t"
                "NEXT_STATE\t"
                "NODE_ADDR\t"
                "NODE_HOSTNAME\t"
                "STATE\t"
                "OS\t"
                "OWNER\t"
                "PARTITIONS\t"
                "PORT\t"
                "REAL_MEMORY\t"
                "REASON\t"
                "REASON_TIME\t"
                "SLURMD_START_TIME\t"
                "SOCKETS\t"
                "THREADS\t"
                "TMP_DISK\t"
                "WEIGHT\t"
                "TRES\t"
                "VERSION\n");

  for (size_t i = 0; i < node_buffer_ptr->record_count; i++) {
    node_info_t *info = &node_buffer_ptr->node_array[i];
    fprintf(file, "%s\t", info->arch);
    fprintf(file, "%u\t", info->boards);

    print_time(file, info->boot_time, false);
    fprintf(file, "%s\t", info->cluster_name);
    fprintf(file, "%u\t", info->cores);
    fprintf(file, "%u\t", info->cpu_bind);
    float cpu_load = info->cpu_load;
    fprintf(file, "%.2f\t", cpu_load / 100);
    print_u64(file, info->free_mem, false);
    fprintf(file, "%u\t", info->cpus);
    fprintf(file, "%s\t", info->features);
    fprintf(file, "%s\t", info->features_act);
    fprintf(file, "%s\t", info->gres);
    fprintf(file, "%s\t", info->gres_drain);
    fprintf(file, "%s\t", info->gres_used);
    fprintf(file, "%s\t", info->mcs_label);
    fprintf(file, "%lu\t", info->mem_spec_limit);
    fprintf(file, "%s\t", info->name);
    print_node_state(file, info->next_state);
    fprintf(file, "\t");
    fprintf(file, "%s\t", info->node_addr);
    fprintf(file, "%s\t", info->node_hostname);
    print_node_state(file, info->node_state);
    fprintf(file, "\t");
    fprintf(file, "%s\t", info->os);
    print_u32(file, info->owner, false);
    fprintf(file, "%s\t", info->partitions);
    fprintf(file, "%u\t", info->port);
    fprintf(file, "%lu\t", info->real_memory);
    fprintf(file, "%s\t", info->reason);
    print_time(file, info->reason_time, false);
    print_time(file, info->slurmd_start_time, false);
    fprintf(file, "%u\t", info->sockets);
    fprintf(file, "%u\t", info->threads);
    fprintf(file, "%u\t", info->tmp_disk);
    fprintf(file, "%u\t", info->weight);
    fprintf(file, "%s\t", info->tres_fmt_str);
    fprintf(file, "%s\n", info->version);
  }

  slurm_free_node_info_msg(node_buffer_ptr);
}
