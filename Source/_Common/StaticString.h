#pragma once

#include <array>
#include <string>

// In a world where we use Boost, we would use
// https://www.boost.org/doc/libs/1_75_0/libs/static_string/doc/html/index.html
// instead

class CAr;

template<size_t N>
class StaticString {
public:
  StaticString() = default;

  template<typename ... Ts>
  StaticString(LPCTSTR lpszFormat, Ts && ... ts) {
    _vsntprintf(buffer.data(), N, lpszFormat, std::forward(ts)...);
  }

  operator const char * () const {
    return buffer.data();
  }

  [[nodiscard]] const char * GetRawStr() const {
    return buffer.data();
  }

  [[nodiscard]] std::string GetStdString() const {
    return std::string(buffer.data());
  }

  [[nodiscard]] CString GetCString() const {
    return CString(buffer.data());
  }

  template<size_t M> friend CAr & operator<<(CAr &, const StaticString<M> &);
  template<size_t M> friend CAr & operator>>(CAr &, StaticString<M> &);

  template<typename ... Ts>
  void Format(LPCTSTR lpszFormat, Ts && ... ts) {
    // TODO: what is the diff between sntprintf and wsprintf?
    _sntprintf(buffer.data(), N - 1, lpszFormat, std::forward<Ts>(ts)...);
  }

  StaticString & operator=(const char * str) {
    StringCchCopy(buffer.data(), N, str);
    return *this;
  }

  [[nodiscard]] char * GetBuffer() { return buffer.data(); }

private:
  std::array<char, N> buffer = { '\0' };
};

