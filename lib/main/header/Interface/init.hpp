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

namespace Interface {

class init {
  using digraph = DS::digraph;
  
public:
  init(int argc, char** argv);

private:
  static constexpr int knumProgArgs = 2;

  // Performs some basic validation on the program arguments.
  bool validateArguments(int argc, char** argv) const noexcept;
  digraph* processEntries(int argc, char** argv) const noexcept(false);
  
};

}

#endif
