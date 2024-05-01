#pragma once
#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
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
 * 64 x 64 block of bits for gameboard.
 */
template <uint32_t size> class Chunk;

/**
 * Main gameboard structure for working with chunks and controlling the system.
 */
template <uint32_t ChunkSize = 64> class GameBoard {
public:
  void setPoint(int32_t x, int32_t y, bool value);
  bool getPoint(int32_t x, int32_t y);

  void update();

  friend std::ostream &operator<<(std::ostream &o, GameBoard<ChunkSize> &g);

private:
  friend class Chunk<ChunkSize>;

  std::unordered_map<std::pair<int32_t, int32_t>, Chunk<ChunkSize>, PairHash>
      m_chunks;

  int32_t m_maxX = 0;
  int32_t m_minX = 0;
  int32_t m_maxY = 0;
  int32_t m_minY = 0;

  std::optional<std::reference_wrapper<Chunk<ChunkSize>>> getChunk(int32_t x,
                                                                   int32_t y);
  Chunk<ChunkSize> &getOrMakeChunk(int32_t x, int32_t y);
};

template <uint32_t Size> class Chunk {

public:
  Chunk() {};

  struct Border {
    bool topLeft;
    bool topRight;
    bool bottomLeft;
    bool bottomRight;
    std::bitset<Size> top;
    std::bitset<Size> bottom;
    std::bitset<Size> left;
    std::bitset<Size> right;
  };

  Border border;

  void processNextState();
  void swapToNextState();
  bool empty();

  uint32_t size() const { return Size; }

  using iterator = typename std::array<std::bitset<Size>, Size>::iterator;
  using const_iterator =
      typename std::array<std::bitset<Size>, Size>::const_iterator;

  std::bitset<Size> &operator[](int32_t i);
  const std::bitset<Size> &operator[](int32_t i) const;

  iterator begin() { return m_dataBuffer[m_currBuffer].begin(); };
  const_iterator begin() const { return m_dataBuffer[m_currBuffer].begin(); };

  iterator end() { return m_dataBuffer[m_currBuffer].end(); };
  const_iterator end() const { return m_dataBuffer[m_currBuffer].end(); };

  friend std::ostream &operator<<(std::ostream &o, Chunk<Size> c);

private:
  std::array<std::array<std::bitset<Size>, Size>, 2> m_dataBuffer;
  uint32_t m_currBuffer = 0;
};
