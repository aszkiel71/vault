#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "core/bit_stream.hpp"
#include "core/frequency_analyzer.hpp"
#include "core/huffman_tree.hpp"

class Encoder {
 public:
  Encoder() = default;
  ~Encoder() = default;

  void compress(const std::string& input_file, const std::string& output_file);
  void decompress(const std::string& input_file,
                  const std::string& output_file);

 private:
  FrequencyAnalyzer frequency_analyzer_;
  HuffmanTree huffman_tree_;
  BitStream bit_stream_;

  void write_header(std::ofstream& output,
                    const std::map<uint8_t, uint64_t>& frequencies);

  std::map<uint8_t, uint64_t> read_header(std::ifstream& input);
};

#endif
