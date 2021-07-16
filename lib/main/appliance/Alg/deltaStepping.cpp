//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class implementation. See header
// file for more detail
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"

#include "Utils/str.hpp"

#include <cfloat>
#include <stdexcept>

#define INF FLT_MAX

using namespace std;
using namespace Utils;

namespace Alg {

using digraph = DS::digraph<ALG_TYPE>;


// Software engineering strategy:
//
// - Implement delta-stepping as described in the original
// paper. Leave this implementation in a function of itself.
//
// - Parallelize the algorithm (use a different function).
//
// - Try to fit in bucket fusion, and any other optimizations I
// might find useful. At this point, we must compare different
// optimizations, so we need to have a timing framework ready.
//
////////////////////////////////////////////////////////////////////
// Notes from delta-stepping: a parallel shoretest path algorithm
// 1998:
//
// Buckets implemented as doubly-linked list. -- Should I put these
// separated from the graph structure?
////////////////////////////////////////////////////////////////////
// Questions that have to be answered before implementing the
// algorithm:
//
// - Which data structure to use for the buckets?
// - Use eager or lazy bucket updates?
// - Is the current graph data structure optimal for the algorithm?
//
// Foster's methodology:
// - Which tasks can we identify in this algorithm?
// - How to aggregate the tasks?
// - How to map the aggregated tasks to threads?
deltaStepping::deltaStepping(digraph* inGraph, const char* mode, 
                const char* outFileName)
{
  // Intialize internal variables
  assignGraph(inGraph);
  openOutFile(outFileName);

  // TODO: is there anything that should be done under every mode?

  const string modeStr{mode};
  if (modeStr == "sequential") 
    sequential();
  else if (modeStr == "parallel")
    parallel();
  else if (modeStr == "parallel-bucket-fusion")
    parallelBucketFusion();
  else
    invalidMode(mode);
  // Allocate buckets

  // Initialize the distances to infinity, except for source. 
}

deltaStepping::~deltaStepping()
{
  // Close output file
  outFile.close();

  // Deallocate buckets
}


//===----------------------------------------------------------===//
// Aux functions for the constructor.
//===----------------------------------------------------------===//

void deltaStepping::assignGraph(digraph* inGraph) {
  if (inGraph == nullptr) {
    throw invalid_argument{
      string("(assignGraph) nullptr inGraph argument")};
  }
  diGraph = inGraph;
}

void deltaStepping::openOutFile(const char* outFileName)
{
  outFile.open(outFileName, ios_base::out | ios_base::trunc);
}

void deltaStepping::invalidMode(const char* mode)
{
  throw invalid_argument{
    string("Unknown option '") + mode +
      "' for the deltaStepping algorithm"};
}

//===----------------------------------------------------------===//
// Below is the implementation of the main algorithms
//===----------------------------------------------------------===//


void deltaStepping::sequential()
{}

void deltaStepping::parallel()
{}

void deltaStepping::parallelBucketFusion()
{}

}
