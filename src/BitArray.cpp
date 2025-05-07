#include "BitArray.h"
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

BitArray::BitArray(uint32_t bits) {
  m_bits = bits;
  int bytes = bits / 8;
  if (bits % 8 > 0) {
    bytes++;
  }
  m_byteSize = bytes;

  m_bytes = std::make_unique<uint8_t[]>(bytes);
}

bool BitArray::get(uint32_t index) const {
  checkIndex(index);

  return (m_bytes[index / 8] >> (7 - (index % 8))) & 0b1;
}

void BitArray::set(uint32_t index, bool value) {
  checkIndex(index);

  if (value) {
    m_bytes[index / 8] |= 1 << (7 - (index % 8));
  } else {
    m_bytes[index / 8] &= ~(1 << (7 - (index % 8)));
  }
}

uint32_t BitArray::size() const { return m_bits; }

void BitArray::checkIndex(uint32_t index) const {
  if (index < 0 || index > m_bits - 1) {
    throw std::invalid_argument("Index is out of range.");
  }
}

std::ostream &operator<<(std::ostream &os, BitArray const &m) {
  os << "0b";
  for (uint32_t i = 0; i < m.size(); i++) {
    os << m.get(i);
  }
  return os;
}
