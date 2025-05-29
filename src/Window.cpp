#include "Window.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

unsigned int Window::s_numOfWindows = 0;

Window::Window(const char *title, int width, int height) {
  if (s_numOfWindows++ == 0 && !glfwInit()) {
    throw std::runtime_error("Failed to intialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (m_window == nullptr) {
    const char *description;
    glfwGetError(&description);
    glfwTerminate();
    throw std::runtime_error("Failed to Create Window Error: " +
                             std::string(description));
  }

  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, Window::keyCallbackBridge);
  glfwSetMouseButtonCallback(m_window, Window::mouseButtonCallbackBridge);
  glfwSetCursorPosCallback(m_window, Window::cursorPosCallbackBridge);
  glfwSetScrollCallback(m_window, Window::scrollCallbackBridge);
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

std::pair<int, int> Window::getFramebufferSize() const {
  int width, height;
  glfwGetFramebufferSize(m_window, &width, &height);
  return {width, height};
}

void Window::pollEvents() { glfwPollEvents(); }

bool Window::keyPressed(int key) {
  return glfwGetKey(m_window, key) == GLFW_PRESS;
}

CallbackId Window::addKeyCallback(KeyCallback callback) {
  CallbackId id = m_nextKeyCbId++;
  m_keyCallbacks[id] = std::move(callback);
  return id;
}

void Window::removeKeyCallback(CallbackId id) { m_keyCallbacks.erase(id); }

CallbackId Window::addMouseButtonCallback(MouseButtonCallback callback) {
  CallbackId id = m_nextMouseButtonCbId++;
  m_mouseButtonCallbacks[id] = std::move(callback);
  return id;
}

void Window::removeMouseButtonCallback(CallbackId id) {
  m_mouseButtonCallbacks.erase(id);
}

CallbackId Window::addCursorPosCallback(CursorPosCallback callback) {
  CallbackId id = m_nextCursorPosCbId++;
  m_cursorPosCallbacks[id] = std::move(callback);
  return id;
}

void Window::removeCursorPosCallback(CallbackId id) {
  m_cursorPosCallbacks.erase(id);
}

CallbackId Window::addScrollCallback(ScrollCallback callback) {
  CallbackId id = m_nextScrollCbId++;
  m_scrollCallbacks[id] = std::move(callback);
  return id;
}

void Window::removeScrollCallback(CallbackId id) {
  m_scrollCallbacks.erase(id);
}

void Window::keyCallbackBridge(GLFWwindow *window, int key, int scancode,
                               int action, int mods) {
  Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
#ifndef NDEBUG
  assert(instance);
#else
  if (!instance)
    return;
#endif

  for (auto cb : instance->m_keyCallbacks) {
    cb.second(*instance, key, scancode, action, mods);
  }
}

void Window::mouseButtonCallbackBridge(GLFWwindow *window, int button,
                                       int action, int mods) {
  Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
#ifndef NDEBUG
  assert(instance);
#else
  if (!instance)
    return;
#endif

  for (auto cb : instance->m_mouseButtonCallbacks) {
    cb.second(*instance, button, action, mods);
  }
}

void Window::cursorPosCallbackBridge(GLFWwindow *window, double xpos,
                                     double ypos) {
  Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
#ifndef NDEBUG
  assert(instance);
#else
  if (!instance)
    return;
#endif

  for (auto cb : instance->m_cursorPosCallbacks) {
    cb.second(*instance, xpos, ypos);
  }
}

void Window::scrollCallbackBridge(GLFWwindow *window, double xoffset,
                                  double yoffset) {
  Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
#ifndef NDEBUG
  assert(instance);
#else
  if (!instance)
    return;
#endif

  for (auto cb : instance->m_scrollCallbacks) {
    cb.second(*instance, xoffset, yoffset);
  }
}
