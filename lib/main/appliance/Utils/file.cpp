//===----------------------------------------------------------===//
// Utils module
//
// File purpose: implementation of class ~file~. See header file for
// more detail.
//===----------------------------------------------------------===//

#include "Utils/file.hpp"

#include <cstdio>

namespace Utils {

bool file::exists(const char* fileName)
{
  FILE* f = fopen(fileName, "r");
  bool result = false;
  if (f) {
    result = true;
  }
  fclose(f);
  return result;
}

}
