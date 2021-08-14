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
#include "Utils/error.hpp"

#include <stdexcept>
#include <vector>

namespace DS {
  
template <typename valueType>
class digraph {
  using node = digraphNode<valueType>;
  using dinVec = DS::array<node*>;
public:
  // Max number of nodes
  static constexpr int kmaxSize = 0xFFFF;

  explicit digraph(const unsigned knumNodes,
          DS::array<unsigned>& numEdges) : numNodes(knumNodes)
  {
    buildGraph(numEdges);
  }

  explicit digraph(const int knumNodes, DS::array<unsigned>& numEdges)
    : numNodes(knumNodes)
  {
    if (knumNodes < 0) {
      throw std::domain_error{"Cannot generate graph with negative"\
                                "number of nodes"};
    }
    
    buildGraph(numEdges);
  }

  explicit digraph(const unsigned knumNodes,
          std::vector<unsigned>& numEdges) : numNodes(knumNodes)
  {
    buildGraph(numEdges);
  }

  explicit digraph(const int knumNodes, std::vector<unsigned>& numEdges)
    : numNodes(knumNodes)
  {
    if (knumNodes < 0) {
      throw std::domain_error{"Cannot generate graph with negative"\
                                "number of nodes"};
    }
    
    buildGraph(numEdges);
  }

  ~digraph()
  {
    destroy();
  }

  void insertEdge(const int nodeId1, const int nodeId2, const int weight,
                  const int pos)
  {
    adjList->at(nodeId1)->insertOut(nodeId2, weight, pos);
  }   

  // Utility functions
  unsigned size()
  {
    return numNodes;
  }

  unsigned getNumNodes()
  {
    return numNodes;
  }

  // Reference to the node with id ~nodeId~.
  node* at(unsigned nodeId)
  {
    return adjList->at(nodeId);
  }
private:
  // Adjacency list. Since the graph is of fixed size, we use a
  // vector.
  unsigned numNodes;
  dinVec* adjList;

  // This should only be called if there was some failure when
  // allocating basic structures for the object.
  void destroy()
  {
    register unsigned i = 0;
    for (i = 0; i < adjList->size(); ++i) {
      delete adjList->at(i);
    }
    delete adjList;
  }

  // This function just calls the two parts needed to fill the
  // graph:
  // - allocating the nodes
  // - allocating the edges
  //
  // After this function is done, we still have to define its edges.
  void buildGraph(DS::array<unsigned>& numEdges)
  {
    try {
      // It is of high importance to first allocate all the nodes
      // for ~adjList~, and only then fill them with their edges.
      //
      // This arranges the memory in a better way, avoiding cache
      // misses, since adjList is going to be used all the time, and
      // nodes with close Id are more likely to be used within a
      // small window of time.
      fillNewNodes();
      allocateEdges(numEdges);
    }
    catch (std::exception& e) {
      destroy();
      throw;
    }
  }

  void buildGraph(std::vector<unsigned>& numEdges)
  {
    try {
      fillNewNodes();
      allocateEdges(numEdges);
    }
    catch (std::exception& e) {
      destroy();
      throw;
    }
  }

  void fillNewNodes() noexcept(false)
  {
    adjList = new dinVec{numNodes, nullptr};
    register unsigned i = 0;
    for (i = 0; i < adjList->size(); ++i) {
      adjList->at(i) = new node(i); // node with ID ~i~
    }
  }

  // This function receives the number of edges for each node
  //
  // Note that we do it all at once. This avoids memory
  // fragmentation, and thus poor performance.
  void allocateEdges(DS::array<unsigned>& numEdges) noexcept(false)
  {
    register unsigned i = 0;
    for (i = 0; i < numEdges.size(); ++i) {
      if (numEdges.at(i)) {
        adjList->at(i)->allocEdges(numEdges.at(i));
      }
    }
  }

  void allocateEdges(std::vector<unsigned>& numEdges) noexcept(false)
  {
    LOG(1, "Start -- allocateEdges");
    register unsigned i = 0;
    for (i = 0; i < numEdges.size(); ++i) {
      if (numEdges.at(i) != 0) {
        LOGATT(1, i);
        adjList->at(i)->allocEdges(numEdges.at(i));
      }
    }
    LOG(1, "End -- allocateEdges");
  }

};

}

#endif

