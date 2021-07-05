//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"
#include "Interface/init.hpp"
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

  Alg::deltaStepping{inGraph};

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

void init::readEdges()
{
  // Edges in format <source node>,<destination node>
  // Keep it as simple as possible, so that we avoid bugs.
  //
  // Also, do some basic checks that assure the validity of the
  // generated graph.
  int nodeId1 = 0;              // Node on the left
  int nodeId2 = 0;              // Node on the right
  unsigned int lineCounter = 1; // Used to debug
  while(inFile.good()) {
    
    inFile >> nodeId1;

    file::checkIfstreamNextChar(inFile, ',', lineCounter);
    file::checkIfstreamGood(inFile, lineCounter);
    
    inFile >> nodeId2;

    // Check if both inputs are in the right range
    num<int>::checkInRange(nodeId1, 0, numNodes - 1);
    num<int>::checkInRange(nodeId2, 0, numNodes - 1);

    

    lineCounter++;
  }
}

// While processing the entries, this function should check if the
// file format is correct, of course.
//
// The file should come in the format:
// <number-of-nodes>
// <number-of-edges>
// [<node0>,<node1>]
// [<node0>,<node2>]
// ...
// <number-of-edges>
// [<node1>,<node0>]
// [<node1>,<node2>]
// ...
void init::processEntries(int argc, char** argv) noexcept(false)
{
  // We trust that argv[1] exists
  inFile.open(argv[1]);
  
  numNodes = -1;
  inFile >> numNodes;
  if (digraph::kmaxSize < numNodes || numNodes < 0) {
    throw std::logic_error{"Wrong input file format (numNodes)"};
  }

  // Build the whole digraph here
  inGraph = new digraph(numNodes);

  readEdges();
}

}
