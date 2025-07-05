#include <iostream>
#include <string>

#include "core/bit_stream.hpp"    // Adding include for BitStream
#include "core/huffman_tree.hpp"  // Adding include for HuffmanTree

struct Arguments {
  void printHelp() const {
    std::cout
        << "Usage: quickcompress [options]\n"
        << "Options:\n"
        << "  -c, --compress       Compress the input file\n"
        << "  -d, --decompress     Decompress the input file\n"
        << "  -i, --input <file>   Input file path\n"
        << "  -o, --output <file>  Output file path\n"
        << "  -v, --verbose        Enable verbose output\n"
        << "  -h, --help           Show this help message\n"
        << "  -t, --threads <num>  Number of threads to use (default: 1)\n";
  }
  // This struct will hold information about command line arguments:
  // If it is compression or decompression,
  // the input file, the output file, and any other options.
  bool isCompression = false;  // true for compression, false for decompression
  std::string inputFile;       // Input file path
  std::string outputFile;      // Output file path
  bool verbose = false;        // Verbose mode for detailed output
  bool help = false;           // Show help message
  int numThreads = 1;  // Number of threads to use for compression/decompression
};

Arguments parse_arguments(int argc, char* argv[]) {
  Arguments args;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-c" || arg == "--compress") {
      args.isCompression = true;
    } else if (arg == "-d" || arg == "--decompress") {
      args.isCompression = false;
    } else if (arg == "-i" || arg == "--input") {
      if (i + 1 < argc) {
        args.inputFile = argv[++i];
      } else {
        std::cerr << "Error: No input file specified.\n";
        args.help = true;
      }
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 < argc) {
        args.outputFile = argv[++i];
      } else {
        std::cerr << "Error: No output file specified.\n";
        args.help = true;
      }
    } else if (arg == "-v" || arg == "--verbose") {
      args.verbose = true;
    } else if (arg == "-h" || arg == "--help") {
      args.help = true;
    } else if (arg == "-t" || arg == "--threads") {
      if (i + 1 < argc) {
        args.numThreads = std::stoi(argv[++i]);
      } else {
        std::cerr << "Error: No number of threads specified.\n";
        args.help = true;
      }
    } else {
      std::cerr << "Error: Unknown argument '" << arg << "'\n";
      args.help = true;
    }
  }

  return args;
}

int main(int argc, char* argv[]) {
  std::cout << "Hello World from QuickCompress!\n";

  // Test BitStream
  std::cout << "\n=== BitStream Test ===\n";
  BitStream bs;

  // Test write_bit
  bs.write_bit(true);
  bs.write_bit(false);
  bs.write_bit(true);
  bs.write_bit(true);

  // Test write_bits
  bs.write_bits(0xA5, 8);  // 10100101

  std::cout << "BitStream size: " << bs.size() << " bits\n";

  // Test get_buffer
  auto buffer = bs.get_buffer();
  std::cout << "Buffer contents: ";
  for (auto byte : buffer) {
    std::cout << std::hex << "0x" << static_cast<int>(byte) << " ";
  }
  std::cout << std::dec << "\n";

  // Test HuffmanTree edge case
  std::cout << "\n=== HuffmanTree Edge Case Test ===\n";
  HuffmanTree huffman;

  // Test single character case (edge case)
  std::map<uint8_t, uint64_t> single_char_freq;
  single_char_freq[65] = 100;  // 'A' appears 100 times

  try {
    huffman.build_tree(single_char_freq);
    auto codes = huffman.generate_codes();
    std::cout << "Single character test successful!\n";
    std::cout << "Character 'A' (65) gets code: '" << codes[65] << "'\n";
  } catch (const std::exception& e) {
    std::cout << "Single character test failed: " << e.what() << "\n";
  }

  // Test multiple characters case (normal case)
  std::map<uint8_t, uint64_t> multi_char_freq;
  multi_char_freq[65] = 10;  // 'A'
  multi_char_freq[66] = 20;  // 'B'
  multi_char_freq[67] = 30;  // 'C'

  try {
    huffman.build_tree(multi_char_freq);
    auto codes = huffman.generate_codes();
    std::cout << "Multiple character test successful!\n";
    std::cout << "Generated codes:\n";
    for (const auto& pair : codes) {
      std::cout << "  Character " << static_cast<char>(pair.first) << " ("
                << static_cast<int>(pair.first) << "): '" << pair.second
                << "'\n";
    }
  } catch (const std::exception& e) {
    std::cout << "Multiple character test failed: " << e.what() << "\n";
  }

  // Parse command line arguments
  Arguments args = parse_arguments(argc, argv);

  // If help is needed, display it
  if (args.help) {
    args.printHelp();
    return 0;
  }

  // Show parsed arguments
  std::cout << "\n=== Parsed Arguments ===\n";
  std::cout << "Mode: "
            << (args.isCompression ? "Compression" : "Decompression") << "\n";
  std::cout << "Input file: "
            << (args.inputFile.empty() ? "<not specified>" : args.inputFile)
            << "\n";
  std::cout << "Output file: "
            << (args.outputFile.empty() ? "<not specified>" : args.outputFile)
            << "\n";
  std::cout << "Threads: " << args.numThreads << "\n";
  std::cout << "Verbose: " << (args.verbose ? "enabled" : "disabled") << "\n";

  // Example logic based on arguments
  if (args.inputFile.empty()) {
    std::cout << "\nWarning: No input file specified!\n";
  } else {
    if (args.isCompression) {
      std::cout << "\nTODO: Compress file '" << args.inputFile << "'";
      if (!args.outputFile.empty()) {
        std::cout << " -> '" << args.outputFile << "'";
      }
      std::cout << "\n";
    } else {
      std::cout << "\nTODO: Decompress file '" << args.inputFile << "'";
      if (!args.outputFile.empty()) {
        std::cout << " -> '" << args.outputFile << "'";
      }
      std::cout << "\n";
    }
  }

  if (args.verbose) {
    std::cout << "\n=== Verbose Info ===\n";
    std::cout << "Program name: " << argv[0] << "\n";
    std::cout << "Total arguments: " << argc << "\n";
    std::cout << "Raw arguments:\n";
    for (int i = 1; i < argc; ++i) {
      std::cout << "  " << i << ": " << argv[i] << "\n";
    }
  }

  return 0;
}
