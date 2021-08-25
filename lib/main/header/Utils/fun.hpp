//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of class ~fun~. This class provides the
//  user with some commonly seen functional jargon that can be used
//  in c++.
//===----------------------------------------------------------===//

#ifndef FUN_H
#define FUN_H

#include <functional>

namespace Utils {
  
class fun {
public:
  template <class sig>
  std::function<sig> functionCast(void* f)
  {
    return static_cast<sig*>(f);
  }
};

}



#endif
