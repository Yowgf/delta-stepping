//===----------------------------------------------------------===//
// Utils module
//
// File purpose: implementation of the str class.
//===----------------------------------------------------------===//

#include "Utils/str.hpp"

namespace Utils {

std::string str::basename(std::string& inName)
{
  return inName.substr(0, inName.find_last_of('.'));
}

std::string str::getOutName(std::string& inName)
{
  return basename(inName) + ".out";
}

}
