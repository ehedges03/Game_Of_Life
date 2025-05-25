#include <bitset>
#include <chrono>
#include <iostream>

#include "BitArray.h"
#include "Chunk.h"
#include "GameBoard.h"
#include "LibFunni/log.h"
#include "Shader.h"
#include "Window.h"
#include "utils/Console.h"
#include "utils/WrappedPoint.h"

void simpleBitArrayTest();
void simpleChunkTest();
void simpleGameBoardTest();
void simpleWrappedPointTest();
void simpleLoggerTest();
void simpleGLFWWindow();

int main() {
  simpleBitArrayTest();
  simpleWrappedPointTest();
  // simpleChunkTest();
  // simpleGameBoardTest();
  simpleLoggerTest();
  simpleGLFWWindow();
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
constexpr std::array<std::bitset<8>, 8> chunkStart = {
    0b00000000, 
    0b00000000, 
    0b00000010,
    0b00001011, 
    0b00001010, 
    0b00001000,
    0b00100000, 
    0b10100000, 
};
// clang-format on

void simpleChunkTest() {
  Chunk c;
  char input;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      c.setCell(x, y, chunkStart[7 - y][7 - x]);
    }
  }

  Console::Screen::clear();
  Console::Cursor::setPosition(0, 0);
  std::cout << c << std::endl;
  std::cin.get(input);

  while (input != 'q') {
    c.processNextState();

    Console::Screen::clear();
    Console::Cursor::setPosition(0, 0);
    std::cout << c << std::endl;
    std::cin.get(input);
  }
}

void simpleGameBoardTest() {
  GameBoard gb;
  char input;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      gb.setPoint(x, y, chunkStart[7 - y][7 - x]);
    }
  }

  std::cout << gb << std::endl;
  std::cin.get(input);

  double lastTimeMuS = 0, averageTimeMuS = 0;
  uint32_t runs = 0;

  while (input != 'q') {
    auto start = std::chrono::steady_clock::now();
    gb.update();
    auto end = std::chrono::steady_clock::now();

    lastTimeMuS =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count() /
        1000.0f;
    runs++;
    averageTimeMuS = ((runs - 1) * averageTimeMuS + lastTimeMuS) / runs;

    // if (runs % 10000 == 0) {
    std::cout << gb << std::flush;
    std::cout << "Run " << runs << " | Time: " << lastTimeMuS
              << " micro sec | Avg time: " << (averageTimeMuS) << " micro sec"
              << std::endl;
    std::cin.get(input);
    // }
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

void processInput(Window &window) {
  if (window.keyPressed(GLFW_KEY_ESCAPE)) {
    window.close();
  }
}

void simpleGLFWWindow() {
  Window gameWindow("Game Of Life", 800, 600);
  // clang-format off
  float vertices[] = {
    // First triangle
    0.0f, 0.0f, 0.0f, 0.0f, // top left
    1.0f, 0.0f, 1.0f, 0.0f, // top right
    0.0f, -1.0f, 0.0f, 1.0f, // bottom left

    // Second triangle
    0.0f, -1.0f, 0.0f, 1.0f, // bottom left
    1.0f, 0.0f, 1.0f, 0.0f, // top right
    1.0f, -1.0f, 1.0f, 1.0f  // bottom right
  };
  // clang-format on

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  Shader shaderProgram("chunk.vert", "chunk.frag");

  GLint loc = shaderProgram.getUniformLocation("uData");

  uint32_t data[] = {0xAA55AA55, 0x55AA55AA};

  shaderProgram.use();
  glUniform1uiv(loc, 2, data);

  while (!gameWindow.shouldClose()) {
    processInput(gameWindow);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    gameWindow.swapBuffers();
    Window::pollEvents();
  }
}
