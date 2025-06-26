#include <bitset>
#include <chrono>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "BitArray.h"
#include "Chunk.h"
#include "GameBoard.h"
#include "LibFunni/log.h"
#include "Shader.h"
#include "Window.h"
#include "utils/Console.h"
#include "utils/WrappedPoint.h"

struct View {
  float scale;
  float panX;
  float panY;

  View() : scale(1), panX(0), panY(0) {}
  View(float zoom, float panX, float panY)
      : scale(zoom), panX(panX), panY(panY) {}
};

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

constexpr float PAN_DELTA = 5;
constexpr float ZOOM_RATE = 1.1;

void processInput(Window &window, GameBoard &gb, View &view) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  if (window.keyPressed(GLFW_KEY_ESCAPE)) {
    window.close();
  }

  if (window.keyPressed(GLFW_KEY_ENTER)) {
    gb.update();
  }

  // Pan controls - WASD basic control
  if (window.keyPressed(GLFW_KEY_W)) {
    view.panY += PAN_DELTA;
  }
  if (window.keyPressed(GLFW_KEY_A)) {
    view.panX -= PAN_DELTA;
  }
  if (window.keyPressed(GLFW_KEY_S)) {
    view.panY -= PAN_DELTA;
  }
  if (window.keyPressed(GLFW_KEY_D)) {
    view.panX += PAN_DELTA;
  }

  // Zoom controls - Q zoom out, E zoom in
  if (window.keyPressed(GLFW_KEY_E)) {
    view.panX *= ZOOM_RATE;
    view.panY *= ZOOM_RATE;
    view.scale *= ZOOM_RATE;
  }

  if (window.keyPressed(GLFW_KEY_Q)) {
    view.panX /= ZOOM_RATE;
    view.panY /= ZOOM_RATE;
    view.scale /= ZOOM_RATE;
  }
}

void renderGUI(GameBoard &gb, View &view) {
  ImGuiIO &io = ImGui::GetIO();
  // render your GUI
  ImGui::Begin("Game Of Life Controls");

  ImGui::Text("Current Generation: %d", gb.currentGeneration());
  ImGui::SameLine();
  if (ImGui::Button("Progress")) {
    gb.update();
  }

  if (ImGui::Button("Home")) {
    view.panX = 0;
    view.panY = 0;
    view.scale = 1;
  }

  if (ImGui::IsMousePosValid()) {
    ImGui::Text("Mouse Pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
  } else {
    ImGui::Text("Mouse Pos: <INVALID>");
  }

  ImGui::Text("FPS: %g", io.Framerate);
  static bool vsyncEnabled = true;
  if (ImGui::Checkbox("V-Sync", &vsyncEnabled)) {
    if (vsyncEnabled) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }

  ImGui::End();

  // Render dear imgui into screen
  // ImGui::ShowDemoWindow();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void simpleGLFWWindow() {
  GameBoard gb;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      gb.setPoint(x, y, chunkStart[7 - y][7 - x]);
    }
  }

  for (auto [key, chunk] : gb) {
    auto data = chunk->data();
    std::cout << std::hex;
    std::cout << std::setfill('0') << std::setw(8) << data[0] << std::endl;
    std::cout << std::setfill('0') << std::setw(8) << data[1] << std::endl;
    std::cout << std::dec;
  }

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

  GLuint vao;
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

  GLint dataLoc = shaderProgram.getUniformLocation("u_data");
  GLint projectionLoc = shaderProgram.getUniformLocation("u_projection");
  GLint viewLoc = shaderProgram.getUniformLocation("u_view");
  GLint transposeLoc = shaderProgram.getUniformLocation("u_transform");

  gameWindow.addKeyCallback(
      [&gb](Window &window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
          gb.update();
        }
      });

  gameWindow.addMouseButtonCallback(
      [](Window &window, int button, int action, int mods) {
        ImGuiIO &io = ImGui::GetIO();
        std::cout << button << " click occured at: " << io.MousePos.x << ", "
                  << io.MousePos.y << std::endl;
      });

  static const float CHUNK_SIZE = 8.0f;
  std::array<float, 9> projectionMat = {};
  std::array<float, 9> viewMat = {};

  shaderProgram.use();
  View view(1, 100, 100);

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("assets/fonts/doto/Doto-Regular.ttf", 24.0f);

  while (!gameWindow.shouldClose()) {
    processInput(gameWindow, gb, view);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto [width, height] = gameWindow.getFramebufferSize();
    projectionMat[0] = (2.0f) / width;
    projectionMat[4] = (2.0f) / height;
    glUniformMatrix3fv(projectionLoc, 1, true, projectionMat.data());

    viewMat[0] = view.scale;
    viewMat[4] = view.scale;
    viewMat[2] = -view.panX;
    viewMat[5] = -view.panY;
    viewMat[8] = 1.0f;
    glUniformMatrix3fv(viewLoc, 1, true, viewMat.data());

    shaderProgram.use();
    glBindVertexArray(vao);
    for (const auto [key, chunk] : gb) {
      std::array<float, 9> transform = {};
      float originX = key.x * CHUNK_SIZE,
            originY = (key.y * CHUNK_SIZE) + CHUNK_SIZE;
      transform[0] = CHUNK_SIZE;
      transform[2] = originX;
      transform[4] = CHUNK_SIZE;
      transform[5] = originY;
      transform[8] = 1.0f;
      glUniformMatrix3fv(transposeLoc, 1, true, transform.data());

      auto data = chunk->data();
      glUniform1uiv(dataLoc, 2, data.data());

      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    renderGUI(gb, view);

    gameWindow.swapBuffers();
    Window::pollEvents();
  }
}
