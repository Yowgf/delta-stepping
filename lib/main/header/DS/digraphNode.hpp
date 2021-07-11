//===----------------------------------------------------------===//
// DS module
//
// File purpose: ~digraphNode~ class declaration, and definition
// (since it is a template).
//===----------------------------------------------------------===//

#ifndef NODE_H
#define NODE_H

#include "DS/array.hpp"

namespace DS {

constexpr int kmaxNumEdges = 0xFF;

template<typename valueType>
class digraphNode {
private:
  using idT = unsigned; // id Type
  using outEdges = DS::array<idT>; // See DS/array.hpp

public:
  digraphNode(const idT ID,
              const valueType& value,
              const unsigned numOut  // Number of outgoing edges
              )
    : ID(ID), value(value), out(nullptr)
  {
    if (numOut) {
      out = new outEdges(numOut, 0);
    }
  }

  // These constructors create nodes with 0 outgoing edges.
  digraphNode(const idT ID, const valueType& value)
    : digraphNode(ID, value, 0) {}

  digraphNode(const idT ID) : digraphNode(ID, 0) {}

  ~digraphNode()
  {
    destroy();
  }

  void allocEdges(const unsigned numEdges) noexcept(false)
  {
    out = new outEdges(numEdges); // may throw bad_alloc
  }

  // In the functions below, we expect that the user knows where to
  // put the node (by providing the pos parameter).
  //
  // This just goes to avoid having to store this information in
  // every node.
  void insertOut(const idT outNodeID, const unsigned pos)
    noexcept(false)
  {
    if (out != nullptr) {
      out->at(pos) = outNodeID;
    }
    else {
      throw std::logic_error{"Impossible to use nullptr outEdges"};
    }
  }
  
  // Utility functions
  const unsigned size()
  {
    return out->size();
  }

  const idT getId()
  {
    return ID;
  }

  // Returns the ID of the destination node of the output edges at
  // ~pos~
  const idT getEdgeDest(const unsigned pos)
  {
    return out->at(pos);
  }

  // End -- Utility functions

private:
  const idT ID;
  valueType value;
  outEdges* out;

  void destroy()
  {
    delete out;
  }
 
};

}

#endif
