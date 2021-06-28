//===----------------------------------------------------------===//
// Utils module
//
// File purpose: implementation of the error class. See header file
// for more detail.
//===----------------------------------------------------------===//

// Currently inactive
#if 0

#include "Utils/error.hpp"

#include <cstring>

namespace Utils {

char error::errbuf[errbufLen];
  
error::error(int errnum)
{
  error::errbuf[0] = '\0';

  switch (errnum) {
    
  case EDOM:
  case ERANGE:
  case EILSEQ:
    strerror_r(errnum, errbuf, errbufLen);
  default:
    report_error(errnum);
  }
  
}

void error::report_error(int errnum)
{
  LOG(1, "Error number %d. %s", errnum, errbuf);
}

  
}

#endif
