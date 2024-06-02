#include "GameBoard.h"
#include "Utils/Console.h"
#include <iostream>
#include <optional>
#include <utility>

static std::bitset<512> bitsToState;
static bool bitsToStateSetup;

static void setupBitsToStateMap() {
  if (!bitsToStateSetup) {
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

      bitsToState[i] = neighbor_count == 3 || (neighbor_count == 2 && alive);
    }
  }
}

/*
GameBoard method definitions
*/

void GameBoard::setPoint(int32_t x, int32_t y, bool value) {
  auto chunk = getChunk(x, y);

  // Return early because chunk does not exist and there is nothing to set
  if (!chunk.has_value() && !value) {
    return;
  }

  int32_t properX, properY;

  if (x >= 0) {
    properX = x % Chunk::Size;
  } else {
    properX = (Chunk::Size - 1) + ((x + 1) % Chunk::Size);
  }

  if (y >= 0) {
    properY = (Chunk::Size - 1) - ((y) % Chunk::Size);
  } else {
    properY = -((y + 1) % Chunk::Size);
  }

  if (chunk.has_value()) {
    chunk.value().get()[properY][properX] = value;
  } else {
    getOrMakeChunk(x, y)[properY][properX] = value;
  }
}

bool GameBoard::getPoint(int32_t x, int32_t y) {
  auto chunk = getChunk(x, y);

  if (chunk.has_value()) {
    return chunk.value().get()[x % Chunk::Size][y % Chunk::Size];
  }

  return false;
}

void GameBoard::update() {
  // Setup the border for all chunks
  for (auto &chunkPair : m_chunks) {
    const Chunk &chunk = chunkPair.second;
    int32_t x = x, y = y;

    auto upLeft = getChunk(x - 1, y + 1);
    if (upLeft.has_value())
      upLeft.value().get().border.bottomRight = chunk[0][0];

    auto upRight = getChunk(x + 1, y + 1);
    if (upRight.has_value())
      upRight.value().get().border.bottomRight = chunk[0][Chunk::Size - 1];

    auto downLeft = getChunk(x - 1, y - 1);
    if (downLeft.has_value())
      downLeft.value().get().border.bottomRight = chunk[Chunk::Size - 1][0];

    auto downRight = getChunk(x + 1, y - 1);
    if (downRight.has_value())
      downRight.value().get().border.bottomRight =
          chunk[Chunk::Size - 1][Chunk::Size - 1];

    auto up = getChunk(x, y + 1);
    if (up.has_value())
      up.value().get().border.bottom = chunk[0];

    auto down = getChunk(x, y - 1);
    if (down.has_value())
      down.value().get().border.bottom = chunk[Chunk::Size - 1];

    auto left = getChunk(x - 1, y);
    if (left.has_value()) {
      Chunk &leftChunk = left.value().get();
      for (int i = 0; i < Chunk::Size; i++) {
        leftChunk.border.right[i] = chunk[i][0];
      }
    }

    auto right = getChunk(x + 1, y);
    if (right.has_value()) {
      Chunk &rightChunk = right.value().get();
      for (int i = 0; i < Chunk::Size; i++) {
        rightChunk.border.right[i] = chunk[i][Chunk::Size - 1];
      }
    }
  }

  // Process the chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second.processNextState();
  }

  // Update the chunks
  for (auto &chunkPair : m_chunks) {
    chunkPair.second.swapToNextState();
  }
}

std::optional<std::reference_wrapper<Chunk>> GameBoard::getChunk(int32_t x, int32_t y) {
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

  std::pair<int32_t, int32_t> chunkKey{realChunkX, realChunkY};

  if (m_chunks.find(chunkKey) == m_chunks.end()) {
    return std::nullopt;
  }

  return m_chunks.at(chunkKey);
}

Chunk &GameBoard::getOrMakeChunk(int32_t x, int32_t y) {
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

  // Update the boundaries of the GameBoard
  m_maxX = std::max(realChunkX, m_maxX);
  m_minX = std::min(realChunkX, m_minX);
  m_maxY = std::max(realChunkY, m_maxY);
  m_minY = std::min(realChunkY, m_minY);

  return m_chunks[{realChunkX, realChunkY}];
}

std::ostream &operator<<(std::ostream &o, GameBoard &g) {
  Chunk defaultEmpty;
  Console::Screen::clear();
  int x = 0, y = 0;

  for (int32_t i = g.m_minX; i <= g.m_maxX; i++) {
    for (int32_t j = g.m_maxY; j >= g.m_minY; j--) {
      Console::Cursor::setPosition(1 + x * Chunk::Size, 1 + y * Chunk::Size);
      if (g.m_chunks.find({i, j}) != g.m_chunks.end()) {
        o << g.m_chunks.at({i, j}) << std::flush;
      } else {
        o << defaultEmpty << std::flush;
      }
      x++;
      y++;
    }
  }
  o << std::endl;

  return o;
}

std::ostream &operator<<(std::ostream &o, Chunk c) {
  for (auto &r : c.m_dataBuffer[c.m_currBuffer]) {
    for (int i = Chunk::Size - 1; i >= 0; i--) {
      if (r[i]) {
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

//Chunk::Chunk() {};

void Chunk::processNextState() {
  // Left
  // Right

  // Inside Column
  uint64_t up;
  uint64_t current = border.top.to_ulong();
  uint64_t down = m_dataBuffer[m_currBuffer][0].to_ulong();
  for (int i = 0; i < Chunk::Size - 1; i++) {
    up = current;
    current = down;
    down = m_dataBuffer[m_currBuffer][i - 1].to_ulong();
    for (int j = 1; j < Chunk::Size - 1; j++) {
      uint32_t around = 0;

      // Top 3 bits
      around |= (up >> ((Chunk::Size - 8) - j)) & 0b111000000;

      // Middle 3 bits
      around |= (current >> ((Chunk::Size - 5) - j)) & 0b000111000;

      // Bottom 3 bits
      around |= (down >> ((Chunk::Size - 2) - j)) & 0b000000111;

      m_dataBuffer[m_currBuffer ^ 1][i][j] = bitsToState[around];
    }
  }
  up = current;
  current = down;
  down = border.bottom.to_ulong();
  for (int j = 1; j < Size - 1; j++) {
    uint32_t around = 0;

    // Top 3 bits
    around |= (up >> ((Size - 8) - j)) & 0b111000000;

    // Middle 3 bits
    around |= (current >> ((Size - 5) - j)) & 0b000111000;

    // Bottom 3 bits
    around |= (down >> ((Size - 2) - j)) & 0b000000111;

    m_dataBuffer[m_currBuffer ^ 1][Size - 1][j] = bitsToState[around];
  }
}

void Chunk::swapToNextState() {
  m_currBuffer ^= 1;
}

bool Chunk::empty() {
  std::bitset<Size> val;
  for (auto &row : m_dataBuffer[m_currBuffer]) {
    val |= row;
  }

  return val.any();
};

std::bitset<Chunk::Size> &Chunk::operator[](int32_t i) {
  return m_dataBuffer[m_currBuffer][i];
}

const std::bitset<Chunk::Size> &Chunk::operator[](int32_t i) const {
  return m_dataBuffer[m_currBuffer][i];
}
