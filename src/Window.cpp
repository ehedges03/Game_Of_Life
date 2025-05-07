#include "Window.h"
#include <iostream>
#include <stdexcept>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

unsigned int Window::s_numOfWindows = 0;

Window::Window(const char *name, int width, int height) {
  if (s_numOfWindows++ == 0) {
    glfwInit();
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);

  if (m_window == nullptr) {
    const char *description;
    glfwGetError(&description);
    glfwTerminate();
    throw new std::runtime_error("Failed to Create Window Error: " +
                                 std::string(description));
  }

  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
}

Window::~Window() {
  glfwDestroyWindow(m_window);
  if (--s_numOfWindows == 0) {
    glfwTerminate();
  }
}

void Window::close() { glfwSetWindowShouldClose(m_window, true); }

bool Window::shouldClose() { return glfwWindowShouldClose(m_window); }

void Window::swapBuffers() { glfwSwapBuffers(m_window); }

void Window::pollEvents() { glfwPollEvents(); }

bool Window::keyPressed(int key) {
  return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Window::setKeyCallback(GLFWkeyfun callback) {
  glfwSetKeyCallback(m_window, callback);
}

void Window::setMouseButtonCallback(GLFWmousebuttonfun callback) {
  glfwSetMouseButtonCallback(m_window, callback);
}

void Window::setCursorPosCallback(GLFWcursorposfun callback) {
  glfwSetCursorPosCallback(m_window, callback);
}

void Window::setScrollCallback(GLFWscrollfun callback) {
  glfwSetScrollCallback(m_window, callback);
}
