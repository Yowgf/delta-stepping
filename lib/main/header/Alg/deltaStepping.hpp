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

#include "DS/circVec.hpp"
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
typedef DS::circVec<buckT> bucksT;
typedef std::vector<unsigned> tlBucksT;
typedef std::list<std::pair<unsigned, distT> > reqT;

class deltaStepping {
  using digraph = DS::digraph<nodeIdT>;

public:

  // This function performs the delta stepping algorithm according
  // to the given mode. It writes the results on the file whose name
  // is provided in the argument ~outFileName~.
  deltaStepping();
  ~deltaStepping();

  void run(digraph* inGraph, const char* mode, const unsigned numThreads);
  distsT& getDists() { return dists; }
  void printOutToFile(const char* outFileName);
  void printOutToStream(std::ostream& os);
private:
  digraph* diGraph;
  std::ofstream outFile;
  unsigned numThreads;
  
  void initInternalVars(digraph* inGraph, const char* mode,
			const unsigned numThreads);
  void assignGraph(digraph* inGraph);
  void invalidMode(const char* mode);

  //===--------------------------------------------------------===//
  // Structures used by the algorithm
  //===--------------------------------------------------------===//
  distsT dists;
  bucksT bucks;
  tlBucksT tlBucks;

  static constexpr float delta = 100; // TODO: make this a user argument.
  const nodeIdT sourceNode;
  // Our reusable definition of infinity
  const distT infDist = std::numeric_limits<distT>::max();
  const unsigned maxUns = std::numeric_limits<unsigned>::max();

  // rm is the current set of removed nodes
  boost::dynamic_bitset<> rm;
  std::list<std::pair<unsigned, distT> > req;

  //===--------------------------------------------------------===//
  // Procedures used by the algorithm
  //===--------------------------------------------------------===//
  // These are all the versions of the algorithm, that can be
  // differentiated according to the ~mode~ argument given to the
  // constructor.
  void sequential();
  void parallel();
  void parallelListDeprecated();
  void parallelBucketFusion();

  void preprocessingPrl();
  void preprocessing();
  unsigned getMinBuckPrl(tlBucksT& tlBucks);
  buckT* getMinBuck();
  // FIXME: we are currently getting some graph attributes, such as edge weight,
  // from outside the algorithm. This might not be possible in some
  // settings. The function fetchGraphGlobalAtts below is supposed to take care
  // of that in the future.
  //
  // void fetchGraphGlobalAtts();
  void initThreadLocalBucks();
  void initBucksDists();
  void initBucks();
  void initDists();
  reqT findRequests(const buckT& curBuck, const unsigned mode);
  reqT findRequestsAux(const buckT& curBuck, bool (*f) (weightT w));
  reqT findRequests(const boost::dynamic_bitset<>& curBuck, 
                    const unsigned mode);
  reqT findRequestsAux(const boost::dynamic_bitset<>& curBuck, 
                       bool (*f) (weightT w));
  void relaxRequests(reqT&);
  void relax(nodeIdT, distT);
  static bool isLight(weightT w);
  static bool isHeavy(weightT w);
  void bitsetListUnion(boost::dynamic_bitset<>&, const buckT&);
  void recycleBucks();
  // FIXME: Deprecated
  bool isEmpty(tlBucksT&);
  
  //===--------------------------------------------------------===//
  // Debugging procedures
  //===--------------------------------------------------------===//
  void printBuck(buckT&);
  void printTlBucks(tlBucksT&);
  void printReq(reqT& req);
  void printBs(boost::dynamic_bitset<>& bs);
  void printDists(distsT& dists);
};

}

#endif
