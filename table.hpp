#ifndef _TABLE_HPP
#define _TABLE_HPP

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <string.h>

#include "common.h"

void print_table(FILE *output,
                 const std::vector<std::vector<std::string>> table);

extern const std::string NOT_FOUND;

class HeaderTable {
public:
  std::vector<std::string> header;
  std::map<std::string, size_t> header_to_index;
  std::vector<std::vector<std::string>> data;

  template <typename PredicateT> HeaderTable(FILE *reader, PredicateT Pred) {
    char buffer[1024 * 10];
    std::vector<std::string> row;
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, sizeof(buffer) - 1, reader) == NULL) {
      return;
    }
    if (buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }

    boost::algorithm::split(row, buffer, Pred);
    header = row;

    for (size_t i = 0; i < row.size(); i++) {
      header_to_index[row[i]] = i;
    }

    while (1) {
      memset(buffer, 0, sizeof(buffer));

      if (fgets(buffer, sizeof(buffer) - 1, reader) == NULL) {
        break;
      }

      if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
      }

      if (buffer[strlen(buffer) - 1] == '\r') {
        buffer[strlen(buffer) - 1] = '\0';
      }

      boost::algorithm::split(row, buffer, Pred);
      data.push_back(row);
    }
  }

  template <typename PredicateT>
  HeaderTable(const std::string &input_data, PredicateT Pred) {
    // INFO("header parse");
    std::vector<std::string> lines;
    boost::algorithm::split(lines, input_data, boost::is_any_of("\n"));
    if (lines.empty()) {
      // INFO("empty");
      return;
    }

    // INFOF("first line: %s\n", lines[0].c_str());
    std::vector<std::string> row;
    boost::algorithm::split(row, lines[0], Pred);
    header = row;

    for (size_t i = 0; i < row.size(); i++) {
      header_to_index[row[i]] = i;
    }

    for (auto it = lines.begin() + 1; it != lines.end(); ++it) {
      if (it->length() > 0) {
        boost::algorithm::split(row, *it, Pred);
        data.push_back(row);
      }
    }
  }

  size_t row_count() const { return data.size(); }

  const std::string &get(const std::string &column, size_t row) const {
    auto key_index = header_to_index.find(column);
    if (key_index == header_to_index.end()) {
      return NOT_FOUND;
    }

    if (data.size() <= row) {
      return NOT_FOUND;
    }

    if (data[row].size() <= key_index->second) {
      return NOT_FOUND;
    }

    return data[row][key_index->second];
  }
};

#endif
