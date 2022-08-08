#pragma once

#include <algorithm>

namespace sqktd {
  namespace _ {
    template<typename T, T Low, T High, T Speed>
    class Flasher {
      T m_currentValue;
      bool m_increasing;

    public:
      constexpr explicit Flasher(T initialValue = High)
        : m_currentValue(std::clamp(initialValue, Low, High)),
        m_increasing(m_currentValue == Low) {
      }

      constexpr T Increment() {
        if (m_increasing) {
          m_currentValue += Speed;
          if (m_currentValue >= High) {
            m_currentValue = High;
            m_increasing = false;
          }
        } else {
          m_currentValue -= Speed;
          if (m_currentValue <= Low) {
            m_currentValue = Low;
            m_increasing = true;
          }
        }

        return Get();
      }
      
      [[nodiscard]] constexpr T Get() const noexcept { return m_currentValue; }
      [[nodiscard]] constexpr void Set(T value) { m_currentValue = std::clamp(value, Low, High); }
    };
  }

  /// A class designed for the blinking mechanic
  /// 
  /// Assumptions:
  /// - Low <= High
  /// - No underflow/overflow is possible
  template<auto Low, auto High, auto Speed>
  using Flasher = _::Flasher<decltype(Low), Low, High, Speed>;

  // Indirection is to ensure that Low, High and Speed are of the same type.

}

