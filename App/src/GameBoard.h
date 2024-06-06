#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>

struct ChunkKey {
  ChunkKey(int32_t x, int32_t y) : x(x), y(y) {}
  ChunkKey(std::pair<int32_t, int32_t> p) : x(p.first), y(p.second) {}

  int32_t x;
  int32_t y;

  bool operator==(ChunkKey &key) {
    return this->x == key.x && this->y == key.y;
  }

  bool operator==(const ChunkKey &key) const {
    return this->x == key.x && this->y == key.y;
  }
};

class ChunkKeyHash {
public:
  std::size_t operator()(const ChunkKey &c) const {
    auto h1 = std::hash<int32_t>{}(c.x);
    auto h2 = std::hash<int32_t>{}(c.y);

    return (53 + h1) * 53 + h2;
  }
};

/**
 * 64 x 64 block of bits for gameboard.
 */
class Chunk;

/**
 * Main gameboard structure for working with chunks and controlling the system.
 */
class GameBoard {
public:
  void setPoint(int32_t x, int32_t y, bool value);
  bool getPoint(int32_t x, int32_t y);

  void update();

  friend std::ostream &operator<<(std::ostream &o, GameBoard &g);

private:
  friend class Chunk;

  std::unordered_map<ChunkKey, std::shared_ptr<Chunk>, ChunkKeyHash> m_chunks;

  int32_t m_maxX = std::numeric_limits<int32_t>::min();
  int32_t m_minX = std::numeric_limits<int32_t>::max();
  int32_t m_maxY = std::numeric_limits<int32_t>::min();
  int32_t m_minY = std::numeric_limits<int32_t>::max();

  /**
   * Take a general (x,y) coordinate and find the chunk that it cooresponds
   * with.
   */
  ChunkKey calcChunkKey(int32_t x, int32_t y);
  /**
   * Checks a given chunk to see if it is empty and if it is removes it.
   */
  void checkChunk(ChunkKey key, std::shared_ptr<Chunk> c);
  std::shared_ptr<Chunk> getChunk(ChunkKey key);
  std::shared_ptr<Chunk> getOrMakeChunk(ChunkKey key);
};

class Chunk {
public:
  std::shared_ptr<Chunk> upLeft;
  std::shared_ptr<Chunk> up;
  std::shared_ptr<Chunk> upRight;
  std::shared_ptr<Chunk> left;
  std::shared_ptr<Chunk> right;
  std::shared_ptr<Chunk> downLeft;
  std::shared_ptr<Chunk> down;
  std::shared_ptr<Chunk> downRight;

  // I think this should size should be 6 bits under the type used in m_data for
  // each row.
  static constexpr int32_t k_size = 8;
  static constexpr int32_t k_topBorder = k_size + 1;
  static constexpr int32_t k_bottomBorder = 0;
  static constexpr uint64_t k_leftBorderBit = 1ul << (k_size + 1);
  static constexpr uint64_t k_rightBorderBit = 1ul;
  static constexpr uint64_t k_dataBits = ((1ul << k_size) - 1) << 1;

  void processNextState();
  void readInBorder();
  bool empty();

  bool getCell(int32_t x, int32_t y);
  void setCell(int32_t x, int32_t y, bool val);

  using iterator = typename std::array<uint64_t, k_size + 2>::iterator;
  using reverse_iterator =
      typename std::array<uint64_t, k_size + 2>::reverse_iterator;
  using const_iterator =
      typename std::array<uint64_t, k_size + 2>::const_iterator;
  using const_reverse_iterator =
      typename std::array<uint64_t, k_size + 2>::const_reverse_iterator;

  iterator begin() { return m_data.begin(); };
  const_iterator begin() const { return m_data.begin(); };

  reverse_iterator rbegin() { return m_data.rbegin(); };
  const_reverse_iterator rbegin() const { return m_data.rbegin(); };

  iterator end() { return m_data.end(); };
  const_iterator end() const { return m_data.end(); };

  reverse_iterator rend() { return m_data.rend(); };
  const_reverse_iterator rend() const { return m_data.rend(); };

  friend std::ostream &operator<<(std::ostream &o, Chunk &c);

private:
  std::array<uint64_t, k_size + 2> m_data {};
};
