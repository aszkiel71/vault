#include "core/frequency_analyzer.hpp"

#include <fstream>
#include <map>
#include <stdexcept>

std::map<uint8_t, uint64_t> FrequencyAnalyzer::analyze_file(
    const std::string& file_name) const {
  std::map<uint8_t, uint64_t> frequency_map;
  std::ifstream file(file_name, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + file_name);
  }

  char buffer[4096];
  while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
    for (int i = 0; i < file.gcount(); ++i) {
      frequency_map[static_cast<uint8_t>(buffer[i])]++;
    }
  }

  return frequency_map;
}
