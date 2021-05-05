#include "summary_common.hpp"

std::vector<std::string> split(const std::string &str, const char *delimiter) {
  size_t next_search = 0;
  size_t found_cammna = std::string::npos;
  std::vector<std::string> result;
  while ((found_cammna = str.find(delimiter, next_search)) !=
         std::string::npos) {
    std::string one_partition_name =
        str.substr(next_search, found_cammna - next_search);
    result.push_back(one_partition_name);
    next_search = found_cammna + 1;
  }
  std::string one_partition_name = str.substr(next_search);
  result.push_back(one_partition_name);
  return result;
}

Tres::Tres(const std::string &tres_str)
    : cpu(1), memory(0), node(1), billing(1), gpu(0) {
  auto entries = split(tres_str, ",");
  for (auto one_entry : entries) {
    if (one_entry.find("cpu=") == 0) {
      bool ok;
      long cpu = string2long(one_entry.substr(4).c_str(), &ok);
      if (ok) {
        this->cpu = cpu;
      }
    } else if (one_entry.find("gres/gpu=") == 0) {
      bool ok;
      long gpu = string2long(one_entry.substr(9).c_str(), &ok);
      if (ok) {
        this->gpu = gpu;
      }
    }
    if (one_entry.find("billing=") == 0) {
      bool ok;
      long billing = string2long(one_entry.substr(8).c_str(), &ok);
      if (ok) {
        this->billing = billing;
      }
    }
    if (one_entry.find("mem=") == 0) {
      long long multiple = 1;
      bool ok;
      if (one_entry.rfind("G") == one_entry.size() - 1) {
        multiple = 1024;
      } else if (one_entry.rfind("M") == one_entry.size() - 1) {
        multiple = 1;
      } else if (one_entry.rfind("T") == one_entry.size() - 1) {
        multiple = 1024LL * 1024;
      } else {
        continue;
      }

      double mem = string2double(
          one_entry.substr(4, one_entry.size() - 1 - 4).c_str(), &ok);
      if (ok) {
        this->memory = (uint64_t)(mem * multiple);
      } else {
        perror("failed to parse num");
      }
    }
  }
}
