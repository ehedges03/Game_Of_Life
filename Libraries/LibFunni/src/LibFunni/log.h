#pragma once

#include <iostream>
#include <string>

// TODO: super unfinished, need to verify the behavior of this juvenile
// metaprogramming monstrosity
namespace funni {

// class selects from one of two explicit template instantiations of an
// implementation for each public method used for generating a log event I...
// think this is all compile time and therefore when a stub is selected it can
// be excluded from the binary by optimization

template <bool enD, bool enI, bool enW, bool enE> class Logger {
public:
  Logger(const std::string &tag) { m_tag = tag; }
  ~Logger() = default;

  Logger(const Logger &) = delete;
  const Logger &operator=(const Logger &) = delete;

  bool Start() {

    std::cout << "testing logging STARTED" << '\n';
    // TODO: launch a new process that writes logs to shell/file
    return true;
  }

  void logd() { 
    if (!enD) return;
    std::cout << "testing logging d" << '\n';
  }

  void logi() { 
    if (!enI) return;
    std::cout << "testing logging i" << '\n';
  }

  void logw() { 
    if (!enW) return;
    std::cout << "testing logging w" << '\n'; 
  }

  void loge() { 
    if (!enE) return;
    std::cout << "testing logging e" << '\n';
  }

private:
  std::string m_tag;
};

} // namespace funni
