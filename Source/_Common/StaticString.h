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

private:
  std::array<char, N> buffer = { '\0' };
};

