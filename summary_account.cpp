#include "summary_account.hpp"
#include "common.h"
#include "spawn.h"
#include "summary_common.hpp"
#include "table.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <cstring>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <vector>

class JobResult {
public:
  std::string job_id;
  std::string job_name;
  std::string partition;
  std::string node;
  std::string user;
  std::string account;
  std::string state;
  std::string exit_code;
  std::string submit;
  std::string start;
  std::string end;
  std::string max_disk_read;
  std::string max_disk_write;
  std::string max_rss;
  std::string max_vmem;
  std::string node_list;
  std::string alloc_tres;
  uint64_t n_tasks;
  uint64_t alloc_cpus;
  double user_cpu_time;
  double system_cpu_time;
  double elapsed_time;
};

double parse_time(const std::string &str) {
  std::vector<std::string> day_hour;
  boost::algorithm::split(day_hour, str, boost::algorithm::is_any_of("-"));

  std::vector<std::string> elements;
  boost::algorithm::split(elements, *day_hour.rbegin(),
                          boost::algorithm::is_any_of(":"));

  double parsed_time = 0.0;

  std::vector<double> num_elements;
  auto one_element = elements.rbegin();

  // second
  if (one_element != elements.rend()) {
    parsed_time = boost::lexical_cast<double>(*one_element);
    ++one_element;
  }

  // min
  if (one_element != elements.rend()) {
    parsed_time += boost::lexical_cast<double>(*one_element) * 60;
    ++one_element;
  }

  // hour
  if (one_element != elements.rend()) {
    parsed_time += boost::lexical_cast<double>(*one_element) * 3600;
    ++one_element;
  }

  // day
  if (day_hour.size() == 2) {
    parsed_time += boost::lexical_cast<double>(day_hour[0]) * 3600 * 24;
  }

  return parsed_time;
}

