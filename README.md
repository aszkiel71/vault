# QuickCompress - Huffman File Compression Tool

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

**Clean C++17 implementation of Huffman coding algorithm with command-line interface and real-time progress tracking.**

## 🚀 Features

- **Lossless Compression** - Huffman coding algorithm implementation
- **Command Line Interface** - Full-featured CLI with argument parsing
- **Real-time Progress** - Visual progress bars with compression speed
- **Memory Efficient** - Optimized bit-stream operations
- **Cross-platform** - Works on Linux, macOS, and Windows
- **Error Handling** - Comprehensive validation and error reporting

## 📊 Performance Results

Real-world compression ratios achieved by QuickCompress:

| File Type | Original Size | Compressed Size | Compression Ratio | Verdict |
|-----------|---------------|-----------------|-------------------|---------|
| **Repetitive Patterns** | 6,060 bytes | 2,031 bytes | **+66.5%** | 🔥 Excellent |
| **Large Text Files** | 127,703 bytes | 82,997 bytes | **+35.0%** | ✅ Very Good |
| **Source Code (C++)** | 6,511 bytes | 4,603 bytes | **+29.3%** | ✅ Good |
| **Small Text Files** | 1,224 bytes | 1,662 bytes | -35.8% | ⚠️ Not Recommended |
| **Random Binary Data** | 50,000 bytes | 52,308 bytes | -4.6% | ⚠️ Not Recommended |

### Key Insights:
- **Best performance**: Files with repetitive patterns (up to 66% reduction)
- **Good performance**: Large text files and source code (29-35% reduction)
- **Poor performance**: Small files and random data (may increase size)
- **Overhead**: Header information affects small files disproportionately

## 🛠️ Requirements

- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.16** or higher
- **Git** for cloning the repository and managing submodules
- **Internet connection** for downloading dependencies (indicators library)

## 📦 Installation

### Windows (PowerShell)
```powershell
# Clone the repository
git clone https://github.com/aszkiel71/Vault.git
cd Vault

# Initialize and update submodules
git submodule update --init --recursive

# Configure with CMake (Debug mode)
cmake -S . -B build

# Compile
cmake --build build

# For Release build (faster execution)
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

# Test installation
.\build\Debug\quickcompress.exe --help
# or for Release:
# .\build-release\Release\quickcompress.exe --help
```

### Linux/macOS
```bash
# Clone the repository
git clone https://github.com/aszkiel71/Vault.git
cd Vault

# Initialize and update submodules
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Compile
make -j$(nproc)

# Test installation
./quickcompress --help
```

## 🎯 Usage

### Basic Commands

### Windows
```powershell
# Compress a file
.\build\Debug\quickcompress.exe -c -i document.txt -o document.qcmp

# Decompress a file
.\build\Debug\quickcompress.exe -d -i document.qcmp -o restored.txt

# With verbose progress tracking
.\build\Debug\quickcompress.exe -c -i large_file.txt -o compressed.qcmp -v
```

### Linux/macOS
```bash
# Compress a file
./quickcompress -c -i document.txt -o document.qcmp

# Decompress a file
./quickcompress -d -i document.qcmp -o restored.txt

# With verbose progress tracking
./quickcompress -c -i large_file.txt -o compressed.qcmp -v
```

### Command Line Options

```
Usage: quickcompress [options]

Options:
  -c, --compress       Compress the input file
  -d, --decompress     Decompress the input file
  -i, --input <file>   Input file path
  -o, --output <file>  Output file path
  -v, --verbose        Enable verbose output with additional information
  -h, --help           Show this help message
  -t, --threads <num>  Number of threads (reserved for future use)
```

### Example Session (Windows)

```powershell
PS C:\Vault> .\build\Debug\quickcompress.exe -c -i example.txt -o example.qcmp -v

Hello World from QuickCompress!
=== Parsed Arguments ===
Mode: Compression
Input file: example.txt
Output file: example.qcmp
Verbose: enabled

Compressing file 'example.txt' -> 'example.qcmp'
[==================================================] Compressing file
Compression completed successfully!

PS C:\Vault> Get-ChildItem example.*

Name            Length
----            ------
example.txt       6511
example.qcmp      4603

# Achieved 29.3% size reduction!
```

