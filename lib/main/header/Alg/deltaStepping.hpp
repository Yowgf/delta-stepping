//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class declaration.
//===----------------------------------------------------------===//

// TODO: make the sequential version correct
// TODO: optimize the entire algorithm
// - usage of pairs seems to not be optimized
// TODO: maybe remove dynamic_bitset-dependent implementations

#ifndef DELTA_STEPPING_H
#define DELTA_STEPPING_H

#include "DS/digraph.hpp"
#include "Utils/defs.hpp"

#include "boost/dynamic_bitset.hpp"

#include <fstream>
#include <limits>
#include <list>
#include <utility>
#include <vector>

namespace Alg {

typedef weightT distT; // Distance type
typedef std::vector<distT> distsT;
typedef std::list<nodeIdT> buckT;
// TODO: maybe change this from vector to something else.
typedef std::vector<buckT> bucksT;
typedef std::list<std::pair<unsigned, distT> > reqT;

class deltaStepping {
  using digraph = DS::digraph<nodeIdT>;

public:

  // This function performs the delta stepping algorithm according
  // to the given mode. It writes the results on the file whose name
  // is provided in the argument ~outFileName~.
  deltaStepping(digraph* inGraph, const char* mode, 
                const char* outFileName);
  ~deltaStepping();

private:
  digraph* diGraph;
  std::ofstream outFile;
  
  void initInternalVars(digraph* inGraph, const char* mode,
                const char* outFileName);
  void assignGraph(digraph* inGraph);
  void openOutFile(const char* outFileName);
  void invalidMode(const char* mode);

  //===--------------------------------------------------------===//
  // Structures used by the algorithm
  //===--------------------------------------------------------===//
  distsT dists;
  bucksT bucks;

  static constexpr float delta = 1000; // TODO: make this a user argument.
  const nodeIdT sourceNode;
  // Our reusable definition of infinity
  const distT infDist = std::numeric_limits<distT>::max();

  // rm is the current set of removed nodes
  boost::dynamic_bitset<> rm;
  std::list<std::pair<unsigned, distT> > req;

  // These are all the versions of the algorithm, that can be
  // differentiated according to the ~mode~ argument given to the
  // constructor.
  void sequential();
  void parallel();
  void parallelBucketFusion();

  void preprocessing();
  buckT* getMinBuck();
  // FIXME: we are currently getting some graph attributes, such as edge weight,
  // from outside the algorithm. This might not be possible in some
  // settings. The function fetchGraphGlobalAtts below is supposed to take care
  // of that in the future.
  //
  // void fetchGraphGlobalAtts();
  void initBucksDists();
  void initBucks();
  void initDists();
  void relax(nodeIdT, distT);
  reqT findRequests(const buckT& curBuck, const unsigned mode);
  reqT findRequestsAux(const buckT& curBuck, bool (*f) (weightT w));
  reqT findRequests(const boost::dynamic_bitset<>& curBuck, 
                    const unsigned mode);
  reqT findRequestsAux(const boost::dynamic_bitset<>& curBuck, 
                       bool (*f) (weightT w));
  void relaxRequests(reqT&);
  static bool isLight(weightT w);
  static bool isHeavy(weightT w);
  void bitsetListUnion(boost::dynamic_bitset<>&, const buckT&);

  //===--------------------------------------------------------===//
  // Debugging procedures
  //===--------------------------------------------------------===//
  void printBuck(buckT&);
  void printReq(reqT& req);
  void printBs(boost::dynamic_bitset<>& bs);
  void printDists(distsT& dists);
};

}

#endif