void print_job_account_summary(FILE *file, const char *job_id,
                               const char *start_time, const char *end_time) {
  int args_len = 5;
  if (job_id != NULL) {
    args_len += 2;
  }
  if (start_time != NULL) {
    args_len += 2;
  }
  if (end_time != NULL) {
    args_len += 2;
  }

  char **args = (char **)malloc(args_len * sizeof(char *));

  size_t current_pos = 0;

  args[current_pos++] = strdup("sacct");
  args[current_pos++] = strdup("--format");
  args[current_pos++] =
      strdup("JobID,JobName,Partition,User,Account,AllocCPUS,State,"
             "Elapsed,SystemCPU,UserCPU,ExitCode,Submit,Start,End,ReqMem,"
             "Reason,NodeList,NTasks,AllocTres,"
             "MaxDiskRead,MaxDiskWrite,MaxRSS,MaxVMSize");
  args[current_pos++] = strdup("-P");

  if (job_id != NULL) {
    args[current_pos++] = strdup("-j");
    args[current_pos++] = strdup(job_id);
  }
  if (start_time != NULL) {
    args[current_pos++] = strdup("--starttime");
    args[current_pos++] = strdup(start_time);
  }
  if (end_time != NULL) {
    args[current_pos++] = strdup("--endtime");
    args[current_pos++] = strdup(end_time);
  }
  args[current_pos] = NULL;

  fprintf(stderr, "$");
  for (size_t i = 0; args[i] != NULL; i++) {
    fprintf(stderr, " %s", args[i]);
  }
  fprintf(stderr, "\n");

  child_process_t process = spawn_pipe("sacct", args, SPAWN_PIPE_STDOUT, true);
  if (process.pid < 0) {
    perror("Failed to start sacct");
    exit(1);
  }

  FILE *sacct_result = fdopen(process.fd_stdout, "rt");
  int process_result;
  std::string result_str;
  {
    char buf[1024 * 100];
    std::memset(buf, 0, sizeof(buf));
    size_t read_bytes;
    while ((read_bytes = fread(buf, 1, sizeof(buf) - 1, sacct_result)) ||
           (waitpid(process.pid, &process_result, WNOHANG) == 0)) {
      result_str += buf;
      std::memset(buf, 0, sizeof(buf));
    }
  }

  // fprintf(stderr, "sacct result: \"%s\"\n", result_str.c_str());

  HeaderTable table(result_str, boost::algorithm::is_any_of("|"));

  if (process_result != 0) {
    ERROR("sacct is not succeeded\n");
    exit(1);
  }

  std::vector<JobResult> job_results;
  JobResult current_job_result;

  fprintf(stderr, "loading data\n");

  // INFOF("Data count: %zu", table.data.size());

  fclose(sacct_result);

  if (table.header_to_index.find("JobID") == table.header_to_index.end()) {
    ERROR("Unexpected sacct result");
    exit(1);
  }

  for (size_t i = 0; i < table.row_count(); i++) {
    std::vector<std::string> job_id_elements;
    boost::algorithm::split(job_id_elements, table.get("JobID", i),
                            boost::algorithm::is_any_of("."));

    if (current_job_result.job_id != "" &&
        current_job_result.job_id != job_id_elements[0]) {
      job_results.push_back(current_job_result);
      current_job_result = JobResult();
    }

    if (job_id_elements.size() == 1) {
      current_job_result.job_id = table.get("JobID", i);
      current_job_result.job_name = table.get("JobName", i);
      current_job_result.partition = table.get("Partition", i);
      current_job_result.user = table.get("User", i);
      current_job_result.state = table.get("State", i);
      current_job_result.submit = table.get("Submit", i);
      current_job_result.start = table.get("Start", i);
      current_job_result.end = table.get("End", i);
      current_job_result.node_list = table.get("NodeList", i);

      try {
        current_job_result.alloc_cpus =
            boost::lexical_cast<uint64_t>(table.get("AllocCPUS", i));
      } catch (const boost::bad_lexical_cast &err) {
        ERRORF("Failed to parse AllocCPUS in row %zu", i);
        exit(1);
      }

      try {
        current_job_result.elapsed_time = parse_time(table.get("Elapsed", i));
        // fprintf(stderr, "Parsing: %s -> %f\n", table.get("Elapsed",
        // i).c_str(),
        //        current_job_result.elapsed_time);
      } catch (const boost::bad_lexical_cast &err) {
        ERRORF("Failed to parse ElapseRaw in row %zu", i);
        exit(1);
      }

      try {
        current_job_result.user_cpu_time = parse_time(table.get("UserCPU", i));
        // fprintf(stderr, "Parsing: %s -> %f\n", table.get("UserCPU",
        // i).c_str(),
        //        current_job_result.user_cpu_time);
      } catch (const boost::bad_lexical_cast &err) {
        ERRORF("Failed to parse CPUTimeRaw in row %zu", i);
        exit(1);
      }

      try {
        current_job_result.system_cpu_time =
            parse_time(table.get("SystemCPU", i));
      } catch (const boost::bad_lexical_cast &err) {
        ERRORF("Failed to parse CPUTimeRaw in row %zu", i);
        exit(1);
      }

    } else if (job_id_elements.size() == 2 && job_id_elements[1] == "batch") {
      current_job_result.exit_code = table.get("Exit Code", i);
      current_job_result.max_disk_read = table.get("MaxDiskRead", i);
      current_job_result.max_disk_write = table.get("MaxDiskWrite", i);
      current_job_result.max_rss = table.get("MaxRSS", i);
      current_job_result.max_vmem = table.get("MaxVMSize", i);
      current_job_result.alloc_tres = table.get("AllocTRES", i);

      try {
        current_job_result.n_tasks =
            boost::lexical_cast<uint64_t>(table.get("NTasks", i));
      } catch (const boost::bad_lexical_cast &err) {
        ERRORF("Failed to parse NTasks in row %zu", i);
        exit(1);
      }
    }
  }

  std::vector<std::vector<std::string>> table_data;
  table_data.push_back({"Job ID", "Job Name", "State", "Partition", "User", "Submission",
                        "Start", "End", "Elapsed time", "CPU", "CPU Time",
                        "CPU %", "Sys CPU %", "Max Disk Read", "Max Disk Write",
                        "Max RSS", "Max VMSize", "Alloc Mem", "RSS %",
                        "VMSize %"});
  for (auto one_job : job_results) {
    char elapsed_time[256];
    sprint_diff_time(elapsed_time, sizeof(elapsed_time) - 1,
                     one_job.elapsed_time);

    char cpu_time[256];
    sprint_diff_time(cpu_time, sizeof(cpu_time) - 1,
                     one_job.system_cpu_time + one_job.user_cpu_time);

    double cpu_ratio = (one_job.system_cpu_time + one_job.user_cpu_time) /
                       (one_job.elapsed_time) / ((double)one_job.alloc_cpus);
    char cpu_ratio_str[256];
    std::snprintf(cpu_ratio_str, sizeof(cpu_ratio_str) - 1, "%0.1f%%",
                  cpu_ratio * 100);

    char sys_cpu_ratio_str[256];
    std::snprintf(sys_cpu_ratio_str, sizeof(sys_cpu_ratio_str) - 1, "%0.1f%%",
                  (one_job.system_cpu_time) /
                      (one_job.system_cpu_time + one_job.user_cpu_time) * 100);

    Tres alloc_tres(one_job.alloc_tres);

    uint64_t max_rss;
    uint64_t max_vmem;
    uint64_t max_disk_read;
    uint64_t max_disk_write;
    try {
      max_rss = parse_size(one_job.max_rss);
      max_vmem = parse_size(one_job.max_vmem);
      max_disk_read = parse_size(one_job.max_disk_read);
      max_disk_write = parse_size(one_job.max_disk_write);
    } catch (const boost::bad_lexical_cast &err) {
      ERRORF("Failed to parse MaxRSS/MaxVMSize in JobID %s | %s %s",
             one_job.job_id.c_str(), one_job.max_rss.c_str(),
             one_job.max_vmem.c_str());
      exit(1);
    }
    uint64_t alloc_mem = alloc_tres.memory * 1024 * 1024;

    char rss_ratio[200];
    snprintf(rss_ratio, sizeof(rss_ratio) - 1, "%0.1f%%",
             ((double)max_rss) / alloc_mem * 100);
    char vmem_ratio[200];
    snprintf(vmem_ratio, sizeof(vmem_ratio) - 1, "%0.1f%%",
             ((double)max_vmem) / alloc_mem * 100);

    table_data.push_back({one_job.job_id,
                          one_job.job_name,
                          one_job.state,
                          one_job.partition,
                          one_job.user,
                          one_job.submit,
                          one_job.start,
                          one_job.end,
                          elapsed_time,
                          boost::lexical_cast<std::string>(one_job.alloc_cpus),
                          cpu_time,
                          cpu_ratio_str,
                          sys_cpu_ratio_str,
                          size_to_string(max_disk_read),
                          size_to_string(max_disk_write),
                          size_to_string(max_rss),
                          size_to_string(max_vmem),
                          size_to_string(alloc_mem),
                          rss_ratio,
                          vmem_ratio,
      });
  }

  print_table(file, table_data);
}
