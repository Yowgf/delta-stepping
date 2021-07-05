//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of class ~file~. This file contains
// useful functions to deal with files throughout the program.
//===----------------------------------------------------------===//

#ifndef FILE_H
#define FILE_H

#include <fstream>

namespace Utils {

class file {
public:
  static bool exists(const char* fileName);

  static void checkIfstreamGood
  (std::ifstream& ifs, const unsigned lineNum) noexcept(false);

  // Checks if next character in ~ifs~ is ~c~. Advances one
  // character in the stream. Throws an exception in case the test
  // fails.
  static void checkIfstreamNextChar
  (std::ifstream& ifs, const char exp, const unsigned lineNum)
    noexcept(false);
private:
};

}

#endif
