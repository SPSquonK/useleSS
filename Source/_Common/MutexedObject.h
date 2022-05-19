#pragma once

// SquonK, 2022-05
// Class for an object that can only be accessed in a thread safe manner.

#include <mutex>
#include <type_traits>

template <typename T, typename Mutex = std::mutex>
class CMutexedObject {
private:
  T m_value;
  Mutex m_mutex;

public:
  template <typename F>
  void Access(F f)
  requires (std::is_invocable_v<F, T &>) {
    std::lock_guard lg(m_mutex);
    f(m_value);
  }
};

