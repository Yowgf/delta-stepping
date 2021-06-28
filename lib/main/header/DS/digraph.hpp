//===----------------------------------------------------------===//
// DS module
//
// File purpose: digraph class header file
//
// Description: digraph is the main type to be used by the
// delta-stepping algorithm. It shall be optimized to make use of
// bucket fusion, good memory locality and reuse.
//
// This implementation is deeply tied to the algorithm itself. This
// is inevitable. Since we aim to achieve best results, we cannot be
// happy with a generic framework.
//===----------------------------------------------------------===//

#ifndef INTERFACE_H
#define INTERFACE_H

#include "DS/digraphNode.hpp"

namespace DS {
  
class digraph {
public:
  digraph();
  
};

}

#endif
