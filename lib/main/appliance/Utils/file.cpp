//===----------------------------------------------------------===//
// Utils module
//
// File purpose: implementation of class ~file~. See header file for
// more detail.
//===----------------------------------------------------------===//

#include "Utils/file.hpp"

#include <cstdio>
#include <stdexcept>

using namespace std;

namespace Utils {

bool file::exists(const char* fileName)
{
  FILE* f = fopen(fileName, "r");
  bool result = false;
  if (f) {
    result = true;
    fclose(f);
  }
  return result;
}


void file::checkIfstreamGood
(std::ifstream& ifs, const unsigned lineNum) noexcept(false)
{
  if (!ifs.good()) {
    throw std::logic_error{
      string("Line ") + to_string(lineNum) + ": " +
        "stream not in good state"};
  }
}

void file::checkIfstreamNextChar
(std::ifstream& ifs, const char exp, const unsigned lineNum)
  noexcept(false)
{
  char got = '\0';
  got = ifs.get();
  if (got != exp) {
    throw std::logic_error{
      string("Line ") + to_string(lineNum) + ": " +\
        "Expected '" + exp + "', got "  + got};
  }
}

}
