//===----------------------------------------------------------===//
// DS module
//
// File purpose: ~digraphNode~ class declaration.
//===----------------------------------------------------------===//

#ifndef NODE_H
#define NODE_H

#include <vector>

namespace DS {

template<typename valueType>
class digraphNode {

  // We build different types for in and out edges, to avoid
  // confusion
  using inEdges = std::vector<digraphNode<valueType>*>;
  using outEdges = std::vector<digraphNode<valueType>*>;

public:
  digraphNode(unsigned ID, valueType value,
       inEdges* prev, outEdges* next) :
    ID(ID), value(value)
  {
    // Assure that we don't receive nullptrs
    if (prev) this->prev = *prev;
    if (next) this->next = *next;
  }

  digraphNode(unsigned ID) : digraphNode(ID, 0, nullptr) {}

private:
  const unsigned ID;
  valueType value;

  // We use vectors to allocate previous and next nodes, since we
  // will have a fixed number of those, and we do it on the stack,
  // to avoid memory fragmentation.
  inEdges prev;
  outEdges next;
 
};

}

#endif
