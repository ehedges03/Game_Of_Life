#pragma once
#include <stdint.h>
#include <stdbool.h>

class BitArray {
public:
	BitArray(int bits);
	~BitArray();
	bool get(int index) const;
	void set(int index, bool value);
	int size() const;

private:
	uint8_t* m_bytes;
	int m_byteSize;
	int m_bits;
	void checkIndex(int index) const;
};

std::ostream& operator<<(std::ostream& os, BitArray const& m);