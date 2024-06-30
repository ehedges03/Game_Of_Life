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

bool GameBoard::startThreads() {

  if (m_threadsStarted) {
    std::cout << "WARN: attempted to start started threads" << '\n';
    return false;
  }

  for (int i = 0; i < NUM_WORKERS; i++) {
    m_threads[i] = std::thread(std::bind(&GameBoard::processChunks, this, std::placeholders::_1), i);
  }

  m_threadsStarted = true;
  return true;
}

bool GameBoard::stopThreads() {
  if (!m_threadsStarted) {
    std::cout << "WARN: attempted to stop stopped threads" << '\n';
    return false;
  }

  // signal each thread to stop !!!HAZARD: if this signal is sent while processing chunks, then it might be missed -- address this somehow
  // I think I want some kind of check-in (semaphore?) from each thread that confirms it has completed its work after getting signaled
  // threads should have a timeout on the wait (wait_for) and a predicate that only does chunk work if not terminated && updating (updating/idle statuses are key to differentiate timeouts from signals)
  {
    std::lock_guard lock(m_cvMutex);
    m_tSig = Tsig::Terminate;
  }
  m_signalThreadsCv.notify_all();

  // join each thread
  for (auto& worker : m_threads) {
    worker.join();
  }

  m_threadsStarted = false;
  return true;
}

void GameBoard::processChunks(int order) {

  while (1) {
    std::unique_lock lock(m_cvMutex);
    m_signalThreadsCv.wait(lock);

    if (m_tSig == Tsig::Terminate) {
      break;
    }
    if (m_tSig == Tsig::Idle) {
      continue;
    }
    lock.unlock();

    // TODO: update the appropriate chunks
    
  }

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

// TODO: move this into Chunk
void GameBoard::deleteChunkBorders(std::shared_ptr<Chunk> c) {
  if (!c)
    return;

  if (c->m_upLeft)
    c->m_upLeft->m_downRight = nullptr;

  if (c->m_up)
    c->m_up->m_down = nullptr;

  if (c->m_upRight)
    c->m_upRight->m_downLeft = nullptr;

  if (c->m_left)
    c->m_left->m_right = nullptr;

  if (c->m_right)
    c->m_right->m_left = nullptr;

  if (c->m_downLeft)
    c->m_downLeft->m_upRight = nullptr;

  if (c->m_down)
    c->m_down->m_up = nullptr;

  if (c->m_downRight)
    c->m_downRight->m_upLeft = nullptr;
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
  chunk->m_upLeft = getChunk({key.x - 1, key.y + 1});
  chunk->m_up = getChunk({key.x, key.y + 1});
  chunk->m_upRight = getChunk({key.x + 1, key.y + 1});
  chunk->m_left = getChunk({key.x - 1, key.y});
  chunk->m_right = getChunk({key.x + 1, key.y});
  chunk->m_downLeft = getChunk({key.x - 1, key.y - 1});
  chunk->m_down = getChunk({key.x, key.y - 1});
  chunk->m_downRight = getChunk({key.x + 1, key.y - 1});

  // For each border chunk reference this chunk in it
  if (chunk->m_upLeft)
    chunk->m_upLeft->m_downRight = chunk;

  if (chunk->m_up)
    chunk->m_up->m_down = chunk;

  if (chunk->m_upRight)
    chunk->m_upRight->m_downLeft = chunk;

  if (chunk->m_left)
    chunk->m_left->m_right = chunk;

  if (chunk->m_right)
    chunk->m_right->m_left = chunk;

  if (chunk->m_downLeft)
    chunk->m_downLeft->m_upRight = chunk;

  if (chunk->m_down)
    chunk->m_down->m_up = chunk;

  if (chunk->m_downRight)
    chunk->m_downRight->m_upLeft = chunk;
}

void GameBoard::makeBorderChunks(ChunkKey key, std::shared_ptr<Chunk> c) {
  if (!c->m_upLeft) {
    makeChunk({key.x - 1, key.y + 1});
  }

  if (!c->m_up) {
    makeChunk({key.x, key.y + 1});
  }

  if (!c->m_upRight) {
    makeChunk({key.x + 1, key.y + 1});
  }

  if (!c->m_left) {
    makeChunk({key.x - 1, key.y});
  }

  if (!c->m_right) {
    makeChunk({key.x + 1, key.y});
  }

  if (!c->m_downLeft) {
    makeChunk({key.x - 1, key.y - 1});
  }

  if (!c->m_down) {
    makeChunk({key.x, key.y - 1});
  }

  if (!c->m_downRight) {
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