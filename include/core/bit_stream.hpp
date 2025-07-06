#ifndef BIT_STREAM_HPP
#define BIT_STREAM_HPP

#include <cstdint>
#include <vector>

class BitStream {
 public:
  BitStream() = default;
  ~BitStream() = default;

  // basic operations on bits
  void write_bit(bool bit);
  bool read_bit();

  // operations on bits
  void write_bits(uint32_t value, int count);
  uint32_t read_bits(int count);

  // operations on bytes
  void writeByte(uint8_t byte);
  uint8_t readByte();

  // management of the buffer
  std::vector<uint8_t> get_buffer() const;
  void load_from_buffer(const std::vector<uint8_t>& data);
  void clear();

  // information about the stream
  size_t size() const;  // size in bits
  bool empty() const;

 private:
  std::vector<uint8_t> buffer_;
  size_t bit_position_ = 0;   // current position in bits
  size_t read_position_ = 0;  // read position in bits

  // helper function to ensure enough capacity in the buffer
  void ensure_capacity(size_t additional_bits);
};

#endif
