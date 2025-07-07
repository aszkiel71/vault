#include "core/encoder.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "indicators/progress_bar.hpp"

void Encoder::write_header(std::ofstream& output,
                           const std::map<uint8_t, uint64_t>& frequencies) {
  if (!output.is_open()) {
    throw std::runtime_error("Output file is not open");
  }

  // Write the number of unique characters
  uint32_t num_unique_chars = static_cast<uint32_t>(frequencies.size());
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
  // 1. Analyze frequencies
  auto frequencies = frequency_analyzer_.analyze_file(input_file);
  if (frequencies.empty()) {
    // Handle empty file: create an empty output file.
    std::ofstream output(output_file, std::ios::binary);
    output.close();
    return;
  }

  // 2. Build Huffman tree
  huffman_tree_.build_tree(frequencies);
  auto codes = huffman_tree_.generate_codes();

  // 3. Open files
  std::ifstream input(input_file, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("Could not open input file: " + input_file);
  }

  std::ofstream output(output_file, std::ios::binary);
  if (!output.is_open()) {
    throw std::runtime_error("Could not open output file: " + output_file);
  }

  // 4. Write header
  write_header(output, frequencies);

  // 5. Read and compress file byte by byte
  input.seekg(0, std::ios::end);
  size_t file_size = input.tellg();
  input.seekg(0, std::ios::beg);

  indicators::ProgressBar bar{
      indicators::option::BarWidth{50},
      indicators::option::Start{"["},
      indicators::option::Fill{"="},
      indicators::option::Lead{">"},
      indicators::option::Remainder{" "},
      indicators::option::End{"]"},
      indicators::option::PostfixText{"Compressing file"},
      indicators::option::ForegroundColor{indicators::Color::green},
      indicators::option::FontStyles{
          std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

  bit_stream_.clear();
  size_t processed_bytes = 0;

  char byte;
  while (input.get(byte)) {
    uint8_t ubyte = static_cast<uint8_t>(byte);
    auto it = codes.find(ubyte);
    if (it != codes.end()) {
      const std::string& code = it->second;
      for (char c : code) {
        bit_stream_.write_bit(c == '1');
      }
    } else {
      throw std::runtime_error("Byte not found in Huffman codes: " +
                               std::to_string(ubyte));
    }

    processed_bytes++;
    if (processed_bytes % 1000 == 0) {
      bar.set_progress((processed_bytes * 100) / file_size);
    }
  }

  bar.set_progress(100);

  // 6. Write compressed data
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

  // Check if the file is empty
  input.seekg(0, std::ios::end);
  if (input.tellg() == 0) {
    std::ofstream output(output_file, std::ios::binary);
    output.close();
    return;
  }
  input.seekg(0, std::ios::beg);

  // 1. Read header and build Huffman tree
  auto frequencies = read_header(input);
  huffman_tree_.build_tree(frequencies);

  // 2. Read compressed data
  std::vector<uint8_t> compressed_data;
  size_t current_pos = input.tellg();
  input.seekg(0, std::ios::end);
  size_t end_pos = input.tellg();
  input.seekg(current_pos, std::ios::beg);

  compressed_data.resize(end_pos - current_pos);
  input.read(reinterpret_cast<char*>(compressed_data.data()),
             compressed_data.size());
  input.close();

  // 3. Decompress data
  std::ofstream output(output_file, std::ios::binary);
  if (!output.is_open()) {
    throw std::runtime_error("Could not open output file: " + output_file);
  }

  bit_stream_.clear();
  bit_stream_.load_from_buffer(compressed_data);

  indicators::ProgressBar bar{
      indicators::option::BarWidth{50},
      indicators::option::Start{"["},
      indicators::option::Fill{"="},
      indicators::option::Lead{">"},
      indicators::option::Remainder{" "},
      indicators::option::End{"]"},
      indicators::option::PostfixText{"Decompressing file"},
      indicators::option::ForegroundColor{indicators::Color::yellow},
      indicators::option::FontStyles{
          std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

  // Calculate total original size from frequencies
  size_t total_original_size = 0;
  for (const auto& freq : frequencies) {
    total_original_size += freq.second;
  }

  size_t processed_bytes = 0;
  try {
    while (processed_bytes < total_original_size) {
      uint8_t decoded_byte = huffman_tree_.decode_byte(bit_stream_);
      output.write(reinterpret_cast<const char*>(&decoded_byte), 1);
      processed_bytes++;

      if (processed_bytes % 1000 == 0) {
        bar.set_progress((processed_bytes * 100) / total_original_size);
      }
    }
  } catch (const std::runtime_error& e) {
    throw std::runtime_error("Failed to decompress file: " +
                             std::string(e.what()));
  }

  bar.set_progress(100);
}
