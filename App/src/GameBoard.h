#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "Chunk.h"

#define VISUALIZE_BORDERS 0
#define VISUALIZE_DEFAULT 1
#define VISUALIZE VISUALIZE_DEFAULT
#define PRINT_GB true

// These have to be able to print as one character wide otherwise it will break
// the print
#ifdef _WIN32

#define ALIVE_CELL "#"
#define DEAD_CELL "-"

#else

#define ALIVE_CELL "▓"
#define DEAD_CELL "0"

#endif

struct ChunkKey {
  ChunkKey(int32_t x, int32_t y) : x(x), y(y) {}
  ChunkKey(std::array<int32_t, 2> p) : x(p[0]), y(p[1]) {}

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

  /**
   * Take a general (x,y) coordinate and find the chunk that it cooresponds
   * with.
   */
  ChunkKey calcChunkKey(int32_t x, int32_t y);
  void makeChunk(ChunkKey key);
  /**
   * Delets a given chunk's border connections
   */
  void deleteChunkBorders(std::shared_ptr<Chunk> c);
  std::shared_ptr<Chunk> getChunk(ChunkKey key);
  std::shared_ptr<Chunk> getOrMakeChunk(ChunkKey key);
  void makeBorderChunks(ChunkKey key, std::shared_ptr<Chunk> c);
};