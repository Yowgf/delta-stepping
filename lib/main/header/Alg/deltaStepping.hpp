//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class declaration.
//===----------------------------------------------------------===//

// TODO: get twitter benchmark
//
// TODO: make sure the us road benchmark is the correct one. If so,
//   insert unitary weights in it.
// 
// TODO: maybe remove dynamic_bitset-dependent implementations
//
// TODO: Rename the original paper implementation, to indicate
// precisely this.

#ifndef DELTA_STEPPING_H
#define DELTA_STEPPING_H

#ifdef _OPENMP
#include "omp.h"
#endif

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
typedef std::vector<nodeIdT> buckT;
typedef DS::circVec<buckT> bucksT;
typedef std::list<std::pair<unsigned, distT> > reqT;

// Separated declarations for parallel implementation.
typedef std::vector<nodeIdT> lBuckT; // Local
typedef std::vector<lBuckT> lBucksT; // Local

class deltaStepping {
  using digraph = DS::digraph<nodeIdT>;

public:

  // This function performs the delta stepping algorithm according
  // to the given mode. It writes the results on the file whose name
  // is provided in the argument ~outFileName~.
  deltaStepping();
  ~deltaStepping();

  void run(digraph* inGraph, const char* mode, const float delta,
	   const unsigned numThreads);
  distsT& getDists();
  void printOutToFile(const char* outFileName);
  void printOutToStream(std::ostream& os);
  
private:
  digraph* diGraph;
  std::ofstream outFile;
  unsigned numThreads;
  
  void initInternalVars(digraph* inGraph, const char* mode,
			const float delta, const unsigned numThreads);
  void assignGraph(digraph* inGraph);
  void invalidMode(const char* mode);

  //===--------------------------------------------------------===//
  // Structures used by the algorithm
  //===--------------------------------------------------------===//
  distsT dists;
  bucksT bucks;

  static float delta;
  const nodeIdT sourceNode;
  // Our reusable definition of infinity
  const distT infDist = std::numeric_limits<distT>::max();
  const unsigned maxUns = std::numeric_limits<unsigned>::max();

  omp_lock_t* locks;
  // Parallel bucket fusion needs this
  const unsigned kminBuckThreshold;

  // rm is the current set of removed nodes
  boost::dynamic_bitset<> rm;
  std::list<std::pair<unsigned, distT> > req;

  //===--------------------------------------------------------===//
  // Procedures used by the algorithm
  //===--------------------------------------------------------===//
  // Ground truth version
  void dijkstra();
  // These are all the versions of the algorithm, that can be
  // differentiated according to the ~mode~ argument given to the
  // constructor.
  void sequential();
  void parallel();
  void parallelBucketFusion();

  void preprocessingPrl();
  void preprocessing();
  void postprocessingPrl();
  unsigned getMinBuckIdx();
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
  // Relaxes the outgoing edges of srcNode
  void relaxEdgesPrl(nodeIdT srcNodeId, lBucksT& lBucks);
  void copyToGBuck(bucksT&, lBucksT&);
  void relax(nodeIdT, distT);
  static bool isLight(weightT w);
  static bool isHeavy(weightT w);
  void bitsetListUnion(boost::dynamic_bitset<>&, const buckT&);
  void recycleBucks();
  
  //===--------------------------------------------------------===//
  // Debugging procedures
  //===--------------------------------------------------------===//
  void printBuck(buckT&);
  void printReq(reqT& req);
  void printBs(boost::dynamic_bitset<>& bs);
  void printDists(distsT& dists);
  void assertEqualRes(void (deltaStepping::* f1)(),
		      void (deltaStepping::* f2)());
  bool compareDists(distsT&, distsT&);
};

}

#endif
