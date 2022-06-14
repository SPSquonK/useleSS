#pragma once

// SquonK, 2022-05
// Class for an object that can only be accessed in a thread safe manner.

#include <mutex>
#include <type_traits>

namespace sqktd {
  template <typename T, typename Mutex = std::mutex>
  class mutexed_object {
  private:
    T m_value;
    Mutex m_mutex;

  public:
    template <typename F>
    void access(F f)
      requires (std::is_invocable_v<F, T &>) {
      std::lock_guard lg(m_mutex);
      f(m_value);
    }
  };

  template <typename T, typename Mutex = std::mutex>
  class mutexed_on_write_object {
  private:
    // TODO: a shared mutex on read
    T m_value;
    Mutex m_mutex;

  public:
    template <typename F>
    void write(F f)
      requires (std::is_invocable_v<F, T &>) {
      std::lock_guard lg(m_mutex);
      f(m_value);
    }

    template <typename F>
    void read(F f)
      requires (std::is_invocable_v<F, const T &>) {
      f(m_value);
    }
  };
}
