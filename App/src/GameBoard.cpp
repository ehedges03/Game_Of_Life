#include "GameBoard.h"
#include "Utils/Console.h"
#include <bitset>
#include <iostream>
#include <utility>
#include <limits>

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

constexpr std::array<bool, 256> createThreeConsecBitInByteMap() {
  std::array<bool, 256> map{};

  for (int val = 0b111; val <= 0xFF; val = (val << 1) | 1) {
    for (int i = val; i <= 0xFF; i <<= 1) {
      map[i] = true;
    }
  }

  return map;
}

// I apoligize for this name bit it stands for three consecutive bits in a byte
// map Which takes in a byte and tells you if there are 3 consecutive bits in
// that byte pretty self explanatory ¯\_(ツ)_/¯
static const std::array<bool, 256> tcbibm = createThreeConsecBitInByteMap();

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

/*
GameBoard method definitions
*/

void GameBoard::setPoint(int32_t x, int32_t y, bool value) {
  ChunkKey key = calcChunkKey(x, y);
  auto chunk = getOrMakeChunk(key);

  int32_t properX, properY;

  if (x >= 0) {
    properX = x % Chunk::k_size;
  } else {
    properX = (Chunk::k_size - 1) + ((x + 1) % Chunk::k_size);
  }

  if (y >= 0) {
    properY = y % Chunk::k_size;
    // properY = (Chunk::Size - 1) - ((y) % Chunk::Size);
  } else {
    // properY = -((y + 1) % Chunk::Size);
    properY = (Chunk::k_size - 1) + ((y + 1) % Chunk::k_size);
  }

  chunk->setCell(properX, properY, value);
}

bool GameBoard::getPoint(int32_t x, int32_t y) {
  ChunkKey key = calcChunkKey(x, y);
  auto chunk = getChunk(key);

  if (chunk) {
    return chunk->getCell(x % Chunk::k_size, y % Chunk::k_size);
  }

  return false;
}

void GameBoard::update() {

  // Check chunks for deletion
  for (auto it = m_chunks.begin(); it != m_chunks.end();) {
    Chunk::Flags flags = it->second->getFlags();

    // Check that the chunk is empty and all borders are empty
    if ((flags & (Chunk::Flags::EMPTY | Chunk::Flags::ALL_BORDERS_EMPTY)) ==
        (Chunk::Flags::EMPTY | Chunk::Flags::ALL_BORDERS_EMPTY)) {
      deleteChunkBorders(it->second);
      it = m_chunks.erase(it);
    } else {
      it++;
    }
  }

  // Check if chunks need to be created
  for (auto &chunkPair : m_chunks) {
    Chunk::Flags flags = chunkPair.second->getFlags();

    // Check that the chunk is not empty and has missing border chunks
    if ((flags & (Chunk::Flags::EMPTY | Chunk::Flags::MISSING_BORDER_CHUNK)) ==
        Chunk::Flags::MISSING_BORDER_CHUNK) {
      makeBorderChunks(chunkPair.first, chunkPair.second);
    }
  }

  // Setup the border for all chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second->readInBorder();
  }

  // Process the chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second->processNextState();
  }
}

void GameBoard::deleteChunkBorders(std::shared_ptr<Chunk> c) {
  if (!c)
    return;

  if (c->upLeft)
    c->upLeft->downRight = nullptr;

  if (c->up)
    c->up->down = nullptr;

  if (c->upRight)
    c->upRight->downLeft = nullptr;

  if (c->left)
    c->left->right = nullptr;

  if (c->right)
    c->right->left = nullptr;

  if (c->downLeft)
    c->downLeft->upRight = nullptr;

  if (c->down)
    c->down->up = nullptr;

  if (c->downRight)
    c->downRight->upLeft = nullptr;
}

ChunkKey GameBoard::calcChunkKey(int32_t x, int32_t y) {
  int32_t realChunkX, realChunkY;

  if (x >= 0) {
    realChunkX = x / Chunk::k_size;
  } else {
    realChunkX = -1 + ((x + 1) / Chunk::k_size);
  }

  if (y >= 0) {
    realChunkY = y / Chunk::k_size;
  } else {
    realChunkY = -1 + ((y + 1) / Chunk::k_size);
  }

  return {realChunkX, realChunkY};
}

std::shared_ptr<Chunk> GameBoard::getChunk(ChunkKey key) {
  auto chunk_entry = m_chunks.find(key);
  if (chunk_entry == m_chunks.end()) {
    return nullptr;
  }

  return chunk_entry->second;
}

void GameBoard::makeChunk(ChunkKey key) {
  std::shared_ptr<Chunk> chunk = getChunk(key);

  if (chunk) {
    return;
  }

  chunk = std::make_shared<Chunk>();
  m_chunks[key] = chunk;

  // Get all border chunks into the references
  chunk->upLeft = getChunk({key.x - 1, key.y + 1});
  chunk->up = getChunk({key.x, key.y + 1});
  chunk->upRight = getChunk({key.x + 1, key.y + 1});
  chunk->left = getChunk({key.x - 1, key.y});
  chunk->right = getChunk({key.x + 1, key.y});
  chunk->downLeft = getChunk({key.x - 1, key.y - 1});
  chunk->down = getChunk({key.x, key.y - 1});
  chunk->downRight = getChunk({key.x + 1, key.y - 1});

  // For each border chunk reference this chunk in it
  if (chunk->upLeft)
    chunk->upLeft->downRight = chunk;

  if (chunk->up)
    chunk->up->down = chunk;

  if (chunk->upRight)
    chunk->upRight->downLeft = chunk;

  if (chunk->left)
    chunk->left->right = chunk;

  if (chunk->right)
    chunk->right->left = chunk;

  if (chunk->downLeft)
    chunk->downLeft->upRight = chunk;

  if (chunk->down)
    chunk->down->up = chunk;

  if (chunk->downRight)
    chunk->downRight->upLeft = chunk;
}

