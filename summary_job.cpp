#include "summary_job.hpp"

#include <algorithm>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "jobs.h"
#include "summary_common.hpp"
#include "term.h"

typedef std::function<void(
    FILE *file, slurm_job_info_t *jobinfo, const Tres &tres, size_t index,
    const char *job_id, const char *mem, const char *username,
    const char *start_duration, const char *submit_duration,
    const char *state_reason)>
    job_print_function_t;

void print_job_vector_summary(FILE *file, const char *show_username,
                              const std::vector<slurm_job_info_t *> &jobs,
                              const std::string &list_name,
                              const std::string &list_header,
                              job_print_function_t job_print);

void print_job_summary(FILE *file, job_info_msg_t *job_buffer_ptr,
                       bool show_running_jobs, const char *show_username) {

  term_set_background_color(file, TERM_YELLOW);
  term_set_foreground_color(file, TERM_BLACK);
  fprintf(file, "================= Job List =================");
  term_set_foreground_color(file, TERM_DEFAULT);
  term_set_background_color(file, TERM_DEFAULT);
  fprintf(file, "\n");

  std::vector<slurm_job_info_t *> running_jobs;
  std::vector<slurm_job_info_t *> pending_jobs;
  std::vector<slurm_job_info_t *> blocked_jobs;
  std::vector<slurm_job_info_t *> error_jobs;

  for (size_t i = 0; i < job_buffer_ptr->record_count; i++) {
    slurm_job_info_t *job_ptr = &job_buffer_ptr->job_array[i];
    switch (job_ptr->job_state & JOB_STATE_BASE) {
    case JOB_PENDING:
      switch (job_ptr->state_reason) {
      case WAIT_PRIORITY:
      case WAIT_RESOURCES:
      case WAIT_PART_NODE_LIMIT:
      case WAIT_PART_TIME_LIMIT:
        pending_jobs.push_back(job_ptr);
        break;
      default:
        blocked_jobs.push_back(job_ptr);
        break;
      }
      break;
    case JOB_RUNNING:
      running_jobs.push_back(job_ptr);
      break;
    case JOB_SUSPENDED:
    case JOB_PREEMPTED:
      blocked_jobs.push_back(job_ptr);
      break;
    case JOB_COMPLETE:
    case JOB_CANCELLED:
      // do nothing
      break;
    case JOB_TIMEOUT:
    case JOB_NODE_FAIL:
    case JOB_BOOT_FAIL:
    case JOB_DEADLINE:
    case JOB_OOM:
    default:
      error_jobs.push_back(job_ptr);
      break;
    }
  }

  std::sort(pending_jobs.begin(), pending_jobs.end(),
            [](slurm_job_info_t *a, slurm_job_info_t *b) {
              return a->priority > b->priority;
            });

  std::sort(running_jobs.begin(), running_jobs.end(),
            [](slurm_job_info_t *a, slurm_job_info_t *b) {
              return a->start_time < b->start_time;
            });

  std::sort(blocked_jobs.begin(), blocked_jobs.end(),
            [](slurm_job_info_t *a, slurm_job_info_t *b) {
              return a->submit_time < b->submit_time;
            });

  std::sort(error_jobs.begin(), error_jobs.end(),
            [](slurm_job_info_t *a, slurm_job_info_t *b) {
              return a->submit_time < b->submit_time;
            });

  if (show_running_jobs) {
    print_job_vector_summary(
        file, show_username, running_jobs, "Running Jobs",
        "     # Job ID          CPU   Memory  Partition Node      Running Time  "
        "User "
        "       Name",
        [](FILE *file, slurm_job_info_t *jobinfo, const Tres &tres,
           size_t index, const char *job_id, const char *mem,
           const char *username, const char *start_duration,
           const char * /*submit_duration*/, const char * /*state_reason*/) {
          term_set_foreground_color(file, TERM_GREEN);
          fprintf(file, "R");
          term_set_foreground_color(file, TERM_DEFAULT);
          fprintf(file, "%5lu %-16s %2u  %6sG  %-8s  %-8s  %12s  %-10s  %s",
                  index, job_id, tres.cpu, mem, jobinfo->partition,
                  jobinfo->nodes, start_duration, username, jobinfo->name);
        });
  }

  print_job_vector_summary(
      file, show_username, pending_jobs, "Pending Jobs",
      "     # Job ID          CPU   Memory  Partition    Wait Time  User       "
      "Reason                               Name",
      [show_username](FILE *file, slurm_job_info_t *jobinfo, const Tres &tres,
                      size_t index, const char *job_id, const char *mem,
                      const char *username, const char * /*start_duration*/,
                      const char *submit_duration, const char *state_reason) {
        term_set_foreground_color(file, TERM_YELLOW);
        fprintf(file, "P");
        term_set_foreground_color(file, TERM_DEFAULT);
        fprintf(file, "%5lu %-16s %2u  %6sG  %-8s  %12s  %-10s %-35s  %s",
                index, job_id, tres.cpu, mem, jobinfo->partition,
                submit_duration, username, state_reason, jobinfo->name);
      });
  print_job_vector_summary(
      file, show_username, blocked_jobs, "Blocked Jobs",
      "     # Job ID          CPU   Memory  Partition    Wait Time  User       "
      "Reason                               Name",
      [](FILE *file, slurm_job_info_t *jobinfo, const Tres &tres, size_t index,
         const char *job_id, const char *mem, const char *username,
         const char * /*start_duration*/, const char *submit_duration,
         const char *state_reason) {
        term_set_foreground_color(file, TERM_CYAN);
        fprintf(file, "B");
        term_set_foreground_color(file, TERM_DEFAULT);

        fprintf(file, "%5lu %-16s %2u  %6sG  %-8s  %12s  %-10s %-35s  %s",
                index, job_id, tres.cpu, mem, jobinfo->partition,
                submit_duration, username, state_reason, jobinfo->name);
      });
  print_job_vector_summary(
      file, show_username, error_jobs, "Error Jobs",
      "     # Job ID          CPU   Memory  Partition    Wait Time  User       "
      "Reason                               Name",
      [](FILE *file, slurm_job_info_t *jobinfo, const Tres &tres, size_t index,
         const char *job_id, const char *mem, const char *username,
         const char * /*start_duration*/, const char *submit_duration,
         const char *state_reason) {
        term_set_foreground_color(file, TERM_RED);
        fprintf(file, "E");
        term_set_foreground_color(file, TERM_DEFAULT);

        fprintf(file, "%5lu %-16s %2u  %6sG  %-8s  %12s  %-10s %-35s  %s",
                index, job_id, tres.cpu, mem, jobinfo->partition,
                submit_duration, username, state_reason, jobinfo->name);
      });
}

