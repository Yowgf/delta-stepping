//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class header
//
// Description: this class validates user argument and gets it ready
// to be run on the target algorithm
//===----------------------------------------------------------===//

// TODO: fix validateArguments.

#ifndef INIT_H
#define INIT_H

#include "DS/digraph.hpp"
#include "Utils/defs.hpp"

#include <fstream>

namespace Interface {

class init {
  using digraph = DS::digraph<nodeIdT>;
  
public:
  init(int argc, char** argv);
  ~init();

private:
  static constexpr int knumProgArgs = 3;
  static constexpr unsigned kmaxFileNameLen = 0x100;
  static constexpr unsigned kmaxModeLen = 0x100;

  static constexpr unsigned kmaxNodeId = 0xFFFFFFF;
  static constexpr int kmaxWeight = 0xFFFFFFF;

  std::string inFileName;
  bool inFileHasHeader;
  std::string inMode;

  // Built with inFileName
  std::string outFileName;
  std::ifstream inFile;

  // The graph is built in this class.
  digraph* inGraph;

  // Integrity assurance. These functions pertain to making sure
  //   the program is safe to run.
  void destroy();

  // Performs some basic validation on the program arguments.
  bool validateArguments(int argc, char** argv) const noexcept;

  // Gets the graph ready to be used. Performs all necessary checks
  // to ensure its validity.
  void processEntries(int argc, char** argv) noexcept(false);

  void openInFile(char const* fileArgName, const char* inModeArg);
  
  // Reads in the edges of the graph. Aux for ~processEntries~.
  void readEdges() noexcept(false);
  // Ignores comments that can come in the beggining of a file.
  void ignoreComments() noexcept(false);
  // This functions prints the graph. It is used just for debug
  // output.
  void printGraph() noexcept(false);
  
};

}

#endif