## 🏗️ Architecture

QuickCompress follows a clean, modular design:

```
QuickCompress/
├── include/core/
│   ├── bit_stream.hpp          # Bit-level I/O operations
│   ├── encoder.hpp             # Main compression orchestrator
│   ├── frequency_analyzer.hpp  # Byte frequency analysis
│   └── huffman_tree.hpp        # Huffman tree construction
├── src/core/
│   ├── bit_stream.cpp
│   ├── encoder.cpp
│   ├── frequency_analyzer.cpp
│   └── huffman_tree.cpp
├── src/main.cpp                # CLI interface & argument parsing
├── external/indicators/        # Progress bar library (submodule)
├── CMakeLists.txt              # Build configuration
└── README.md                   # This file
```

### Core Components

**BitStream** (`bit_stream.hpp/.cpp`)
- Efficient bit-level read/write operations
- Memory-optimized buffer management
- Support for non-byte-aligned data

**FrequencyAnalyzer** (`frequency_analyzer.hpp/.cpp`)
- Analyzes byte frequency distribution in input files
- Optimized for large file processing with buffered I/O

**HuffmanTree** (`huffman_tree.hpp/.cpp`)
- Builds optimal Huffman trees using priority queues
- Generates variable-length prefix codes
- Handles edge cases (single character files)

**Encoder** (`encoder.hpp/.cpp`)
- Orchestrates the entire compression/decompression process
- Custom file format with frequency header
- Error handling and validation

**Main** (`main.cpp`)
- Complete CLI argument parsing
- User interface and help system
- Built-in testing capabilities

## 🔬 Algorithm Details

**Huffman Coding** assigns variable-length codes to characters based on their frequency - frequent characters get shorter codes.

### Compression Process:
1. **Frequency Analysis** - Count occurrence of each byte (0-255)
2. **Tree Construction** - Build binary tree using priority queue
3. **Code Generation** - Generate prefix codes via tree traversal
4. **Encoding** - Replace each byte with its Huffman code
5. **Output** - Write header + bit-packed compressed data

### File Format (.qcmp):
```
Header:
├── Number of unique characters (4 bytes)
└── For each character:
    ├── Byte value (1 byte)
    └── Frequency count (8 bytes)

Compressed Data:
└── Huffman-encoded bit stream
```

## 🧪 Testing

Verify the implementation works correctly:

### Windows Testing
```powershell
# Create test file
echo "Hello World! This is a test for QuickCompress algorithm." > test.txt

# Compress
.\build\Debug\quickcompress.exe -c -i test.txt -o test.qcmp -v

# Decompress
.\build\Debug\quickcompress.exe -d -i test.qcmp -o restored.txt -v

# Verify integrity (should be identical)
Compare-Object (Get-Content test.txt) (Get-Content restored.txt)
# Should return empty if files are identical

# Check file sizes
Get-ChildItem test.txt, test.qcmp, restored.txt | Select-Object Name, Length
```

### Linux/macOS Testing
```bash
# Create test file
echo "Hello World! This is a test for QuickCompress algorithm." > test.txt

# Compress
./quickcompress -c -i test.txt -o test.qcmp -v

# Decompress
./quickcompress -d -i test.qcmp -o restored.txt -v

# Verify integrity (should be identical)
diff test.txt restored.txt
echo $? # Should output 0

# Test with different file types
./quickcompress -c -i /usr/share/dict/words -o words.qcmp -v
```



## 📈 Performance Notes

- **Optimal for**: Text files, source code, repetitive data
- **Memory usage**: Approximately 2x input file size during processing
- **Speed**: Primarily I/O bound for large files
- **Minimum overhead**: ~20 bytes header + frequency table

## 🤝 Contributing

Contributions are welcome! Areas for improvement:
- Performance optimizations
- Additional compression algorithms
- Cross-platform testing
- Documentation improvements

## 📄 License

This project is licensed under the MIT License.

## 👨‍💻 Author

**aszkiel71**
---

*A clean, educational implementation of Huffman coding in modern C++17*
