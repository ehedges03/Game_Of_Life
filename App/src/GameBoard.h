#pragma once
#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>

class PairHash {
public:
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return (53 + h1) * 53 + h2;
  }
};

class PairEqual {
public:
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return (53 + h1) * 53 + h2;
  }
};

/**
 * Main gameboard structure for working with chunks and controlling the system.
 */
class GameBoard {
public:
  GameBoard() : m_chunks(){};
  void setPoint(int32_t x, int32_t y, bool value);
  bool getPoint(int32_t x, int32_t y);

  friend std::ostream &operator<<(std::ostream &o, GameBoard &g);
private:
  /**
   * 64 x 64 block of bits for gameboard.
   */
  class Chunk;
  std::unordered_map<std::pair<int32_t, int32_t>, Chunk, PairHash> m_chunks;
  int32_t m_maxX = 0;
  int32_t m_minX = 0;
  int32_t m_maxY = 0;
  int32_t m_minY = 0;

  // helper function used by nextPointStatus
  uint8_t countNeighbors(uint32_t x, uint32_t y);
};

class GameBoard::Chunk {

public:
  static constexpr int32_t Size = 16;
  Chunk(GameBoard &gb, uint32_t x, uint32_t y);

  bool calcNextCellStatus(int32_t x, int32_t y);
  void processNextState();
  void nextState();

  std::bitset<Size> &operator[](int32_t i);

  friend std::ostream &operator<<(std::ostream &o, Chunk c) {
    for (auto r : c.m_dataBuffer[c.m_currBuffer]) {
      o << r << std::endl;
    }
    return o;
  }

private:
  GameBoard &m_gb;
  std::array<std::array<std::bitset<Size>, Size>, 2> m_dataBuffer;
  uint32_t m_currBuffer = 0;

  const uint32_t c_x;
  const uint32_t c_y;
};
