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
  static void checkInRange(const numType& n, const numType& l, 
                      const numType& r) noexcept(false)
  {
    if (n < l || n > r) {
      throw std::logic_error{
        std::to_string(n) + " out of range (" + std::to_string(l) +
          ", " + std::to_string(r) + ")"};
    }
  }
};

}

#endif
