#include "Chunk.h"

constexpr std::array<bool, 512> createBitsToStateMap() {
  std::array<bool, 512> map;
  for (int16_t i = 0; i <= 0b111111111; i++) {
    uint8_t neighbor_count = 0;
    int16_t neighbors = i & 0b111101111;
    bool alive = i & 0b000010000;
    while (neighbors > 0) {
      if (neighbors & 0b1) {
        neighbor_count++;
      }

      neighbors >>= 1;
    }

    map[i] = neighbor_count == 3 || (neighbor_count == 2 && alive);
  }
  return map;
}

static const std::array<bool, 512> bitsToState = createBitsToStateMap();

// I apoligize for this name bit it stands for three consecutive bits in a byte
// map Which takes in a byte and tells you if there are 3 consecutive bits in
// that byte pretty self explanatory ¯\_(ツ)_/¯
const std::array<bool, 256> tcbibm =
    getThreeConsecutiveBitCheckTable<unsigned char>();

constexpr std::array<bool, 32> createCornerMap() {
  std::array<bool, 32> map{};

  for (int16_t i = 0; i <= 0b11111; i++) {
    int16_t current = i;
    int8_t count = 0;
    while (current > 0) {
      if (current & 0b1) {
        count++;
      }

      current >>= 1;
    }

    map[i] = count >= 3;
  }

  return map;
}

// Map of byte to the number of bits in it
static const std::array<bool, 32> cornerMap = createCornerMap();

bool Chunk::getCell(int32_t x, int32_t y) const {
  uint64_t mask = 1ul << (k_size - x);
  return m_data[y + 1] & mask;
}

void Chunk::setCell(int32_t x, int32_t y, bool val) {
  uint64_t loc = 1ul << (k_size - x);
  if (val) {
    // When the user sets a cell in a chunk assume that the chunk is no longer
    // empty and that there are missing border chunks to simplify intial start
    m_flags &= ~Flags::EMPTY;
    m_flags |= Flags::MISSING_BORDER_CHUNK;

    m_data[y + 1] |= loc;
  } else {
    m_data[y + 1] &= ~loc;
  }
}

std::array<uint32_t, 2> Chunk::data() const {
  std::array<uint32_t, 2> data = {};
  for (int y = k_size - 1; y >= 0; y--) {
    int dataByte = (k_size - 1 - y) / 4;
    data[dataByte] = data[dataByte] << k_size;
    uint32_t databits = (m_data[y + 1] & k_dataBits) >> 1;
    data[dataByte] |= databits;
  }
  return data;
}

void Chunk::readInBorder() {
  for (int i = 1; i <= k_size; i++) {
    m_data[i] &= k_dataBits;
  }
  m_data[k_topBorder] = 0;
  m_data[k_bottomBorder] = 0;
  // This will be added to each time and if there are not 8 then there is a
  // missing border chunk
  int32_t borderingChunks = 0;
  // This will be anded with all chunks flags and if any of them are not empty
  // this will be false
  Flags allBordersEmpty = Flags::EMPTY;

  if (up) {
    m_data[k_topBorder] |= up->m_data[1] & k_dataBits;
    allBordersEmpty &= up->m_flags;
    borderingChunks++;
  }

  if (upLeft) {
    m_data[k_topBorder] |= (upLeft->m_data[1] << k_size) & k_leftBorderBit;
    allBordersEmpty &= upLeft->m_flags;
    borderingChunks++;
  }

  if (upRight) {
    m_data[k_topBorder] |= (upRight->m_data[1] >> k_size) & k_rightBorderBit;
    allBordersEmpty &= upRight->m_flags;
    borderingChunks++;
  }

  if (down) {
    m_data[k_bottomBorder] |= down->m_data[k_size] & k_dataBits;
    allBordersEmpty &= down->m_flags;
    borderingChunks++;
  }

  if (downLeft) {
    m_data[k_bottomBorder] |=
        (downLeft->m_data[k_size] << k_size) & k_leftBorderBit;
    allBordersEmpty &= downLeft->m_flags;
    borderingChunks++;
  }

  if (downRight) {
    m_data[k_bottomBorder] |=
        (downRight->m_data[k_size] >> k_size) & k_rightBorderBit;
    allBordersEmpty &= downRight->m_flags;
    borderingChunks++;
  }

  if (left) {
    for (int i = 1; i <= k_size; i++) {
      m_data[i] |= (left->m_data[i] << k_size) & k_leftBorderBit;
    }
    allBordersEmpty &= left->m_flags;
    borderingChunks++;
  }

  if (right) {
    for (int i = 1; i <= k_size; i++) {
      m_data[i] |= (right->m_data[i] >> k_size) & k_rightBorderBit;
    }
    allBordersEmpty &= right->m_flags;
    borderingChunks++;
  }

  if (static_cast<uint32_t>(allBordersEmpty)) {
    m_flags |= Flags::ALL_BORDERS_EMPTY;
  } else {
    m_flags &= ~Flags::ALL_BORDERS_EMPTY;
  }

  if (borderingChunks != 8) {
    m_flags |= Flags::MISSING_BORDER_CHUNK;
  } else {
    m_flags &= ~Flags::MISSING_BORDER_CHUNK;
  }
}

