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

  void logd() { ilogd<enD>(); }

  void logi() { ilogi<enI>(); }

  void logw() { ilogw<enW>(); }

  void loge() { iloge<enE>(); }

private:
  std::string m_tag;

  // debug
  template <bool en> void ilogd();

  template <> void ilogd<true>() { std::cout << "testing logging d" << '\n'; }

  template <> void ilogd<false>() {
    // no-op
  }

  // info
  template <bool en> void ilogi();

  template <> void ilogi<true>() { std::cout << "testing logging i" << '\n'; }

  template <> void ilogi<false>() {
    // no-op
  }

  // warning
  template <bool en> void ilogw();

  template <> void ilogw<true>() { std::cout << "testing logging w" << '\n'; }

  template <> void ilogw<false>() {
    // no-op
  }

  // error
  template <bool en> void iloge();

  template <> void iloge<true>() { std::cout << "testing logging e" << '\n'; }

  template <> void iloge<false>() {
    // no-op
  }
};

} // namespace funni
