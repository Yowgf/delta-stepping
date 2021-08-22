//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "DS/wEdge.hpp"
#include "Interface/init.hpp"
#include "Utils/error.hpp"
#include "Utils/file.hpp"
#include "Utils/num.hpp"
#include "Utils/str.hpp"

#include <string>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "boost/dynamic_bitset.hpp"

#define INRANGE(n, a, b) (a <= n && n <= b)

using digraph = DS::digraph<nodeIdT>;

using namespace Utils;
using namespace std;

namespace Interface {
  
init::init(int argc, char** argv)
  : inFileHasHeader(false), inGraph(nullptr), numThreads(1)
{
  const bool isProgArgsValid = validateArguments(argc, argv);
  if (!isProgArgsValid) {
    throw std::invalid_argument {"Invalid program arguments"\
                                   "\nUsage: <program> <in-file>"\
                                   " <mode> [<num-threads>]"};
  }

  // This loads inGraph
  processEntries(argc, argv);

# if INTERFACE_INIT_PRINT_GRAPH == 1
  printInGraph();
# endif
  
  try {
    Alg::deltaStepping dsRun;
    TIME_EXECUTION(clkVar,
		   dsRun.run(inGraph, inMode.c_str(),
			     static_cast<unsigned>(numThreads))
		   );

    writeOut(dsRun);
    printOut();
  }
  catch(std::exception&) {
    destroy();
    throw;
  }
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
// Check if the mode string given has proper size.
bool init::validateArguments(int argc, char** argv) const noexcept
{
  if (!INRANGE(argc, kminProgArgs, kmaxProgArgs)) {
    return false;
  }

  const char* fileName = argv[1];
  if (!file::exists(fileName)) {
    return false;
  }

  if (char_traits<char>::length(argv[1]) > kmaxFileNameLen) {
    return false;
  }

  if (char_traits<char>::length(argv[2]) > kmaxModeLen) {
    return false;
  }

  if (argc == knumProgArgsWithThreads) {
    int numThreadsIn = 0;
    std::stringstream ss(argv[knumProgArgsWithThreads - 1]);
    ss >> numThreadsIn;
    if (!INRANGE(numThreadsIn, kminNumThreads, kmaxNumThreads)) {
      return false;
    }
  }
  
  return true;
}

// [COMMENT -> COMMENT COMMENT] where COMMENT ::= %*.*\n
// [<node1> <node2> <weight>]
// [<node1> <node2> <weight>]
// ...
void init::processEntries(int argc, char** argv) noexcept(false)
{
  if (argc == knumProgArgsWithThreads) {
    std::stringstream ss(argv[knumProgArgsWithThreads - 1]);
    ss >> numThreads;
  }
  // Read file name, mode, and then open the file and start reading
  // it.
  openInFile(argv[1], argv[2]);

  ignoreComments();

  // At this point, the graph has been created, so we must be sure
  // to delete it if an exception occurs.
  try {
    readEdges();
  }
  catch(std::exception& e) {
    destroy();
    throw;
  }
}

void init::openInFile(char const* fileArgName, const char* inModeArg)
{
  inFileName.assign(fileArgName);
  outFileName = str::getOutName(inFileName);
  inMode.assign(inModeArg);
  inFile.open(inFileName);
}
  
void init::readEdges() noexcept(false)
{
  DEBUG(INTERFACE_INIT_DEBUG, "Start -- readEdges");
  // Edges in format <source node> <destination node> <weight>
  //
  // We use a vector because we assume that the input file will only include
  // edges for nodes in the graph, so that we are not wasting memory.
  //
  // Note that graphNodes and graphEdges should have the same size.
  const unsigned bitsetAllocSz = 0xFFFF;
  boost::dynamic_bitset<> graphNodes(bitsetAllocSz);
  vector<nodeIdT> numEdges(bitsetAllocSz, 0);
  vector<DS::wEdge<int, int, int> > graphEdges;

  // TODO: use these values! (currently ignoring)
  // Read header
  int m = -1;
  int n = -1;
  int numNonZeroNodes = -1;

  if (inFileHasHeader) {
    inFile >> m >> n >> numNonZeroNodes;
    if (m == -1) throw logic_error{"(readEdges) header -- unexpected EOF"};
  }


  unsigned numNodes = 0;
  unsigned lineCounter = 1;
  int nodeMaxId = -1;
  while (inFile.good()) {
    if (!lineCounter % 100)
      LOGATT(INTERFACE_INIT_DEBUG, lineCounter);

    // <node1, node2, weight>
    DS::wEdge<int, int, int> edge(-1, -1, -1);
  
    inFile >> edge.node1 >> edge.node2 >> edge.weight;
    if (edge.node1 == -1) 
      break; // Hopefully true means EOF

    // Check if inputs are in the right range
    num<int>::checkInRange(edge.node1, 1, kmaxNodeId - 1);
    num<int>::checkInRange(edge.node2, 1, kmaxNodeId - 1);
    num<int>::checkInRange(edge.weight, 0, kmaxWeight - 1);

    // If needed, resize
    nodeMaxId = num<int>::max(edge.node1, edge.node2);
#   pragma GCC diagnostic ignored "-Wsign-compare"
    if (nodeMaxId > graphNodes.size()) {
      graphNodes.resize(num<int>::max(nodeMaxId,
                                      graphNodes.size() + bitsetAllocSz));
      numEdges.resize(num<int>::max(nodeMaxId,
                                    numEdges.size() + bitsetAllocSz), 0);
    }

    // Update graphNodes, graphEdges, numEdges
    graphNodes[edge.node1] = true;
    graphNodes[edge.node2] = true;

    // Fix node ids. Files should come with range starting from 1, but we want
    // it starting from 0. This makes everything easier, later on.
    edge.node1--;
    edge.node2--;
    graphEdges.push_back(edge);

    LOG(INTERFACE_INIT_DEBUG,
        "About to insert edge %d %d", edge.node1, edge.node2);
    numEdges.at(edge.node1)++;

    lineCounter++;
  }

  if (!inFile.eof()) {
    throw logic_error{"(readEdges) encountered error while read input file"};
  }

  // Allocate all the digraph nodes here. This avoids memory fragmentation.
  inGraph = new digraph(static_cast<unsigned>(graphNodes.count()), numEdges);
  
  for (auto edge : graphEdges) {
    inGraph->insertEdge(edge.node1, edge.node2, edge.weight,
                        --numEdges.at(edge.node1));
  }

  DEBUG(INTERFACE_INIT_DEBUG, "End -- readEdges");
}

// Additionally marks if the file has a %%Header
void init::ignoreComments() noexcept(false)
{
  DEBUG(INTERFACE_INIT_DEBUG, "Start -- IgnoreComments");

  const unsigned maxLineSz = 0xFFFF;
  while(inFile.good() && inFile.peek() == '%') {
    inFile.get();
    if (inFile.peek() == '%') inFileHasHeader = true;
    // Ignore line
    while (inFile.good() && inFile.get() != '\n');
  }

  LOGATT(INTERFACE_INIT_DEBUG, inFileHasHeader);

  if (inFile.eof()) {
    throw logic_error{"(ignoreComments) file has no contents."};
  }
  else if (!inFile.good()) {
    throw logic_error{"(ignoreComments) error reading file."};
  }

  DEBUG(INTERFACE_INIT_DEBUG, "End -- IgnoreComments");
}

// For every node in the graph, print its number of outgoing
// edges, and each one of the destination nodes for these edges.
void init::printInGraph() noexcept(false)
{
  register unsigned i = 0;
  register unsigned j = 0;

  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cout << "Printing input graph\n";
  std::cout << '\n';
  for (i = 0; i < inGraph->size(); ++i) {
    if (!inGraph->at(i)->isLeaf()) {
      std::cout << '\n';
      std::cout << "Node " << inGraph->at(i)->getId() << ":\n";

      for (j = 0; j < inGraph->at(i)->size(); ++j) {
        std::cout << i << ',' << inGraph->at(i)->getEdgeDest(j) << ',' << 
                  inGraph->at(i)->getEdgeWeight(j) << '\n';
      }
    }
  }
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";  
}

void init::writeOut(Alg::deltaStepping& dsRun)
{
  dsRun.printOutToFile(outFileName.c_str());
}

void init::printOut() const noexcept
{
#if INTERRFACE_INIT_PRINT_DISTS
  printOutDists();
#endif
#if INTERFACE_INIT_PRINT_TIME
  printOutTime();
#endif
}

void init::printOutDists() const
{
  std::ifstream ifs(outFileName, std::ios_base::in);

  const unsigned bufSz = 0xFFFF;
  while (ifs.good()) {
    char buffer[bufSz];
    ifs.readsome(buffer, bufSz - 1);
    buffer[ifs.gcount()] = '\0';
    std::cout << buffer;
  }

  if (!ifs.eof()) {
    throw std::logic_error{"(printOutDists) Error while printing distances."};
  }
}

void init::printOutTime() const noexcept
{
  double globalClkCount = clkVar.count();
  cout << setprecision(6) << std::fixed << globalClkCount << '\n';
}

}
