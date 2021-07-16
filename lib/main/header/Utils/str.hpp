//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of the str class.
//
// Details: this class contains some useful functions for dealing
// with strings.
//===----------------------------------------------------------===//

#ifndef STR_H
#define STR_H

#include <cstdint>
#include <string>

namespace Utils {

class str {
public:
  static uint64_t constexpr hash(const char*);

  static std::string basename(std::string& inName);

  // Gets the basename of the name, and adds the suffix ~outSuffix~.
  static std::string getOutName(std::string& inName);

private:
  static uint64_t constexpr mix(const char, uint64_t s);

  static constexpr char outSuffix[] = ".out";
};

}

#endif
