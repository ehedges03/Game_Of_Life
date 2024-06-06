#include "GameBoard.h"
#include "Utils/Console.h"
#include <bitset>
#include <iostream>
#include <utility>

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
  // Setup the border for all chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second->readInBorder();
  }

  // Process the chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second->processNextState();
  }
}

void GameBoard::checkChunk(ChunkKey key, std::shared_ptr<Chunk> c) {
  if (!c || !c->empty())
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

  m_chunks.erase(key);
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

std::shared_ptr<Chunk> GameBoard::getOrMakeChunk(ChunkKey key) {
  std::shared_ptr<Chunk> chunk = getChunk(key);

  if (chunk) {
    return chunk;
  }

  // Update the boundaries of the GameBoard
  m_maxX = std::max(key.x, m_maxX);
  m_minX = std::min(key.x, m_minX);
  m_maxY = std::max(key.y, m_maxY);
  m_minY = std::min(key.y, m_minY);

  chunk = std::make_shared<Chunk>();
  m_chunks[key] = chunk;

  chunk->upLeft = getChunk({key.x - 1, key.y + 1});
  chunk->up = getChunk({key.x, key.y + 1});
  chunk->upRight = getChunk({key.x + 1, key.y + 1});
  chunk->left = getChunk({key.x - 1, key.y});
  chunk->right = getChunk({key.x + 1, key.y});
  chunk->downLeft = getChunk({key.x - 1, key.y - 1});
  chunk->down = getChunk({key.x, key.y - 1});
  chunk->downRight = getChunk({key.x + 1, key.y - 1});

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

  return chunk;
}

#define VISUALIZE_BORDERS true


// These have to be able to print as one character wide otherwise it will break the print
#ifdef _WIN32

#define ALIVE_CELL "0"
#define DEAD_CELL "/"

#else

#define ALIVE_CELL "▓"
#define DEAD_CELL "0"

#endif

std::ostream &operator<<(std::ostream &o, GameBoard &g) {
  Chunk defaultEmpty;
  Console::Screen::clear();
  Console::Cursor::setPosition(0, 0);

#if VISUALIZE_BORDERS
  for (int32_t y = g.m_maxY; y >= g.m_minY; y--) {
    for (int32_t x = g.m_minX; x <= g.m_maxX; x++) {
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
#else
  for (int32_t y = g.m_maxY; y >= g.m_minY; y--) {
    for (int32_t x = g.m_minX; x <= g.m_maxX; x++) {
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

  o << std::endl;

  o << "X: (" << g.m_minX << ")-(" << g.m_maxX << ") | Y: (" << g.m_minY
    << ")-(" << g.m_maxY << ")" << std::endl;
  o << "Total Chunks: " << g.m_chunks.size() << std::endl;

  return o;
}

// std::ostream &operator<<(std::ostream &o, Chunk &c) {
//   return o;
// }

std::ostream &operator<<(std::ostream &o, Chunk &c) {
#if VISUALIZE_BORDERS
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
#else
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

  if (up) {
    m_data[k_topBorder] |= up->m_data[1] & k_dataBits;
  }

  if (upLeft) {
    m_data[k_topBorder] |= (upLeft->m_data[1] << k_size) & k_leftBorderBit;
  }

  if (upRight) {
    m_data[k_topBorder] |= (upRight->m_data[1] >> k_size) & k_rightBorderBit;
  }

  if (down) {
    m_data[k_bottomBorder] |= down->m_data[k_size] & k_dataBits;
  }

  if (downLeft) {
    m_data[k_bottomBorder] |=
        (downLeft->m_data[k_size] << k_size) & k_leftBorderBit;
  }

  if (downRight) {
    m_data[k_bottomBorder] |=
        (downRight->m_data[k_size] >> k_size) & k_rightBorderBit;
  }

  if (left) {
    for (int i = 1; i <= k_size; i++) {
      m_data[i] |= (left->m_data[i] << k_size) & k_leftBorderBit;
    }
  }

  if (right) {
    for (int i = 1; i <= k_size; i++) {
      m_data[i] |= (right->m_data[i] >> k_size) & k_rightBorderBit;
    }
  }
}

void Chunk::processNextState() {
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
}

bool Chunk::empty() {
  uint64_t val;
  // Skip top and bottom
  for (int32_t i = 1; i <= k_size; i++) {
    // Or with borders cleared
    val |= (m_data[i] & k_dataBits);
  }

  return val == 0;
};
