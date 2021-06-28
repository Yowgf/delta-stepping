//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"
#include "Interface/init.hpp"
#include "Utils/file.hpp"

#include <fstream>
#include <stdexcept>

using digraph = DS::digraph;

using namespace Utils;

namespace Interface {
  
init::init(int argc, char** argv)
{
  const bool isResultValid = validateArguments(argc, argv);
  if (!isResultValid) {
    throw std::invalid_argument {"Invalid program arguments"\
                                   "\nUsage: <program> <in-file>"};
  }

  digraph* inGraph = processEntries(argc, argv);

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

// While processing the entries, this function should check if the
// file format is correct, of course.
//
// The file should come in the format:
// <number-of-nodes>
// [<node1>,<node2>]          // for every edge (node1, node2)
digraph* init::processEntries(int argc, char** argv) const
  noexcept(false)
{
  // We trust that argv[1] exists
  std::ifstream inFile;
  inFile.open(argv[1]);
  
  int numNodes = -1;
  inFile >> numNodes;
  if (numNodes < 0) {
    throw std::logic_error{"Wrong input file format (numNodes)"};
  }

  digraph* inGraph = nullptr;
  
  return inGraph;
}

}
