#pragma once
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

class Window {
public:
  static void pollEvents();

  Window(const char *name, int height, int width);
  ~Window();

  void close();
  bool shouldClose();
  void swapBuffers();

  bool keyPressed(int key);
  void setKeyCallback(GLFWkeyfun callback);
  void setMouseButtonCallback(GLFWmousebuttonfun callback);
  void setCursorPosCallback(GLFWcursorposfun callback);
  void setScrollCallback(GLFWscrollfun callback);

private:
  static unsigned int s_numOfWindows;
  GLFWwindow *m_window;
};
