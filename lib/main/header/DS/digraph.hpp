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

#ifndef DIGRAPH_H
#define DIGRAPH_H

#include "DS/digraphNode.hpp"

#include <stdexcept>

namespace DS {
  
template <typename valueType>
class digraph {
  using node = digraphNode<valueType>;
  using dinVec = DS::array<node*>;
public:
  // Max number of nodes
  static constexpr int kmaxSize = 0xFFFF;

  digraph(const unsigned knumNodes) : numNodes(knumNodes)
  {
    fillNewNodes();
  }

  digraph(const int knumNodes) : numNodes(knumNodes)
  {
    if (knumNodes < 0) {
      throw std::domain_error{"Cannot generate graph with negative"\
                                "number of nodes"};
    }

    fillNewNodes();
  }
  // Delete every node.
  ~digraph()
  {
    unsigned i = 0;
    for (i = 0; i < adjList->size(); ++i) {
      delete adjList->at(i);
    }

    delete adjList;
  }

private:
  // Adjacency list. Since the graph is of fixed size, we use a
  // vector.
  unsigned numNodes;
  dinVec* adjList;

  void fillNewNodes()
  {
    adjList = new dinVec{numNodes, nullptr};
    unsigned i = 0;
    for (i = 0; i < adjList->size(); ++i) {
      adjList->at(i) = new node(i); // node with ID ~i~
    }
  }  
};

}

#endif
