#pragma once
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>

class Chunk {
public:
    using RowType = uint16_t;

    enum Flags : uint32_t {
        CLEAR = 0,
        // Flag specifying that the chunk is currently empty better to just check
        // this than constantly caluclate it
        EMPTY = 1,
        // Flag specifying that one or more of the surrounding border chunks does
        // not exist
        MISSING_BORDER_CHUNK = 1 << 1,
        // Flag specifying if all surrounding border chunks are empty
        ALL_BORDERS_EMPTY = 1 << 2,
    };

    // I think this should size should be 6 bits under the type used in m_data for
    // each row.
    static constexpr int32_t k_size = 8;
    static constexpr int32_t k_topBorder = k_size + 1;
    static constexpr int32_t k_bottomBorder = 0;
    static constexpr RowType k_leftBorderBit = 1ul << (k_size + 1);
    static constexpr RowType k_rightBorderBit = 1ul;
    static constexpr RowType k_dataBits = ((1ul << k_size) - 1) << 1;

    std::shared_ptr<Chunk> upLeft;
    std::shared_ptr<Chunk> up;
    std::shared_ptr<Chunk> upRight;
    std::shared_ptr<Chunk> left;
    std::shared_ptr<Chunk> right;
    std::shared_ptr<Chunk> downLeft;
    std::shared_ptr<Chunk> down;
    std::shared_ptr<Chunk> downRight;

    // I am not sure if this should return the chunks Flags, maybe there should
    // just be a function called getFlags() or maybe both?
    void processNextState();
    void readInBorder();
    Flags getFlags() { return m_flags; }

    bool getCell(int32_t x, int32_t y);
    void setCell(int32_t x, int32_t y, bool val);

    using iterator = typename std::array<RowType, k_size + 2>::iterator;
    using reverse_iterator =
        typename std::array<RowType, k_size + 2>::reverse_iterator;
    using const_iterator =
        typename std::array<RowType, k_size + 2>::const_iterator;
    using const_reverse_iterator =
        typename std::array<RowType, k_size + 2>::const_reverse_iterator;

    iterator begin() { return m_data.begin(); };
    const_iterator begin() const { return m_data.begin(); };

    reverse_iterator rbegin() { return m_data.rbegin(); };
    const_reverse_iterator rbegin() const { return m_data.rbegin(); };

    iterator end() { return m_data.end(); };
    const_iterator end() const { return m_data.end(); };

    reverse_iterator rend() { return m_data.rend(); };
    const_reverse_iterator rend() const { return m_data.rend(); };

    friend std::ostream& operator<<(std::ostream& o, Chunk& c);

private:
    Flags m_flags = Flags::EMPTY;
    std::array<RowType, k_size + 2> m_data{};

    void processEmpty();
};

inline Chunk::Flags operator~(Chunk::Flags a) {
    return (Chunk::Flags)~(uint32_t)a;
}
inline Chunk::Flags operator|(Chunk::Flags a, Chunk::Flags b) {
    return (Chunk::Flags)((uint32_t)a | (uint32_t)b);
}
inline Chunk::Flags operator&(Chunk::Flags a, Chunk::Flags b) {
    return (Chunk::Flags)((uint32_t)a & (uint32_t)b);
}
inline Chunk::Flags operator^(Chunk::Flags a, Chunk::Flags b) {
    return (Chunk::Flags)((uint32_t)a ^ (uint32_t)b);
}
inline Chunk::Flags& operator|=(Chunk::Flags& a, Chunk::Flags b) {
    return (Chunk::Flags&)((uint32_t&)a |= (uint32_t)b);
}
inline Chunk::Flags& operator&=(Chunk::Flags& a, Chunk::Flags b) {
    return (Chunk::Flags&)((uint32_t&)a &= (uint32_t)b);
}
inline Chunk::Flags& operator^=(Chunk::Flags& a, Chunk::Flags b) {
    return (Chunk::Flags&)((uint32_t&)a ^= (uint32_t)b);
}

/**
* This function creates a table (array) with indices spanning all possible values for a 
* given unsigned integer type. Each value in the array is a bool representing whether the integer
* has three consecutive bits somewhere within it (true) or not (false).
* 
* I have a feeling it could be faster, but it's constexpr so definitely not a priority
*/
template <typename UintThingy, int numVals = (sizeof(UintThingy) * 256)>
constexpr std::array<bool, numVals> getThreeConsecutiveBitCheckTable() {
  static_assert(std::numeric_limits<UintThingy>::is_signed == false);
  std::array<bool, numVals> table{};

  for (UintThingy candidate = std::numeric_limits<UintThingy>::max() ; candidate >= 0b111 ; candidate--) {
    UintThingy testee = candidate;

    // I figured I could subtract 3 and not 2 here, but that seems to be an off by 1 bc/ the output was wrong
    // for consecutive bits in the most significant nibble (i.e. 0b11100000 was false)
    // I might be sleepy...
    for (int i = sizeof(UintThingy) * 8 - 2 ; i >= 0 ; i--) {
      if (!(testee & 0b0111 ^ 0b0111)) {
        table[candidate] = true;
        break;
      }
      testee >>= 1;
    }
  }

  return table;
}
