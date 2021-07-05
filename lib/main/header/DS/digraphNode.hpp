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

template<typename valueType>
class digraphNode {
private:
  using outEdges = DS::array<unsigned>; // See DS/array.hpp

public:
  digraphNode(const unsigned ID,
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
  digraphNode(const unsigned ID, const valueType& value)
    : digraphNode(ID, value, 0) {}

  digraphNode(const unsigned ID) : digraphNode(ID, 0) {}

  // In the functions below, we expect that the user knows where to
  // put the node (by providing the pos parameter).
  void insertOut(const unsigned outNodeID, const unsigned pos)
  {
    if (out != nullptr) {
      out->at(pos) = outNodeID;
    }
    else {
      throw std::logic_error{"Impossible to use nullptr inEdges"};
    }
  }

private:
  const unsigned ID;
  valueType value;
  outEdges* out;
 
};

}

#endif
