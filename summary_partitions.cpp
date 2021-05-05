#include "summary_partitions.hpp"

#include <map>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <vector>

#include "common.h"
#include "nodes.h"
#include "partitions.h"
#include "slurm/slurm.h"
#include "summary_common.hpp"
#include "term.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

void print_partition_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                             node_info_msg_t *node_buffer_ptr,
                             partition_info_msg_t *part_info_ptr) {

  std::map<size_t, std::vector<slurm_job_info_t *>> node2jobs;

  term_set_background_color(file, TERM_YELLOW);
  term_set_foreground_color(file, TERM_BLACK);
  // term_set_reverse(file, true);
  fprintf(file, "=================== Partition List ===================\n");
  term_set_background_color(file, TERM_DEFAULT);
  term_set_foreground_color(file, TERM_DEFAULT);

  for (size_t i = 0; i < job_buffer_ptr->record_count; i++) {
    slurm_job_info_t *job_ptr = &job_buffer_ptr->job_array[i];
    for (size_t j = 0; job_ptr->node_inx[j] >= 0; j += 2) {
      for (int32_t index = job_ptr->node_inx[j];
           index <= job_ptr->node_inx[j + 1]; index++) {
        node2jobs[index].push_back(job_ptr);
      }
    }
  }

  term_set_bold(file, true);
  fprintf(file, "           Node        Total   Total  Executable Jobs(CPU)  "
                "Executable Jobs(Memory)   Executable Jobs(GPU)");
  term_set_bold(file, false);
  fprintf(file, "\n");
  term_set_bold(file, true);
  fprintf(file, "Name       Avail Total   CPU   Mem(G) 1CPU 4CPU 8CPU 24CPU   "
                "4G  16G  64G  80G 170G      1 board  8 board");
  term_set_bold(file, false);
  fprintf(file, "\n");

  for (size_t i = 0; i < part_info_ptr->record_count; i++) {
    partition_info_t *info = &part_info_ptr->partition_array[i];
    // term_set_reverse(file, true);
    //
    // fprintf(file, "Partition: %-10s  Nodes: %3d  Status: %s\n", info->name,
    //        info->total_nodes, partition_state(info->state_up));
    // term_set_reverse(file, false);

    int up_nodes = 0;
    int total_cpu = 0;
    int total_gpu = 0;
    size_t total_mem = 0;

    int cpu_1core = 0;
    int cpu_4core = 0;
    int cpu_8core = 0;
    int cpu_24core = 0;

    int mem_4g = 0;
    int mem_16g = 0;
    int mem_64g = 0;
    int mem_80g = 0;
    int mem_170g = 0;

    int gpu_1board = 0;
    int gpu_8board = 0;

    for (size_t j = 0; info->node_inx[j] >= 0; j += 2) {
      for (int32_t index = info->node_inx[j]; index <= info->node_inx[j + 1];
           index++) {
        node_info_t *node_info = &node_buffer_ptr->node_array[index];
        Tres node_tres = Tres(node_info->tres_fmt_str);

        total_cpu += node_tres.cpu;
        total_mem += node_tres.memory;
        total_gpu += node_tres.gpu;

        if ((node_info->node_state & NODE_STATE_BASE) == NODE_STATE_IDLE ||
            (node_info->node_state & NODE_STATE_BASE) == NODE_STATE_ALLOCATED ||
            (node_info->node_state & NODE_STATE_BASE) == NODE_STATE_MIXED) {
          up_nodes += 1;

          int alloc_cpu = 0;
          int alloc_gpu = 0;
          size_t alloc_mem = 0;
          for (auto one_job : node2jobs[index]) {
            if ((one_job->job_state & JOB_STATE_BASE) == JOB_RUNNING) {
              auto tres = Tres(one_job->tres_alloc_str);
              alloc_cpu += tres.cpu;
              alloc_mem += tres.memory;
              alloc_gpu += tres.gpu;
              continue;
            }
          }

          uint64_t non_reserved_mem = node_tres.memory - alloc_mem;

          uint64_t avail_mem = node_info->free_mem < non_reserved_mem
                                   ? node_info->free_mem
                                   : non_reserved_mem;
          int avail_cpu = node_tres.cpu - alloc_cpu;
          int avail_gpu = node_tres.gpu - alloc_gpu;

          cpu_1core += avail_cpu;
          cpu_4core += avail_cpu / 4;
          cpu_8core += avail_cpu / 8;
          cpu_24core += avail_cpu / 24;

          mem_4g += MIN(avail_mem / (1024 * 4), (uint64_t)avail_cpu);
          mem_16g += MIN(avail_mem / (1024 * 16), (uint64_t)avail_cpu);
          mem_64g += MIN(avail_mem / (1024 * 64), (uint64_t)avail_cpu);
          mem_80g += MIN(avail_mem / (1024 * 80), (uint64_t)avail_cpu);
          mem_170g += MIN(avail_mem / (1024 * 170), (uint64_t)avail_cpu);

          gpu_1board += avail_gpu;
          gpu_8board += avail_gpu / 8;
        }
      }
    }

    fprintf(file,
            "%-10s  %4d  %4d  %4d %8lu %4d %4d %4d  %4d %4d %4d %4d %4d %4d    "
            "       %2d       %2d\n",
            info->name, up_nodes, info->total_nodes, info->total_cpus,
            total_mem / 1024, cpu_1core, cpu_4core, cpu_8core, cpu_24core,
            mem_4g, mem_16g, mem_64g, mem_80g, mem_170g, gpu_1board,
            gpu_8board);
  }
}
