//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class header
//
// Description: this class validates user argument and gets it ready
// to be run on the target algorithm
//===----------------------------------------------------------===//

#ifndef INIT_H
#define INIT_H

#include "DS/digraph.hpp"
#include "Utils/defs.hpp"

#include <fstream>

namespace Interface {

class init {
  using digraph = DS::digraph<ALG_TYPE>;
  
public:
  init(int argc, char** argv);
  ~init();

private:
  static constexpr int knumProgArgs = 2;

  std::ifstream inFile;

  digraph* inGraph;
  int numNodes;

  // Integrity assurance. These functions pertain to making sure
  //   the program is safe to run.
  void destroy();

  // Performs some basic validation on the program arguments.
  bool validateArguments(int argc, char** argv) const noexcept;

  // Gets the graph ready to be used. Performs all necessary checks
  // to ensure its validity.
  void processEntries(int argc, char** argv) noexcept(false);
  // Reads in the edges of the graph. Aux for ~processEntries~.
  void readEdges(DS::array<unsigned>& numEdges) noexcept(false);
  // Ignores comments that can come in the beggining of a file.
  void ignoreComments();
  // This functions prints the graph. It is used just for debug
  // output.
  void printGraph();
};

}

#endif
