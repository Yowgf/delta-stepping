//===----------------------------------------------------------===//
// Utils module
//
// File purpose: definition of error class.
//
// Description: This class defines some useful procedures that can
// be called when encontering errors throughout the program.
//
// 
//===----------------------------------------------------------===//

#ifndef ERROR_H
#define ERROR_H

#include "Utils/defs.hpp"

#include <cerrno>
#include <cstdio>
#include <iostream>

namespace Utils {

// None of these macros are thread-safe

// Log attribute att.
#define LOGATT(att, isSupposedToPrint)\
  fprintf((void) (cerr << #att << ": " << (att) << '\n');

// For the first two logging functions, it is not always certain
// that we want to print the information given in every place we put
// these. Therefore, we provide one additional argument, that can
// control if it will print the message or not based on a macro, for
// example.

// Normal logging function. Accepts printf-like formatting.
#define LOG(isSupposedToPrint, msg, ...)\
  if (isSupposedToPrint) {\
    fprintf(stderr, msg "\n", ##__VA_ARGS__);\
  }
  
// The same, but prints file name and line number.
#define DEBUG(isSupposedToPrint, msg,...)\
  if (isSupposedToPrint) {\
    fprintf(stderr, "[INFO] (%s:%d) " msg "\n",\
            __FILE__, __LINE__, ##__VA_ARGS__);\
  }

// We always will want to print error messages.
#define LOG_ERROR(errnum, msg, ...) DEBUG(1, msg, ##__VA_ARGS__);

// This class is currently inactive
#if 0
class error {
public:
  error(int errnum);
  
private:
  static const int errbufLen = 0xFF;
  static char errbuf[errbufLen];
  
  void report_error(int errnum);  
};
#endif 
  
}

#endif