void Chunk::processNextState() {
  /*
  if (m_flags & Flags::EMPTY) {
    // Logic for if the border will spawn any cells or not
    RowType top = m_data[k_topBorder];
    RowType bottom = m_data[k_bottomBorder];
    RowType left = 0;
    RowType right = 0;

    for (int i = k_bottomBorder; i <= k_topBorder; i++) {
      right <<= 1;
      right |= m_data[i] & k_rightBorderBit;

      left >>= 1;
      left |= m_data[i] & k_leftBorderBit;
    }

    // The + 2 is for the border and the + 3 is to round up the calculation
    constexpr int32_t nibbles = ((k_size + 2) + 3) / 4;

    bool br = cornerMap[((right >> (k_size - 1)) | (bottom << 2)) & 0x1F];
    bool bl = cornerMap[((bottom >> (k_size - 1)) | (left << 2)) & 0x1F];
    bool tr = cornerMap[((top << 2) | (right >> 1)) & 0x1F];
    bool tl = cornerMap[(((top >> 2) | (left << 1)) >> (k_size - 3)) & 0x1F];


    bool spawnFromBorder = br | bl | tr | tl;

    for (int i = 0; i < nibbles && !spawnFromBorder; i++) {
      spawnFromBorder = tcbibm[top & 0xFF] || tcbibm[bottom & 0xFF] ||
                        tcbibm[left & 0xFF] || tcbibm[right & 0xFF];

      top >>= 4;
      bottom >>= 4;
      left >>= 4;
      right >>= 4;
    }

    if (!spawnFromBorder) {
      return;
    }
  }
  */

  uint64_t top = m_data[k_topBorder];

  for (int y = k_size; y > k_bottomBorder; y--) {
    uint64_t newVals = 0;
    uint64_t curr = m_data[y];
    uint64_t bot = m_data[y - 1];
    for (int x = Chunk::k_size - 1; x >= 0; x--) {
      uint32_t around = 0;

      // Top 3 bits
      around |= ((top >> x) & 0b111) << 6;

      // Middle 3 bits
      around |= ((curr >> x) & 0b111) << 3;

      // Bottom 3 bits
      around |= (bot >> x) & 0b111;

      // std::cout << "(" << x << "," << (y - 1) << ") -> "
      //           << std::bitset<3>(around >> 6) << " "
      //           << std::bitset<3>(around >> 3) << " " <<
      //           std::bitset<3>(around)
      //           << " -> " << bitsToState[around]
      //           << std::endl;

      newVals |= bitsToState[around];
      newVals <<= 1;
    }
    // std::cout << std::bitset<Chunk::Size>(curr) << std::endl;

    top = m_data[y];
    m_data[y] = newVals;
  }

  processEmpty();

  return;
}

void Chunk::processEmpty() {
  uint64_t val = 0;
  // Skip top and bottom
  for (int32_t i = 1; i <= k_size; i++) {
    // Or with borders cleared
    val |= (m_data[i] & k_dataBits);
  }

  if (val == 0) {
    m_flags |= Flags::EMPTY;
  } else {
    m_flags &= ~Flags::EMPTY;
  }
};
