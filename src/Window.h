#pragma once
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
#include <cstddef>
#include <functional>
#include <unordered_map>

class Window;

using KeyCallback = std::function<void(Window &window, int key, int scancode,
                                       int action, int mods)>;
using MouseButtonCallback =
    std::function<void(Window &window, int button, int action, int mods)>;
using CursorPosCallback =
    std::function<void(Window &window, double xpos, double ypos)>;
using ScrollCallback =
    std::function<void(Window &window, double xoffset, double yoffset)>;

using CallbackId = std::size_t;

class Window {
public:
  static void pollEvents();

  Window(const char *title, int width, int height);
  ~Window();

  void close();
  bool shouldClose();
  void swapBuffers();
  std::pair<int, int> getFramebufferSize() const;

  bool keyPressed(int key);
  CallbackId addKeyCallback(KeyCallback callback);
  void removeKeyCallback(CallbackId id);
  CallbackId addMouseButtonCallback(MouseButtonCallback callback);
  void removeMouseButtonCallback(CallbackId id);
  CallbackId addCursorPosCallback(CursorPosCallback callback);
  void removeCursorPosCallback(CallbackId id);
  CallbackId addScrollCallback(ScrollCallback callback);
  void removeScrollCallback(CallbackId id);

private:
  static unsigned int s_numOfWindows;
  GLFWwindow *m_window;
  CallbackId m_nextKeyCbId;
  std::unordered_map<CallbackId, KeyCallback> m_keyCallbacks;
  CallbackId m_nextMouseButtonCbId;
  std::unordered_map<CallbackId, MouseButtonCallback> m_mouseButtonCallbacks;
  CallbackId m_nextCursorPosCbId;
  std::unordered_map<CallbackId, CursorPosCallback> m_cursorPosCallbacks;
  CallbackId m_nextScrollCbId;
  std::unordered_map<CallbackId, ScrollCallback> m_scrollCallbacks;

  static void keyCallbackBridge(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouseButtonCallbackBridge(GLFWwindow* window, int button, int action, int mods);
  static void cursorPosCallbackBridge(GLFWwindow* window, double xpos, double ypos);
  static void scrollCallbackBridge(GLFWwindow* window, double xoffset, double yoffset);
};
