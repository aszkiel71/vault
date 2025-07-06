#include "core/encoder.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

void Encoder::write_header(std::ofstream& output,
                           const std::map<uint8_t, uint64_t>& frequencies) {
  if (!output.is_open()) {
    throw std::runtime_error("Output file is not open");
  }

  // Write the number of unique characters
  uint32_t num_unique_chars = frequencies.size();
  output.write(reinterpret_cast<const char*>(&num_unique_chars),
               sizeof(num_unique_chars));

  // Write each character and its frequency
  for (const auto& pair : frequencies) {
    output.write(reinterpret_cast<const char*>(&pair.first),
                 sizeof(pair.first));
    output.write(reinterpret_cast<const char*>(&pair.second),
                 sizeof(pair.second));
  }
}

std::map<uint8_t, uint64_t> Encoder::read_header(std::ifstream& input) {
  if (!input.is_open()) {
    throw std::runtime_error("Input file is not open");
  }

  std::map<uint8_t, uint64_t> frequencies;

  // Read the number of unique characters
  uint32_t num_unique_chars;
  input.read(reinterpret_cast<char*>(&num_unique_chars),
             sizeof(num_unique_chars));

  // Read each character and its frequency
  for (uint32_t i = 0; i < num_unique_chars; ++i) {
    uint8_t byte;
    uint64_t frequency;
    input.read(reinterpret_cast<char*>(&byte), sizeof(byte));
    input.read(reinterpret_cast<char*>(&frequency), sizeof(frequency));
    frequencies[byte] = frequency;
  }

  return frequencies;
}

void Encoder::compress(const std::string& input_file,
                       const std::string& output_file) {
  // Analyze the file to get character frequencies
  auto frequencies = frequency_analyzer_.analyze_file(input_file);
  if (frequencies.empty()) {
    throw std::runtime_error("Input file is empty or could not be read");
  }

  // Build the Huffman tree
  huffman_tree_.build_tree(frequencies);

  // Open the output file
  std::ofstream output(output_file, std::ios::binary);
  if (!output.is_open()) {
    throw std::runtime_error("Could not open output file: " + output_file);
  }

  // Write the header with frequencies
  write_header(output, frequencies);
  auto codes = huffman_tree_.generate_codes();
  std::ifstream input(input_file, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("Could not open input file: " + input_file);
  }
  bit_stream_.clear();  // Clear the bit stream before writing
  uint8_t byte;
  while (input.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
    // For each byte read, get its Huffman code and write it to the bit stream
    auto it = codes.find(byte); 
    if (it != codes.end()) {
      const std::string& code = it->second;
      for (char c : code) {
        bit_stream_.write_bit(c == '1');
      }
    } else {
      throw std::runtime_error("Byte not found in Huffman codes: " +
                               std::to_string(byte));
    }
  }

  auto compressed_data = bit_stream_.get_buffer();
  output.write(reinterpret_cast<const char*>(compressed_data.data()),
               compressed_data.size());
}

void Encoder::decompress(const std::string& input_file,
                         const std::string& output_file) {
  std::ifstream input(input_file, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("Could not open input file: " + input_file);
  }
  auto frequencies = read_header(input);
  if (frequencies.empty()) {
    throw std::runtime_error("Input file is empty or could not be read");
  }
  // Build the Huffman tree
  huffman_tree_.build_tree(frequencies);

  // Open the output file
  std::ofstream output(output_file, std::ios::binary);
  if (!output.is_open()) {
    throw std::runtime_error("Could not open output file: " + output_file);
  }

  // 1. Read the rest of the file (compressed data) into bit_stream_
  std::vector<uint8_t> compressed_data;
  // Read everything that's left in the input file
  input.seekg(0, std::ios::end);
  std::streampos end_pos = input.tellg();
  input.seekg(0, std::ios::beg);

  // Skip the header we already read
  read_header(input);  // This will advance the file position past the header

  std::streampos current_pos = input.tellg();
  size_t compressed_size = end_pos - current_pos;

  if (compressed_size > 0) {
    compressed_data.resize(compressed_size);
    input.read(reinterpret_cast<char*>(compressed_data.data()),
               compressed_size);
  }

  // 2. Load into bit_stream_
  bit_stream_.load_from_buffer(compressed_data);

  // 3. Decoding bit by bit:
  // Calculate the total number of bytes we need to decode
  uint64_t total_bytes = 0;
  for (const auto& pair : frequencies) {
    total_bytes += pair.second;
  }

  // Decode the specified number of bytes
  for (uint64_t i = 0; i < total_bytes; ++i) {
    try {
      uint8_t decoded_byte = huffman_tree_.decode_byte(bit_stream_);
      output.write(reinterpret_cast<const char*>(&decoded_byte),
                   sizeof(decoded_byte));
    } catch (const std::exception& e) {
      // If we can't decode more bytes, we've reached the end
      break;
    }
  }
}