void print_job_vector_summary(FILE *file, const char *show_username,
                              const std::vector<slurm_job_info_t *> &jobs,
                              const std::string &list_name,
                              const std::string &list_header,
                              job_print_function_t job_print) {
  if (jobs.empty()) {
    return;
  }
  term_set_reverse(file, true);
  fprintf(file, " ----------------- %s ----------------- ", list_name.c_str());
  term_set_reverse(file, false);
  fprintf(file, "\n");
  term_set_bold(file, true);
  fprintf(file, "%s", list_header.c_str());
  term_set_bold(file, false);
  fprintf(file, "\n");

  size_t index = 0;
  for (auto one_job = jobs.begin(); one_job < jobs.end(); ++one_job) {
    auto tres = Tres((*one_job)->tres_req_str);
    char job_id_buf[100];
    memset(job_id_buf, 0, sizeof(job_id_buf));
    job_id(job_id_buf, sizeof(job_id_buf) - 1, *one_job);
    char mem[6];
    memset(mem, 0, sizeof(mem));
    giga_memory(mem, sizeof(mem) - 1, tres.memory);
    struct passwd *pwd = getpwuid((*one_job)->user_id);
    const char *username;
    if (pwd) {
      username = pwd->pw_name;
    } else {
      username = "<unknown>";
    }

    if ((strcmp(show_username, username) == 0) ||
        (strcmp(show_username, "*") == 0)) {

      char submit_duration[30];
      memset(submit_duration, 0, sizeof(submit_duration));
      sprint_duration_time(submit_duration, sizeof(submit_duration) - 1,
                           (*one_job)->submit_time);

      char start_duration[30];
      memset(start_duration, 0, sizeof(start_duration));
      sprint_duration_time(start_duration, sizeof(start_duration) - 1,
                           (*one_job)->start_time);

      char reason_buf[36];
      memset(reason_buf, 0, sizeof(reason_buf));
      const char *reason;
      if ((*one_job)->state_desc != nullptr) {
        reason = (*one_job)->state_desc;
      } else if ((*one_job)->state_reason == WAIT_DEPENDENCY) {
        snprintf(reason_buf, sizeof(reason_buf) - 1, "Depend:%s",
                 (*one_job)->dependency);
        reason = reason_buf;
      } else {
        reason = get_reason_text((*one_job)->state_reason);
      }

      if (getuid() == (*one_job)->user_id) {
        term_set_foreground_color(file, TERM_GREEN);
      }
      job_print(file, *one_job, tres, index, job_id_buf, mem, username,
                start_duration, submit_duration, reason);
      if (getuid() == (*one_job)->user_id) {
        term_set_foreground_color(file, TERM_DEFAULT);
      }
      fprintf(file, "\n");
    }
    index += 1;
  }
  term_set_foreground_color(file, TERM_YELLOW);
  fprintf(file, "   Total: %lu jobs", index);
  term_set_foreground_color(file, TERM_DEFAULT);
  fprintf(file, "\n");
}
