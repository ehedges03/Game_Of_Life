#include <cstdint>
#include <iostream>

namespace Console {

class Cursor {
public:
  static void setPosition(uint32_t line, uint32_t column) {
    std::cout << "\033[" << line << ";" << column << "f" << std::flush;
  }

  static void up(uint32_t lines) {
    std::cout << "\033[" << lines << "A" << std::flush;
  }

  static void down(uint32_t lines) {
    std::cout << "\033[" << lines << "B" << std::flush;
  }

  static void forward(uint32_t columns) {
    std::cout << "\033[" << columns << "C" << std::flush;
  }

  static void backward(uint32_t columns) {
    std::cout << "\033[" << columns << "D" << std::flush;
  }

  static void savePosition() { std::cout << "\033[s" << std::flush; }

  static void restorePosition() { std::cout << "\033[u" << std::flush; }
};

class Screen {
public:
  static void clear() { std::cout << "\033[2J" << std::flush; }

  static void eraseToEndOfLine() { std::cout << "\033[K" << std::flush; }
};

}; // namespace Console
