#include "pch.h"
#include "BitArray.h"
#include <stdlib.h>
#include <stdexcept>
#include <iostream>

BitArray::BitArray(int bits) {
	m_bits = bits;
	int bytes = bits / 8;
	if (bits % 8 > 0) {
		bytes++;
	}
	m_byteSize = bytes;

	m_bytes = (uint8_t*) calloc(bytes, sizeof(uint8_t));
}

BitArray::~BitArray() {
	free(m_bytes);
}

bool BitArray::get(int index) const{
	checkIndex(index);

	return (m_bytes[index / 8] >> (7 - (index % 8))) & 0b1;
}

void BitArray::set(int index, bool value) {
	checkIndex(index);

	if (value) {
		m_bytes[index / 8] |= 1 << (7 - (index % 8));
	}
	else {
		m_bytes[index / 8] &= ~(1 << (7 - (index % 8)));
	}
}

int BitArray::size() const {
	return m_bits;
}

void BitArray::checkIndex(int index) const {
	if (index < 0 || index > m_bits - 1) {
		throw std::invalid_argument("Index is out of range.");
	}
}

std::ostream& operator<<(std::ostream& os, BitArray const& m) {
	os << "0b";
	for (int i = 0; i < m.size(); i++) {
		os << m.get(i);
	}
	return os;
}