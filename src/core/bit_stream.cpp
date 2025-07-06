#include "core/bit_stream.hpp"

#include <stdexcept>

void BitStream::write_bit(bool bit) {
  ensure_capacity(1);

  size_t byte_index = bit_position_ / 8;
  size_t bit_index = bit_position_ % 8;

  if (bit) {
    buffer_[byte_index] |= (1 << (7 - bit_index));
  } else {
    buffer_[byte_index] &= ~(1 << (7 - bit_index));
  }

  bit_position_++;
}

bool BitStream::read_bit() {
  if (read_position_ >= bit_position_) {
    throw std::runtime_error("BitStream: End of stream reached");
  }

  size_t byte_index = read_position_ / 8;
  size_t bit_index = read_position_ % 8;

  bool bit = (buffer_[byte_index] >> (7 - bit_index)) & 1;
  read_position_++;

  return bit;
}

void BitStream::write_bits(uint32_t value, int count) {
  if (count < 0 || count > 32) {
    throw std::invalid_argument("BitStream: Count must be between 0 and 32");
  }

  for (int i = count - 1; i >= 0; --i) {
    write_bit((value >> i) & 1);
  }
}

uint32_t BitStream::read_bits(int count) {
  if (count < 0 || count > 32) {
    throw std::invalid_argument("BitStream: Count must be between 0 and 32");
  }

  uint32_t result = 0;
  for (int i = 0; i < count; ++i) {
    result = (result << 1) | (read_bit() ? 1 : 0);
  }

  return result;
}

void BitStream::writeByte(uint8_t byte) { write_bits(byte, 8); }

uint8_t BitStream::readByte() { return static_cast<uint8_t>(read_bits(8)); }

std::vector<uint8_t> BitStream::get_buffer() const {
  size_t byte_count = (bit_position_ + 7) / 8;
  return std::vector<uint8_t>(buffer_.begin(), buffer_.begin() + byte_count);
}

void BitStream::load_from_buffer(const std::vector<uint8_t>& data) {
  clear();
  buffer_ = data;
  bit_position_ = data.size() * 8;  // Set to the end for writing
  read_position_ = 0;               // Reset read position to the beginning
}

void BitStream::clear() {
  buffer_.clear();
  bit_position_ = 0;
  read_position_ = 0;
}

size_t BitStream::size() const { return bit_position_; }

bool BitStream::empty() const { return bit_position_ == 0; }

void BitStream::ensure_capacity(size_t additional_bits) {
  size_t required_bytes = (bit_position_ + additional_bits + 7) / 8;
  if (buffer_.size() < required_bytes) {
    buffer_.resize(required_bytes, 0);
  }
}