void GameBoard::makeBorderChunks(ChunkKey key, std::shared_ptr<Chunk> c) {
  if (!c->upLeft) {
    makeChunk({key.x - 1, key.y + 1});
  }

  if (!c->up) {
    makeChunk({key.x, key.y + 1});
  }

  if (!c->upRight) {
    makeChunk({key.x + 1, key.y + 1});
  }

  if (!c->left) {
    makeChunk({key.x - 1, key.y});
  }

  if (!c->right) {
    makeChunk({key.x + 1, key.y});
  }

  if (!c->downLeft) {
    makeChunk({key.x - 1, key.y - 1});
  }

  if (!c->down) {
    makeChunk({key.x, key.y - 1});
  }

  if (!c->downRight) {
    makeChunk({key.x + 1, key.y - 1});
  }
}

std::shared_ptr<Chunk> GameBoard::getOrMakeChunk(ChunkKey key) {
  std::shared_ptr<Chunk> chunk = getChunk(key);

  if (chunk) {
    return chunk;
  }

  makeChunk(key);

  return m_chunks[key];
}

// These have to be able to print as one character wide otherwise it will break
// the print
#ifdef _WIN32

#define ALIVE_CELL "0"
#define DEAD_CELL "/"

#else

#define ALIVE_CELL "▓"
#define DEAD_CELL "0"

#endif

std::ostream &operator<<(std::ostream &o, GameBoard &g) {
  int32_t maxX = std::numeric_limits<int32_t>::min();
  int32_t minX = std::numeric_limits<int32_t>::max();
  int32_t maxY = std::numeric_limits<int32_t>::min();
  int32_t minY = std::numeric_limits<int32_t>::max();

  // Setup the border for all chunks
  for (auto &chunkPair : g.m_chunks) {
    ChunkKey k = chunkPair.first;

    // Update the boundaries of the GameBoard
    maxX = std::max(k.x, maxX);
    minX = std::min(k.x, minX);
    maxY = std::max(k.y, maxY);
    minY = std::min(k.y, minY);
  }

#if PRINT_GB
  Chunk defaultEmpty;
  o << std::endl;
  Console::Screen::clear();
  Console::Cursor::setPosition(0, 0);

#if VISUALIZE == VISUALIZE_BORDERS
  for (int32_t y = maxY; y >= minY; y--) {
    for (int32_t x = minX; x <= maxX; x++) {
      if (g.m_chunks.find({x, y}) != g.m_chunks.end()) {
        o << *g.m_chunks.at({x, y}) << std::flush;
      } else {
        o << defaultEmpty << std::flush;
      }
      Console::Cursor::up(Chunk::k_size + 2);
      Console::Cursor::forward(Chunk::k_size + 3);
    }
    Console::Cursor::down(Chunk::k_size + 2);
    std::cout << std::endl;
  }
#endif

#if VISUALIZE == VISUALIZE_DEFAULT
  for (int32_t y = maxY; y >= minY; y--) {
    for (int32_t x = minX; x <= maxX; x++) {
      if (g.m_chunks.find({x, y}) != g.m_chunks.end()) {
        o << *g.m_chunks.at({x, y}) << std::flush;
      } else {
        o << defaultEmpty << std::flush;
      }
      Console::Cursor::up(Chunk::k_size);
      Console::Cursor::forward(Chunk::k_size);
    }
    Console::Cursor::down(Chunk::k_size - 1);
    std::cout << std::endl;
  }

#endif

#endif

  o << std::endl;

  o << "X: (" << minX << ")-(" << maxX << ") | Y: (" << minY << ")-(" << maxY
    << ")" << std::endl;
  o << "Total Chunks: " << g.m_chunks.size() << std::endl;

  return o;
}

std::ostream &operator<<(std::ostream &o, Chunk &c) {
#if VISUALIZE == VISUALIZE_BORDERS
  // Make sure that the border is read in before rendering it out
  c.readInBorder();

  for (int32_t y = Chunk::k_topBorder; y >= 0; y--) {
    std::bitset<Chunk::k_size + 2> row(c.m_data[y]);
    for (int x = Chunk::k_size + 1; x >= 0; x--) {
      if (row[x]) {
        o << ALIVE_CELL;
      } else {
        o << DEAD_CELL;
      }
    }
    Console::Cursor::down(1);
    Console::Cursor::backward(Chunk::k_size + 2);
  }
#endif

#if VISUALIZE == VISUALIZE_DEFAULT
  for (int32_t y = Chunk::k_size; y > 0; y--) {
    std::bitset<Chunk::k_size> row((c.m_data[y] & Chunk::k_dataBits) >> 1);
    for (int x = Chunk::k_size - 1; x >= 0; x--) {
      if (row[x]) {
        o << ALIVE_CELL;
      } else {
        o << DEAD_CELL;
      }
    }
    Console::Cursor::down(1);
    Console::Cursor::backward(Chunk::k_size);
  }
#endif

  return o;
}

/*
 * Chunk method definitions
 */

bool Chunk::getCell(int32_t x, int32_t y) {
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
  int32_t allBordersEmpty = Flags::EMPTY;

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

  if (allBordersEmpty) {
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
