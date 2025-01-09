#include <getopt.h>
#include <pwd.h>
#include <slurm/slurm.h>
#include <slurm/slurm_errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "assoc.h"
#include "common.h"
#include "jobs.h"
#include "nodes.h"
#include "partitions.h"
#include "reservations.h"
#include "slurmd.h"
#include "summary.h"
#include "summary_account.hpp"
#include "term.h"
#include "version.h"

int main(int argc, char **argv)
{
  bool show_help = false;
  bool show_version = false;
  bool detail_mode = false;
  const char *show_user = NULL;
  const char *job_id = NULL;
  const char *start_time = NULL;
  const char *end_time = NULL;
  const char *state = NULL;
  int exit_code = 0;

  int opt;
  while ((opt = getopt(argc, argv, "j:S:s:e:E:h?du:avV")) != -1)
  {
    switch (opt)
    {
    case 'h':
    case '?':
      show_help = true;
      break;
    case 'd':
      detail_mode = true;
      break;
    case 'u':
      show_user = optarg;
      break;
    case 'j':
      job_id = optarg;
      break;
    case 'S':
      start_time = optarg;
      break;
    case 'e':
      fprintf(stderr, "-e option was renamed to -E\n");
      end_time = optarg;
      break;
    case 'E':
      end_time = optarg;
      break;
    case 'a':
      show_user = "*";
      break;
    case 's':
      if ('0' <= optarg[0] && optarg[0] <= '9')
      {
        fprintf(stderr, "-s option was renamed to -S\n");
        start_time = optarg;
      }
      else
      {
        state = optarg;
      }
      break;
    case 'v':
    case 'V':
      show_version = true;
      break;
    default:
      show_help = true;
      exit_code = 1;
      break;
    }
  }

  if ((optind >= argc && detail_mode))
  {
    show_help = true;
    exit_code = 1;
    fprintf(stderr, "ERROR: type is required in detail mode.\n\n");
  }

  if (optind + 1 < argc)
  {
    show_help = true;
    exit_code = 1;
    fprintf(stderr, "ERROR: Too many arguments.\n\n");
  }

  if (show_help)
  {
    fprintf(stderr, "Usage: slurminfo [-dh?] [type]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d            : detail mode\n");
    fprintf(stderr, "  -h -?         : show this help\n");
    fprintf(stderr, "  -v -V         : show version\n");
    fprintf(stderr, "  -a            : show all users\n");
    fprintf(stderr, "  -u username   : show data for a user (only for normal "
                    "mode. use * for all users)\n");
    fprintf(stderr, "  -j job_id     : job ID (acct mode)\n");
    fprintf(stderr, "  -s state      : Selects jobs based on their state."
                    " (acct mode)\n");
    fprintf(stderr, "  -S start_time : Select jobs in any state after the "
                    "specified time. (acct mode)\n");
    fprintf(stderr, "  -E end_time   : Select jobs in any state before the "
                    "specified time. (acct mode)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Types in normal mode:\n");
    fprintf(stderr, " * partitions\n");
    fprintf(stderr, " * jobs\n");
    fprintf(stderr, " * nodes\n");
    fprintf(stderr, " * users\n");
    fprintf(stderr, " * acct\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Types in detail mode:\n");
    fprintf(stderr, " * partitions\n");
    fprintf(stderr, " * jobs\n");
    fprintf(stderr, " * nodes\n");
    fprintf(stderr, " * slurmd\n");
    fprintf(stderr, " * reservations\n");
    fprintf(stderr, " * assoc\n");
    exit(exit_code);
  }

  if (show_version ||
      (optind + 1 == argc && strcmp(argv[optind], "version") == 0))
  {
    fprintf(stderr, "Version: %s\n", SLURMINFO_VERSION);
    exit(0);
  }

  const char *type = argv[optind];

  if (type != NULL && strcmp(type, "acct") == 0)
  {
    pager_t less = auto_less();
    print_job_account_summary(less.out, job_id, show_user, start_time, end_time,
                              state);
    close_pager(&less);
    return 0;
  }

  if (detail_mode)
  {
    if (strcmp(type, "nodes") == 0)
    {
      print_nodes(stdout);
    }
    else if (strcmp(type, "partitions") == 0)
    {
      print_partitions(stdout);
    }
    else if (strcmp(type, "jobs") == 0)
    {
      print_jobs(stdout);
    }
    else if (strcmp(type, "slurmd") == 0)
    {
      print_slurmd(stdout);
    }
    else if (strcmp(type, "reservations") == 0)
    {
      print_reservations(stdout);
    }
    else if (strcmp(type, "assoc") == 0)
    {
      print_assoc(stdout);
    }
    else
    {
      fprintf(stderr, "Unknown type: %s\n", type);
      exit(1);
    }
  }
  else
  {
    char buf[2000];
    if (show_user == NULL)
    {
      uid_t uid = getuid();
      struct passwd pwd_buf;
      struct passwd *pwd_result = NULL;
      getpwuid_r(uid, &pwd_buf, buf, sizeof(buf), &pwd_result);
      if (pwd_result == NULL)
      {
        perror("Failed to get current username");
        exit(1);
      }
      show_user = pwd_result->pw_name;
    }

    if (type == NULL)
    {
      pager_t less = auto_less();
      print_summary(less.out, "all", show_user);
      close_pager(&less);
    }
    else
    {
      pager_t less = auto_less();
      print_summary(less.out, type, show_user);
      close_pager(&less);
    }
  }

  return 0;
}
