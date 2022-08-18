#include "summary_node.hpp"

#include <map>
#include <pwd.h>
#include <slurm/slurm.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "common.h"
#include "nodes.h"
#include "partitions.h"
#include "summary_common.hpp"
#include "term.h"

void print_node_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                        node_info_msg_t *node_buffer_ptr,
                        partition_info_msg_t *part_info_ptr,
                        const char *show_username) {

  std::map<size_t, std::vector<slurm_job_info_t *>> node2jobs;

  term_set_background_color(file, TERM_YELLOW);
  term_set_foreground_color(file, TERM_BLACK);
  // term_set_reverse(file, true);
  fprintf(file, "=================== Node List ===================\n");
  term_set_background_color(file, TERM_DEFAULT);
  term_set_foreground_color(file, TERM_DEFAULT);
  fprintf(
      file,
      "  Node: [NAME]  State: [STATE]  Job: [# of running jobs]  Mem: "
      "[Used]/[Allocated]/[Total]  CPU: [Allocated]/[Total]  Load:[LOAD]\n");

  int maximum_length_of_username = 0;
  for (size_t i = 0; i < job_buffer_ptr->record_count; i++) {
    slurm_job_info_t *job_ptr = &job_buffer_ptr->job_array[i];
    for (size_t j = 0; job_ptr->node_inx[j] >= 0; j += 2) {
      for (int32_t index = job_ptr->node_inx[j];
           index <= job_ptr->node_inx[j + 1]; index++) {
        node2jobs[index].push_back(job_ptr);
      }
    }

    struct passwd *pwd = getpwuid(job_ptr->user_id);
    const char *username;
    if (pwd) {
      username = pwd->pw_name;
    } else {
      username = "<unknown>";
    }

    if ((strcmp(show_username, username) == 0) ||
        (strcmp(show_username, "*") == 0)) {

      int l = (int)strnlen(username, 200);
      if (l > maximum_length_of_username) {
        maximum_length_of_username = l;
      }
    }
  }

  for (size_t i = 0; i < part_info_ptr->record_count; i++) {
    partition_info_t *info = &part_info_ptr->partition_array[i];
    term_set_reverse(file, true);

    fprintf(file, "Partition: %-10s  Nodes: %3d  Status: %s\n", info->name,
            info->total_nodes, partition_state(info->state_up));
    term_set_reverse(file, false);
    for (size_t j = 0; info->node_inx[j] >= 0; j += 2) {
      for (int32_t index = info->node_inx[j]; index <= info->node_inx[j + 1];
           index++) {
        node_info_t *node_info = &node_buffer_ptr->node_array[index];
        Tres node_tres = Tres(node_info->tres_fmt_str);

        if ((node_info->node_state & NODE_STATE_BASE) == NODE_STATE_FUTURE) {
          continue; // skip reserved nodes
        }

        if ((node_info->node_state & NODE_STATE_BASE) == NODE_STATE_DOWN ||
            (node_info->node_state & NODE_STATE_BASE) == NODE_STATE_ERROR ||
            (node_info->node_state & NODE_STATE_BASE) == NODE_STATE_UNKNOWN) {
          term_set_foreground_color(file, TERM_RED);
          fprintf(file, "  Node: %-8s  State: %s %s\n", node_info->name,
                  node_state_short(node_info->node_state), node_info->reason);
          term_set_foreground_color(file, TERM_DEFAULT);
        } else {
          int alloc_cpu = 0;
          int alloc_gpu = 0;
          size_t alloc_mem = 0;
          int num_job = 0;
          for (auto one_job : node2jobs[index]) {
            if ((one_job->job_state & JOB_STATE_BASE) == JOB_RUNNING) {
              auto tres = Tres(one_job->tres_alloc_str);
              alloc_cpu += tres.cpu;
              alloc_mem += tres.memory;
              alloc_gpu += tres.gpu;
              num_job += 1;
            }
          }

          char used_mem_str[100];
          char total_mem_str[100];
          char alloc_mem_str[100];
          giga_memory(used_mem_str, sizeof(used_mem_str) - 1,
                      node_tres.memory > node_info->free_mem
                          ? node_tres.memory - node_info->free_mem
                          : 0);
          giga_memory(total_mem_str, sizeof(total_mem_str) - 1,
                      node_tres.memory);
          giga_memory(alloc_mem_str, sizeof(alloc_mem_str) - 1, alloc_mem);

          if (node_info->node_state & NODE_STATE_DRAIN) {
            term_set_foreground_color(file, TERM_RED);
          } else {
            switch (node_info->node_state & NODE_STATE_BASE) {
            case NODE_STATE_IDLE:
              term_set_foreground_color(file, TERM_GREEN);
              break;
            case NODE_STATE_ALLOCATED:
            case NODE_STATE_MIXED:
              term_set_foreground_color(file, TERM_YELLOW);
              break;
            default:
              term_set_foreground_color(file, TERM_RED);
              break;
            }
          }

          fprintf(file,
                  "  Node: %-8s  State: %s  Job: %2d  Mem: %sG/%sG/%sG  CPU: "
                  "%u/%u  "
                  "Load:%.2f",
                  node_info->name, node_state_short(node_info->node_state),
                  num_job, used_mem_str, alloc_mem_str, total_mem_str,
                  alloc_cpu, node_tres.cpu, (node_info->cpu_load / 100.));
          if (node_tres.gpu > 0) {
            fprintf(file, " GPU: %u/%u", alloc_gpu, node_tres.gpu);
          }
          term_set_foreground_color(file, TERM_DEFAULT);
          fprintf(file, "\n");
          for (auto one_job : node2jobs[index]) {
            if ((one_job->job_state & JOB_STATE_BASE) != JOB_RUNNING) {
              continue;
            }
            if (strcmp(one_job->partition, info->name) == 0) {
              auto tres = Tres(one_job->tres_alloc_str);
              char job_id_buf[100];
              memset(job_id_buf, 0, sizeof(job_id_buf));
              job_id(job_id_buf, sizeof(job_id_buf) - 1, one_job);
              char mem[6];
              memset(mem, 0, sizeof(mem));
              giga_memory(mem, sizeof(mem) - 1, tres.memory);
              struct passwd *pwd = getpwuid(one_job->user_id);
              const char *username;
              if (pwd) {
                username = pwd->pw_name;
              } else {
                username = "<unknown>";
              }
              if ((strcmp(show_username, username) == 0) ||
                  (strcmp(show_username, "*") == 0)) {

                char start_time[30];
                memset(start_time, 0, sizeof(start_time));
                sprint_duration_time(start_time, sizeof(start_time) - 1,
                                     one_job->start_time);

                if (one_job->user_id == getuid()) {
                  term_set_underline(file, true);
                }
                fprintf(file,
                        "    Job: %-12s  CPU:%2d MEM:%5sG User: "
                        "%-*s  Time: %-12s "
                        " Name: %s\n",
                        job_id_buf, tres.cpu, mem, maximum_length_of_username,
                        username, start_time, one_job->name);
                if (one_job->user_id == getuid()) {
                  term_set_underline(file, false);
                }
              }
            }
          }
        }
      }
    }
  }
}
