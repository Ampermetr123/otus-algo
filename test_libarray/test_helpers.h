#pragma once

#include <string>
#include <sstream>

template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
  if (!(t == u)) {
    std::ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
      os << " hint: " << hint;
    }
    throw std::runtime_error(os.str());
  }
}

inline void Assert(bool b, const std::string& hint) {
  AssertEqual(b, true, hint);
}

#define ASSERT_EQUAL(x, y) {            \
  std::ostringstream _os;                \
  _os << #x << " != " << #y << ", "      \
    << __FILE__ << ":" << __LINE__;     \
  AssertEqual(x, y, _os.str());          \
}

#define ASSERT_TRUE(x) {                     \
  std::ostringstream _os;                \
  _os << #x << " is false, "             \
    << __FILE__ << ":" << __LINE__;     \
  Assert(x, _os.str());                  \
}
