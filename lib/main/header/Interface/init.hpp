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

#include "DS/graph.hpp"

namespace Interface {

class init {
public:
  init(int argc, char** argv);

private:
  using graph = DS::graph;
  
  bool validate_arguments(int argc, char** argv);
  graph* process_entries(int argc, char** argv);
  
};

}

#endif
