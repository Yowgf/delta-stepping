//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class declaration.
//===----------------------------------------------------------===//

// TODO: where does the cache line size fit in this algorithm? There
// has to be such an optimization.

#ifndef DELTA_STEPPING_H
#define DELTA_STEPPING_H

#include "DS/digraph.hpp"
#include "Utils/defs.hpp"

#include <fstream>

namespace Alg {

class deltaStepping {
  using digraph = DS::digraph<ALG_TYPE>;

public:

  // This function performs the delta stepping algorithm according
  // to the given mode. It writes the results on the file whose name
  // is provided in the argument ~outFileName~.
  deltaStepping(digraph* inGraph, const char* mode, 
                const char* outFileName);
  ~deltaStepping();

private:
  digraph* diGraph;
  std::ofstream outFile;
  
  void assignGraph(digraph* inGraph);
  void openOutFile(const char* outFileName);
  void invalidMode(const char* mode);

  // These are all the versions of the algorithm, that can be
  // differentiated according to the ~mode~ argument given to the
  // constructor.
  void sequential();
  void parallel();
  void parallelBucketFusion();
};

}

#endif
