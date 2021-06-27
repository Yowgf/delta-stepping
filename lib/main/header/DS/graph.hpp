//===----------------------------------------------------------===//
// DS module
//
// File purpose: graph class header file
//
// Description: graph is the main type to be used by the
// delta-stepping algorithm. It shall be optimized to make use of
// bucket fusion, good memory locality and reuse.
//
// This implementation is deeply tied to the algorithm itself. This
// is inevitable. Since we aim to achieve best results, we cannot be
// happy with a generic framework.
//===----------------------------------------------------------===//

#ifndef INTERFACE_H
#define INTERFACE_H

namespace DS {
  
class graph {
public:
  graph();
  
};

}

#endif
