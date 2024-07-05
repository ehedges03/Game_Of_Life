#pragma once
#include <iostream>
#include <memory>
#include <stdbool.h>
#include <stdint.h>

class BitArray {
public:
  BitArray() = delete;
  explicit BitArray(uint32_t bits);
  ~BitArray() = default;
  bool get(uint32_t index) const;
  void set(uint32_t index, bool value);
  uint32_t size() const;

private:
  std::unique_ptr<uint8_t[]> m_bytes;
  uint32_t m_byteSize;
  uint32_t m_bits;
  void checkIndex(uint32_t index) const;
};

std::ostream &operator<<(std::ostream &os, BitArray const &m);
