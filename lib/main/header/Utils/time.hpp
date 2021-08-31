//===----------------------------------------------------------===//
// Utils module
//
// File purpose: useful time functions. Notably, a macro to time the
// execution time of some expression.
//===----------------------------------------------------------===//

#ifndef TIME_H
#define TIME_H

#include <chrono>

#ifdef MEASURE_TIME
#define TIME_EXECUTION(clkVar, expr) {                         \
  auto t1 = std::chrono::high_resolution_clock::now();         \
  (expr);                                                      \
  auto t2 = std::chrono::high_resolution_clock::now();         \
  clkVar = t2 - t1;     \
  }
#else
#define TIME_EXECUTION(clkVar, expr) (expr);
#endif


namespace Utils {

class time {
};

}

#endif
