//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class declaration.
//===----------------------------------------------------------===//

#ifndef DELTA_STEPPING_H
#define DELTA_STEPPING_H

#include "DS/digraph.hpp"
#include "Utils/defs.hpp"

namespace Alg {

class deltaStepping {
  using digraph = DS::digraph<ALG_TYPE>;

public:
  deltaStepping(digraph*);

private:
};

}

#endif
