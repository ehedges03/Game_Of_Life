#include "GameBoard.h"
#include "Utils/Console.h"
#include <bitset>
#include <iostream>
#include <utility>
#include <limits>

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