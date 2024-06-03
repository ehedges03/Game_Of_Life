#include "GameBoard.h"
#include "Utils/Console.h"
#include <bitset>
#include <iostream>
#include <utility>

static std::bitset<512> createBitsToStateMap() {
  std::bitset<512> map;
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

static std::bitset<512> bitsToState = createBitsToStateMap();

/*
GameBoard method definitions
*/

void GameBoard::setPoint(int32_t x, int32_t y, bool value) {
  ChunkKey key = calcChunkKey(x, y);
  auto chunk = getOrMakeChunk(key);

  int32_t properX, properY;

  if (x >= 0) {
    properX = x % Chunk::Size;
  } else {
    properX = (Chunk::Size - 1) + ((x + 1) % Chunk::Size);
  }

  if (y >= 0) {
    properY = y % Chunk::Size;
    // properY = (Chunk::Size - 1) - ((y) % Chunk::Size);
  } else {
    // properY = -((y + 1) % Chunk::Size);
    properY = (Chunk::Size - 1) + ((y + 1) % Chunk::Size);
  }

  chunk->setCell(properX, properY, value);
  checkChunk(key, chunk);
}

bool GameBoard::getPoint(int32_t x, int32_t y) {
  ChunkKey key = calcChunkKey(x, y);
  auto chunk = getChunk(key);

  if (chunk) {
    return chunk->getCell(x % Chunk::Size, y % Chunk::Size);
  }

  return false;
}

void GameBoard::update() {
  // Setup the border for all chunks
  for (auto &chunkPair : m_chunks) {
    // const Chunk &chunk = chunkPair.second;
    // int32_t x = x, y = y;

    // auto upLeft = getChunk(x - 1, y + 1);
    // if (upLeft != nullptr)
    //   upLeft.border.bottomRight = chunk[0][0];

    // auto upRight = getChunk(x + 1, y + 1);
    // if (upRight.has_value())
    //   upRight.value().get().border.bottomRight = chunk[0][Chunk::Size - 1];

    // auto downLeft = getChunk(x - 1, y - 1);
    // if (downLeft.has_value())
    //   downLeft.value().get().border.bottomRight = chunk[Chunk::Size - 1][0];

    // auto downRight = getChunk(x + 1, y - 1);
    // if (downRight.has_value())
    //   downRight.value().get().border.bottomRight =
    //       chunk[Chunk::Size - 1][Chunk::Size - 1];

    // auto up = getChunk(x, y + 1);
    // if (up.has_value())
    //   up.value().get().border.bottom = chunk[0];

    // auto down = getChunk(x, y - 1);
    // if (down.has_value())
    //   down.value().get().border.bottom = chunk[Chunk::Size - 1];

    // auto left = getChunk(x - 1, y);
    // if (left.has_value()) {
    //   Chunk &leftChunk = left.value().get();
    //   for (int i = 0; i < Chunk::Size; i++) {
    //     leftChunk.border.right[i] = chunk[i][0];
    //   }
    // }

    // auto right = getChunk(x + 1, y);
    // if (right.has_value()) {
    //   Chunk &rightChunk = right.value().get();
    //   for (int i = 0; i < Chunk::Size; i++) {
    //     rightChunk.border.right[i] = chunk[i][Chunk::Size - 1];
    //   }
    // }
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

ChunkKey GameBoard::calcChunkKey(uint32_t x, uint32_t y) {
  int32_t realChunkX, realChunkY;

  if (x >= 0) {
    realChunkX = x / Chunk::Size;
  } else {
    realChunkX = -1 + ((x + 1) / Chunk::Size);
  }

  if (y >= 0) {
    realChunkY = y / Chunk::Size;
  } else {
    realChunkY = -1 + ((y + 1) / Chunk::Size);
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

std::ostream &operator<<(std::ostream &o, GameBoard &g) {
  Chunk defaultEmpty;
  Console::Screen::clear();
  Console::Cursor::setPosition(0, 0);

  for (int32_t y = g.m_maxY; y >= g.m_minY; y--) {
    for (int32_t x = g.m_minX; x <= g.m_maxX; x++) {
      if (g.m_chunks.find({x, y}) != g.m_chunks.end()) {
        o << *g.m_chunks.at({x, y}) << std::flush;
      } else {
        o << defaultEmpty << std::flush;
      }
      Console::Cursor::up(Chunk::Size);
      Console::Cursor::forward(Chunk::Size);
    }
    Console::Cursor::down(Chunk::Size - 1);
    std::cout << std::endl;
  }
  o << std::endl;

  o << "X: " << g.m_minX << "-" << g.m_maxX << " | Y: " << g.m_minY << "-"
    << g.m_maxY << std::endl;
  o << "Total Chunks: " << g.m_chunks.size() << std::endl;

  return o;
}

std::ostream &operator<<(std::ostream &o, Chunk &c) {
  for (int32_t y = Chunk::Size; y > 0; y--) {
    std::bitset<Chunk::Size> row((c.m_data[y] & Chunk::DataBits) >> 1);
    for (int x = Chunk::Size - 1; x >= 0; x--) {
      if (row[x]) {
        o << "▓";
      } else {
        o << "0";
      }
    }
    Console::Cursor::down(1);
    Console::Cursor::backward(Chunk::Size);
  }
  return o;
}

/*
 * Chunk method definitions
 */

bool Chunk::getCell(int32_t x, int32_t y) {
  uint64_t mask = 1ul << (Size - x);
  return m_data[y + 1] & mask;
}

void Chunk::setCell(int32_t x, int32_t y, bool val) {
  uint64_t loc = 1ul << (Size - x);
  if (val) {
    m_data[y + 1] |= loc;
  } else {
    m_data[y + 1] &= ~loc;
  }
}

void Chunk::processNextState() {
  // Left
  // Right

  // Inside Column
  // uint64_t up;
  // uint64_t current = border.top.to_ulong();
  // uint64_t current = 0;
  // uint64_t down = m_data[0];
  // for (int i = 0; i < Chunk::Size - 1; i++) {
  //   up = current;
  //   current = down;
  //   down = m_data[i - 1];
  //   for (int j = 1; j < Chunk::Size - 1; j++) {
  //     uint32_t around = 0;

  //     // Top 3 bits
  //     around |= (up >> ((Chunk::Size - 8) - j)) & 0b111000000;

  //     // Middle 3 bits
  //     around |= (current >> ((Chunk::Size - 5) - j)) & 0b000111000;

  //     // Bottom 3 bits
  //     around |= (down >> ((Chunk::Size - 2) - j)) & 0b000000111;

  //     m_data[i][j] = bitsToState[around];
  //   }
  // }
  // up = current;
  // current = down;
  // down = 0;
  // for (int j = 1; j < Size - 1; j++) {
  //   uint32_t around = 0;

  //   // Top 3 bits
  //   around |= (up >> ((Size - 8) - j)) & 0b111000000;

  //   // Middle 3 bits
  //   around |= (current >> ((Size - 5) - j)) & 0b000111000;

  //   // Bottom 3 bits
  //   around |= (down >> ((Size - 2) - j)) & 0b000000111;

  //   m_data[Size - 1][j] = bitsToState[around];
  // }
}

bool Chunk::empty() {
  uint64_t val;
  // Skip top and bottom
  for (int32_t i = 1; i <= Size; i++) {
    // Or with borders cleared
    val |= (m_data[i] & DataBits);
  }

  return val == 0;
};
