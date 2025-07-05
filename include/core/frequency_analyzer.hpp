#ifndef FREQUENCY_ANALYZER_HPP
#define FREQUENCY_ANALYZER_HPP

#include <cstdint>
#include <map>
#include <string>

class FrequencyAnalyzer {
 public:
  FrequencyAnalyzer() = default;
  ~FrequencyAnalyzer() = default;

  std::map<uint8_t, uint64_t> analyze_file(const std::string& file_name) const;
};

#endif
