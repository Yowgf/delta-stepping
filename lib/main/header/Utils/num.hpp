//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of class ~num~. This class contains
// utilities for handling numerical values, such as checking if it
// is in the specified range.
//===----------------------------------------------------------===//

#ifndef NUM_H
#define NUM_H

#include <stdexcept>
#include <string>

namespace Utils {

template <typename numType>
class num {
public:
  static inline void checkInRange(const numType& n, const numType& l,
                      const numType& r) noexcept(false)
  {
    if (n < l || n > r) {
      throw std::logic_error{
        std::to_string(n) + " out of range (" + std::to_string(l) +
          ", " + std::to_string(r) + ")"};
    }
  }

  static inline numType max(const numType& a, const numType& b)
  {
    return a > b ? a : b;
  }

  static inline numType min(const numType& a, const numType& b)
  {
    return a < b ? a : b;
  }
};

}

#endif
