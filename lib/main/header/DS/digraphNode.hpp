//===----------------------------------------------------------===//
// DS module
//
// File purpose: ~digraphNode~ class declaration, and definition
// (since it is a template).
//===----------------------------------------------------------===//

#ifndef NODE_H
#define NODE_H

#include "DS/array.hpp"

#include <utility>

namespace DS {

constexpr int kmaxNumEdges = 0xFF;

template<typename valueType>
class digraphNode {
private:
  using idT = unsigned; // id Type
  using weightT = int;
  using outEdges = DS::array<std::pair<idT, weightT>>; // See DS/array.hpp

public:
  digraphNode(const idT ID,
              const valueType& value,
              const unsigned numOut  // Number of outgoing edges
              )
    : ID(ID), value(value), out(nullptr)
  {
    if (numOut) {
      out = new outEdges(numOut, std::make_pair(0, 0));
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

  // In the functions below, we expect that the user knows where to put the node
  // (by providing the pos parameter).
  //
  // This just goes to avoid having to store this information in every node.
  void insertOut(const idT outNodeID, const int weight, const unsigned pos)
    noexcept(false)
  {
    if (out != nullptr) {
      out->at(pos).first = outNodeID;
      out->at(pos).second = weight;
    }
    else {
      throw std::logic_error{"Impossible to use nullptr outEdges"};
    }
  }
  
  // Utility functions
  unsigned size()
  {
    return out->size();
  }

  idT getId()
  {
    return ID;
  }

  // Returns the ID of the destination node of the output edges at
  // ~pos~
  idT getEdgeDest(const unsigned pos)
  {
    return out->at(pos).first;
  }

  weightT getEdgeWeight(const unsigned pos)
  {
    return out->at(pos).second;
  }

  bool isLeaf()
  {
    return out == nullptr;
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
