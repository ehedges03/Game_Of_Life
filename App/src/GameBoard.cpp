#include "GameBoard.h"
#include <iostream>

/*
GameBoard method definitions
*/

void GameBoard::setPoint(int32_t x, int32_t y, bool value) {
  std::pair<int32_t, int32_t> chunkKey{x / Chunk::Size, y / Chunk::Size};

  if (m_chunks.find(chunkKey) == m_chunks.end()) {
    // Dont create chunk if the value that will be set is false
    if (!value) {
      return;
    }

    // Chunk does not exist, construct it and insert into the map
    Chunk chunk(*this, chunkKey.first, chunkKey.second);
    m_chunks.insert({chunkKey, chunk});
  }

  Chunk &chunk = m_chunks.at(chunkKey);
  chunk[x % Chunk::Size][y % Chunk::Size] = value;
  m_maxX = std::max(chunkKey.first, m_maxX);
  m_minX = std::min(chunkKey.first, m_minX);
  m_maxY = std::max(chunkKey.second, m_maxY);
  m_minY = std::min(chunkKey.second, m_minY);
}

bool GameBoard::getPoint(int32_t x, int32_t y) {
  std::pair<int32_t, int32_t> chunkKey{x / Chunk::Size, y / Chunk::Size};

  if (m_chunks.find(chunkKey) != m_chunks.end()) {
    return m_chunks.at(chunkKey)[x % Chunk::Size][y % Chunk::Size];
  }

  return false;
}

std::ostream &operator<<(std::ostream &o, GameBoard &g) {
  GameBoard::Chunk defaultEmpty(g, 0, 0);
  std::cout << "Max X: " << g.m_maxX << std::endl;
  std::cout << "Min X: " << g.m_minX << std::endl;
  std::cout << "Max Y: " << g.m_maxY << std::endl;
  std::cout << "Min Y: " << g.m_minY << std::endl;

  for (int32_t i = g.m_minX; i <= g.m_maxX; i++) {
    for (int32_t j = g.m_maxY; j >= g.m_minY; j--) {
      if (g.m_chunks.find({i, j}) != g.m_chunks.end()) {
        std::cout << g.m_chunks.at({i, j}) << std::endl;
      } else {
        std::cout << defaultEmpty << std::endl;
      }
    }
  }

  o << "▓" << std::endl;
  o << "░" << std::endl;
  return o;
}

uint8_t GameBoard::countNeighbors(uint32_t x, uint32_t y) {
  // Would it be faster or slower to check if the value is 4 to avoid checking
  // all pixels The if statements could lead to a slower processing time

  // counts neighbors row by row
  // uint8_t count = 0;
  // uint32_t leftPoint = xyToIndex(x - 1, y - 1);
  // count += m_cells.get(leftPoint);
  // count += m_cells.get(leftPoint + 1);
  // count += m_cells.get(leftPoint + 2);
  // leftPoint += c_sideLength;
  // count += m_cells.get(leftPoint);
  // count += m_cells.get(leftPoint + 2);
  // leftPoint += c_sideLength;
  // count += m_cells.get(leftPoint);
  // count += m_cells.get(leftPoint + 1);
  // return count + m_cells.get(leftPoint + 2);
  return 0;
}

/*
 * Chunk method definitions
 */

GameBoard::Chunk::Chunk(GameBoard &gb, uint32_t x, uint32_t y)
    : m_gb(gb), c_x(x), c_y(y) {
  std::cout << "x: " << x << " y: " << y << std::endl;
}

bool GameBoard::Chunk::calcNextCellStatus(int x, int y) {
  bool alive = m_gb.getPoint(x, y);
  uint8_t neighbors = m_gb.countNeighbors(x, y);

  return neighbors == 3 || (alive && neighbors == 2);
}

void GameBoard::Chunk::processNextState() {
  uint8_t hotBuffer = 1;
  uint32_t y, x;
  std::array<std::bitset<Chunk::Size>, 2> buffers;

  // put the first row into the buffer
  for (x = 0; x < Chunk::Size; x++) {
    buffers[0][x] = calcNextCellStatus(x, 0);
  }

  for (y = 0; y < Chunk::Size; y++) {
    // write upcoming line into the buffer (y+1)
    if ((y + 1) < Chunk::Size) {
      for (x = 0; x < Chunk::Size; x++) {
        buffers[hotBuffer][x] = calcNextCellStatus(x, y + 1);
      }
    }

    hotBuffer ^= 1; // switch active buffer before writing

    // write hot buffer the the board (y)
    for (x = 0; x < Chunk::Size; x++) {
      bool value = buffers[hotBuffer][x];
      //  if (!addedToBorderBuffer(x, y, value)) {
      //    m_gb.setPoint(x + c_offsetX, y + c_offsetY, value);
      //  }
    }
    // row y is written to the board
  }
}

void GameBoard::Chunk::nextState() { m_currBuffer ^= 1; }

std::bitset<GameBoard::Chunk::Size> &GameBoard::Chunk::operator[](int32_t i) {
  return m_dataBuffer[m_currBuffer][i];
}
