#include "summary_users.hpp"

#include <map>
#include <pwd.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "summary_common.hpp"
#include "term.h"

void print_users_summary(FILE *file, job_info_msg_t *job_buffer_ptr) {
  term_set_background_color(file, TERM_YELLOW);
  term_set_foreground_color(file, TERM_BLACK);
  fprintf(file, "================= User List =================");
  term_set_foreground_color(file, TERM_DEFAULT);
  term_set_background_color(file, TERM_DEFAULT);
  fprintf(file, "\n");

  std::set<uid_t> users;

  std::map<uid_t, int> user_running_jobs;
  std::map<uid_t, int> user_pending_jobs;
  std::map<uid_t, int> user_blocked_jobs;
  std::map<uid_t, int> user_error_jobs;

  std::map<uid_t, int> user_running_cpus;
  std::map<uid_t, int> user_pending_cpus;
  std::map<uid_t, int> user_blocked_cpus;
  std::map<uid_t, int> user_error_cpus;

  std::map<uid_t, int> user_running_mem;
  std::map<uid_t, int> user_pending_mem;
  std::map<uid_t, int> user_blocked_mem;
  std::map<uid_t, int> user_error_mem;

  for (size_t i = 0; i < job_buffer_ptr->record_count; i++) {
    slurm_job_info_t *job_ptr = &job_buffer_ptr->job_array[i];
    Tres req_tres = Tres(job_ptr->tres_req_str);
    switch (job_ptr->job_state & JOB_STATE_BASE) {
    case JOB_PENDING:
      users.insert(job_ptr->user_id);
      switch (job_ptr->state_reason) {
      case WAIT_PRIORITY:
      case WAIT_RESOURCES:
      case WAIT_PART_NODE_LIMIT:
      case WAIT_PART_TIME_LIMIT:
        user_pending_jobs[job_ptr->user_id] += 1;
        user_pending_cpus[job_ptr->user_id] += req_tres.cpu;
        user_pending_mem[job_ptr->user_id] += req_tres.memory;
        break;
      default:
        user_blocked_jobs[job_ptr->user_id] += 1;
        user_blocked_cpus[job_ptr->user_id] += req_tres.cpu;
        user_blocked_mem[job_ptr->user_id] += req_tres.memory;
        break;
      }
      break;
    case JOB_RUNNING: {
      users.insert(job_ptr->user_id);
      Tres alloc_tres = Tres(job_ptr->tres_req_str);
      user_running_jobs[job_ptr->user_id] += 1;
      user_running_cpus[job_ptr->user_id] += alloc_tres.cpu;
      user_running_mem[job_ptr->user_id] += alloc_tres.memory;
    } break;
    case JOB_SUSPENDED:
    case JOB_PREEMPTED:
      users.insert(job_ptr->user_id);
      user_blocked_jobs[job_ptr->user_id] += 1;
      user_blocked_cpus[job_ptr->user_id] += req_tres.cpu;
      user_blocked_mem[job_ptr->user_id] += req_tres.memory;
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
      users.insert(job_ptr->user_id);
      user_error_jobs[job_ptr->user_id] += 1;
      user_error_cpus[job_ptr->user_id] += req_tres.cpu;
      user_error_mem[job_ptr->user_id] += req_tres.memory;
      break;
    }
  }

  term_set_bold(file, true);
  fprintf(file, "                Running             Pending             "
                "Blocked             Error");
  term_set_bold(file, false);
  fprintf(file, "\n");

  term_set_bold(file, true);
  fprintf(file, "User            Jobs   CPU  MEM(G)  Jobs   CPU  MEM(G)  Jobs  "
                " CPU  MEM(G)  Jobs   CPU  MEM(G)");
  term_set_bold(file, false);
  fprintf(file, "\n");

  for (auto one_user : users) {
    struct passwd *pwd = getpwuid(one_user);
    const char *username;
    if (pwd) {
      username = pwd->pw_name;
    } else {
      username = "<unknown>";
    }
    fprintf(file,
            "%-14s %5d %5d %7.1f %5d %5d %7.1f %5d %5d %7.1f %5d %5d %7.1f\n",
            username, user_running_jobs[one_user], user_running_cpus[one_user],
            user_running_mem[one_user] / 1024., user_pending_jobs[one_user],
            user_pending_cpus[one_user], user_pending_mem[one_user] / 1024.,
            user_blocked_jobs[one_user], user_blocked_cpus[one_user],
            user_blocked_mem[one_user] / 1024., user_error_jobs[one_user],
            user_error_cpus[one_user], user_error_mem[one_user] / 1024.);
  }
}
