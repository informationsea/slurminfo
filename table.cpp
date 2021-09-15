#include "table.hpp"
#include "term.h"

#include "string.h"
#include "summary_common.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstddef>

const std::string NOT_FOUND;

void print_table(FILE *output,
                 const std::vector<std::vector<std::string>> table) {
  // fprintf(stderr, "table size: %zu\n", table.size());
  std::vector<size_t> table_width;
  for (auto it = table.begin(); it < table.end(); ++it) {
    for (size_t i = 0; i < it->size(); ++i) {
      // fprintf(stderr, "%zu %zu\n", i, (*it)[i].size());
      if (table_width.size() == i) {
        table_width.push_back((*it)[i].size());
      } else if (i < table_width.size()) {
        if (table_width[i] < (*it)[i].size()) {
          table_width[i] = (*it)[i].size();
        }
      }
    }
    // fprintf(stderr, "\n");
  }

  for (auto it = table.begin(); it < table.end(); ++it) {
    for (size_t i = 0; i < it->size(); ++i) {
      size_t l = (*it)[i].size();
      fprintf(output, "%s", (*it)[i].c_str());
      for (size_t j = l; j < table_width[i]; j++) {
        fprintf(output, " ");
      }
      if (i < table_width.size() - 1) {
        term_set_foreground_color(output, TERM_GREEN);
        fprintf(output, " | ");
        term_set_foreground_color(output, TERM_DEFAULT);
      }
    }
    fprintf(output, "\n");
  }
}
