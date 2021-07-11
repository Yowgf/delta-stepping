//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"
#include "Interface/init.hpp"
#include "Utils/error.hpp"
#include "Utils/file.hpp"
#include "Utils/num.hpp"

#include <stdexcept>

using digraph = DS::digraph<ALG_TYPE>;

using namespace Utils;
using namespace std;

namespace Interface {
  
init::init(int argc, char** argv) : inGraph(nullptr), numNodes(0)
{
  const bool isResultValid = validateArguments(argc, argv);
  if (!isResultValid) {
    throw std::invalid_argument {"Invalid program arguments"\
                                   "\nUsage: <program> <in-file>"};
  }

  // This loads inGraph
  processEntries(argc, argv);

# ifdef INTERFACE_INIT_PRINT_GRAPH
  printGraph();
# endif

  Alg::deltaStepping{inGraph};
}

init::~init()
{
  destroy();
}

void init::destroy()
{
  delete inGraph;
}

// Checks if the number of arguments is correct.
// Check if the file given at argv[0] exists.
bool init::validateArguments(int argc, char** argv) const noexcept
{
  if (argc != knumProgArgs) {
    return false;
  }

  const char* fileName = argv[1];
  if (!file::exists(fileName)) {
    return false;
  }

  return true;
}

// While processing the entries, this function should check if the
// file format is correct, of course.
//
// The file should come in the format:
// [COMMENT COMMENT] where COMMENT ::= //*\n
// <number-of-nodes>
// [<number-of-edges-node-1>]
// [<number-of-edges-node-2>]
// ...
// [<node0>,<node1>]
// [<node0>,<node2>]
// ...
// [<node1>,<node0>]
// [<node1>,<node2>]
// ...
//
// FIXME: must transfer reception of number of edges to this
// function.
void init::processEntries(int argc, char** argv) noexcept(false)
{
  // We trust that argv[1] exists
  inFile.open(argv[1]);
  
  ignoreComments();

  numNodes = -1;
  inFile >> numNodes;
  if (digraph::kmaxSize < numNodes || numNodes < 0) {
    throw std::logic_error{"Wrong input file format (numNodes)"};
  }

  // Array that tells how many edges each node has
  DS::array<unsigned> numEdges{numNodes, 0};

  // FIXME
  // Build numEdges object
  unsigned i = 0;
  unsigned temp = -1;
  for (i = 0; i < numEdges.size(); ++i) {
    temp = -1;
    inFile >> temp;
    if (temp < 0) {
      throw std::logic_error{"Wrong input file format (numEdges)"};
    }

    numEdges.at(i) = temp;
  }

  // Allocate all the digraph nodes here. This avoids memory
  // fragmentation.
  inGraph = new digraph(numNodes, numEdges);

  // At this point, the graph has been created, so we must be sure
  // to delete it if an exception occurs.
  try {
    readEdges(numEdges);
  }
  catch(std::exception& e) {
    destroy();
    throw;
  }
}

void init::readEdges(DS::array<unsigned>& numEdges) noexcept(false)
{
  DEBUG(INTERFACE_INIT_DEBUG, "Start -- readEdges");
  // Edges in format <source node>,<destination node>
  // Keep it as simple as possible, so that we avoid bugs.
  //
  // Also, do some basic checks that assure the validity of the
  // generated graph.
  int nodeId1 = -1;         // Node on the left
  int nodeId2 = -1;         // Node on the right
  int curNode = 0;          // We are inserting edges for this node
  unsigned edgeCounter = 0; // How many edges have we inserted for
                            // the current node
  unsigned lineCounter = 1; // Used to debug. Start from second line
  while(inFile.good()) {
    nodeId1 = nodeId2 = -1; // Make sure we receive update on these.

    inFile >> nodeId1;
    if (!inFile.good()) break; // Hopefully EOF

    if (curNode != nodeId1) {
      // Check if we are not moving backwards over the node ids
      if (nodeId1 < curNode) {
        throw std::logic_error{
          string("File has wrong order of nodes! (") +
            to_string(curNode) + " to " + to_string(nodeId1) + ')'};
      }

      // Check if we have all the edges that they promised
      if (edgeCounter < numEdges.at(curNode)) {
        throw std::logic_error{
          string("File provided too few edges for node ") +
            to_string(curNode) + " (Expecting " + 
            to_string(numEdges.at(curNode))};
      }

      // At this point we can update the current node and reset the
      // edges counter.
      curNode = nodeId1;
      edgeCounter = 0;
    }

    // Next, we need a comma
    file::checkIfstreamNextChar(inFile, ',', lineCounter);
    
    // Then read in the second node id
    inFile >> nodeId2;

    // Check that we actually read the node Id
    file::checkIfstreamGood(inFile, lineCounter);

    // Check if both inputs are in the right range
    num<int>::checkInRange(nodeId1, 0, numNodes - 1);
    num<int>::checkInRange(nodeId2, 0, numNodes - 1);

    // Insert edge in graph
    if (edgeCounter < numEdges.at(nodeId1)) {
      inGraph->insertEdge(nodeId1, nodeId2, edgeCounter);
      edgeCounter++;
    }
    else {
      throw std::logic_error{
        string("File provided too many edges for node ") +
          to_string(curNode)};
    }
    
    lineCounter++;
  }
  DEBUG(INTERFACE_INIT_DEBUG, "End -- readEdges");
}

void init::ignoreComments()
{
  DEBUG(INTERFACE_INIT_DEBUG, "Start -- IgnoreComments");

  // Get two '/' from file. Then ignore the rest of the line.
  // Check for syntax errors.
  char c1, c2;
  c1 = c2 = '\0';
  unsigned lineCounter = 0;
  while (inFile.good() && (c1 = inFile.get()) == '/') {
    if ((c2 = inFile.get()) == '/') {
      while (inFile.good() && (c2 = inFile.get()) != '\n');
    }
    else {
      inFile.putback(c2);
      inFile.putback(c1);
        
      throw std::logic_error{
        "(IgnoreComments) Syntax error in input file"};
    }

    lineCounter++;
  }
  // That means that we left the loop by the second condition.
  if (inFile.good()) {
    inFile.putback(c1);
  }

  DEBUG(INTERFACE_INIT_DEBUG, "End -- IgnoreComments");
}

// For every node in the graph, print its number of outgoing
// edges, and each one of the destination nodes for these edges.
void init::printGraph()
{
  register unsigned i = 0;
  register unsigned j = 0;

  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cout << "Printing input graph\n";
  std::cout << '\n';
  for (i = 0; i < inGraph->size(); ++i) {
    std::cout << '\n';
    std::cout << "Node " << inGraph->at(i)->getId() << ":\n";

    for (j = 0; j < inGraph->at(i)->size(); ++j) {
      std::cout << i << ',' << inGraph->at(i)->getEdgeDest(j)
                << '\n';
    }
  }
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  // Print the number of outgoing edges
  
}


}
