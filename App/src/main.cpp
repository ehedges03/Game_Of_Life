#include "BitArray.h"
#include "GameBoard.h"
#include "LibFunni/log.h"
#include "Utils/Console.h"
#include "Utils/WrappedPoint.h"
#include <chrono>
#include <iostream>

void simpleBitArrayTest();
void simpleChunkTest();
void simpleGameBoardTest();
void simpleWrappedPointTest();
void simpleLoggerTest();

int main() {
  simpleBitArrayTest();
  simpleWrappedPointTest();
  // simpleChunkTest();
  simpleGameBoardTest();
  simpleLoggerTest();
}

void simpleBitArrayTest() {
  BitArray mybits(9);

  std::cout << mybits << '\n';

  mybits.set(0, true);
  mybits.set(2, true);
  mybits.set(4, true);
  mybits.set(6, true);
  mybits.set(8, true);

  std::cout << mybits << '\n';

  mybits.set(0, false);
  mybits.set(4, false);
  mybits.set(8, false);

  std::cout << mybits << '\n';
}

// clang-format off
constexpr std::array<uint16_t, 16> chunkStart = {
    0b0000000000000000, 
    0b0000000000000000, 
    0b0000000000000000,
    0b0000000000000000, 
    0b0110000000000000, 
    0b0110000000000000,
    0b0000000000000000, 
    0b0000000000000000, 
    0b0000000000000000,
    0b0000000000000000, 
    0b0000000000000000, 
    0b0000000000000000,
    0b0000000000000000, 
    0b0000000000000000, 
    0b0000000000000000,
    0b0000000000000000,
};
// clang-format on

void simpleChunkTest() {
  Chunk c;
  char input;

  Console::Screen::clear();
  Console::Cursor::setPosition(0, 0);
  std::cout << c << std::endl;
  std::cin.get(input);
  for (int y = 0; y < Chunk::Size; y++) {
    for (int x = 0; x < Chunk::Size; x++) {
      c.setCell(x, y, (x + y) % 2);
      Console::Screen::clear();
      Console::Cursor::setPosition(0, 0);
      std::cout << c << std::endl;
      std::cin.get(input);
    }
  }

  // while (input != 'q') {
  //   c.processNextState();

  //   Console::Screen::clear();
  //   Console::Cursor::setPosition(0, 0);
  //   std::cout << c << std::endl;
  //   std::cin.get(input);
  // }
}

void simpleGameBoardTest() {
  GameBoard gb;
  char input;

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 32; x++) {
      gb.setPoint(x, y, (x + y) % 2);
      std::cout << gb << std::flush;
      std::cin.get(input);
    }
  }
  std::cout << gb << std::endl;
  std::cin.get(input);

  double lastTimeMS = 0, totalTimeMS = 0;
  int runs = 0;

  while (input != 'q') {
    auto start = std::chrono::steady_clock::now();
    gb.update();
    auto end = std::chrono::steady_clock::now();

    lastTimeMS =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count() /
        1000.0f;
    totalTimeMS += lastTimeMS;
    runs++;

    std::cout << gb << std::flush;
    std::cout << "Last run time: " << lastTimeMS
              << " ms | Total run time: " << (totalTimeMS / runs) << " ms"
              << std::endl;
    std::cin.get(input);
  }
}

void simpleWrappedPointTest() {
  // test WrappedPoint -- imagine all the unit tests yay

  WrappedPoint testWPoint1({-2, -6}, {5, 5});
  WrappedPoint testWPoint2({5, 15}, {5, 5});

  bool result1 = (testWPoint1.x() == 3) && (testWPoint1.y() == 4);
  bool result2 = (testWPoint2.x() == 0) && (testWPoint2.y() == 0);

  std::cout << '\n'
            << "negative wrap: " << (result1 ? "success " : "failed ")
            << testWPoint1.x() << ", " << testWPoint1.y();
  std::cout << '\n'
            << "positive wrap: " << (result2 ? "success " : "failed ")
            << testWPoint2.x() << ", " << testWPoint2.y();
}

void simpleLoggerTest() {
  // only logd will function
  funni::Logger<true, false, false, false> logger("logtest");

  std::cout << "\n\n";

  logger.Start();

  logger.logd();
  logger.logi();
  logger.logw();
  logger.logi();
}
