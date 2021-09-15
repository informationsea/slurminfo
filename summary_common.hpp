#ifndef _SUMMARY_COMMON_HPP
#define _SUMMARY_COMMON_HPP

#include "common.h"
#include <cstdint>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &str, const char *delimiter);
uint64_t parse_size(const std::string &str);
std::string size_to_string(uint64_t size);

class Tres {
public:
  uint32_t cpu;
  uint64_t memory;
  uint32_t node;
  uint32_t billing;
  uint32_t gpu;

  Tres(const std::string &tres_str);
};

#endif /* _SUMMARY_COMMON_HPP */
