//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class implementation. See header
// file for more detail
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"
#include "Utils/fun.hpp"
#include "Utils/num.hpp"
#include "Utils/str.hpp"

#ifdef _OPENMP
#include "omp.h"
#endif

#include <cassert>
#include <cmath>
#include <queue>
#include <stdexcept>

using namespace std;
using namespace Utils;

namespace Alg {

using digraph = DS::digraph<nodeIdT>;

// Static members
float deltaStepping::delta = 0.0;

deltaStepping::deltaStepping()
  : sourceNode(0), // TODO: find more sofisticated way to decide on this
    kminBuckThreshold(1000)
{}


distsT& deltaStepping::getDists()
{
  return dists;
}    

void deltaStepping::printOutToFile(const char* outFileName)
{
  if (outFile.is_open()) {
    outFile.close();
  }
  outFile.open(outFileName, ios_base::out | ios_base::trunc);
  printOutToStream(outFile);
  outFile.close();
}

void deltaStepping::printOutToStream(std::ostream& os)
{
  for (unsigned i = 0; i < dists.size(); ++i) {
    os << i << ' ' << dists.at(i) << '\n';
  }  
}
  
deltaStepping::~deltaStepping()
{}

  
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
void deltaStepping::run(digraph* inGraph, const char* mode, 
			const float delta, const unsigned numThreads)
{
  // Intialize internal variables
  initInternalVars(inGraph, mode, delta, numThreads);

  // TODO: is there anything that should be done under every mode?
  const string modeStr{mode};
  if (modeStr == "sequential")
    sequential();
  else if (modeStr == "parallel") {
    if (ALG_DELTASTEPPING_ASSERT) {
      assertEqualRes(&deltaStepping::parallel,
		     &deltaStepping::dijkstra);
    }
    else {
      parallel();
    }
  }
  else if (modeStr == "parallel-bucket-fusion") {
    if (ALG_DELTASTEPPING_ASSERT) {
      assertEqualRes(&deltaStepping::parallelBucketFusion,
		     &deltaStepping::dijkstra);
    }
    else {
      parallelBucketFusion();
    }      
  }
  else if (modeStr == "dijkstra")
    dijkstra();
  else
    invalidMode(mode);
}

//===----------------------------------------------------------===//
// Aux functions for the constructor.
//===----------------------------------------------------------===//

void deltaStepping::initInternalVars(digraph* inGraph, const char* mode,
				     const float delta,
				     const unsigned numThreads)
{
  if (delta < 1.0) {
    throw std::logic_error{string("Invalid delta '") +
			     to_string(delta) + "'"};
  }
  this->delta = delta;
  if (numThreads < 1) {
    throw std::logic_error{string("Invalid number of threads '") +
			     to_string(numThreads) + "'"};
  }
  this->numThreads = numThreads;
  assignGraph(inGraph);
}

void deltaStepping::assignGraph(digraph* inGraph) {
  if (inGraph == nullptr) {
    throw invalid_argument{"(assignGraph) nullptr inGraph argument"};
  }
  diGraph = inGraph;
}

void deltaStepping::invalidMode(const char* mode)
{
  throw invalid_argument{
    string("Unknown option '") + mode + "' for the deltaStepping algorithm"};
}

//===----------------------------------------------------------===//
// Below is the implementation of the main algorithms
//===----------------------------------------------------------===//

typedef std::pair<nodeIdT, distT> dijkstraPair; 
class ordDijkstra {
public:
  bool operator() (const dijkstraPair& a, const dijkstraPair& b) {
    return a.second > b.second;
  }
};
void deltaStepping::dijkstra()
{
  std::priority_queue<dijkstraPair, std::vector<dijkstraPair>,
		      ordDijkstra> frontier;
  std::vector<bool> isVisited(diGraph->getNumNodes(), false);
  dists.resize(diGraph->getNumNodes(), infDist);
  dists.at(sourceNode) = 0;
  frontier.push(make_pair(sourceNode, dists.at(sourceNode)));
  while (!frontier.empty()) {
    nodeIdT srcNodeId = frontier.top().first;
    frontier.pop();
    isVisited.at(srcNodeId) = true;
    auto* srcNode = diGraph->at(srcNodeId);
    auto* edges = srcNode->getOutEdges();
    if (edges != nullptr) {
      for (unsigned i = 0; i < edges->size(); ++i) {
	auto& edge = edges->at(i);
	auto& destNodeId = edge.first;
	auto& edgeWeight = edge.second;
	distT& oldDist = dists.at(destNodeId);
	distT newDist = dists.at(srcNodeId) + edgeWeight;
	if (newDist < oldDist) {
	  dists.at(destNodeId) = newDist;
	  if (!isVisited.at(destNodeId)) {
	    frontier.push(make_pair(destNodeId, dists.at(destNodeId)));
	  }
	}
      }
    }    
  }
}  

//===----------------------------------------------------------===//
// Sequential algorithm
//===----------------------------------------------------------===//
void deltaStepping::sequential()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::sequential");
  preprocessing();
  while (true) {
    buckT* minBuck = getMinBuck();
    if (minBuck == nullptr) // Means bucks is empty
      break;
    rm.reset();
    while (!minBuck->empty()) {
      if (ALG_DELTASTEPPING_DEBUG)
        printBuck(*minBuck);
      // Find requests for light edges
      req = findRequests(*minBuck, 0);
      if (ALG_DELTASTEPPING_DEBUG)
	printReq(req);
      bitsetListUnion(rm, *minBuck);
      if (ALG_DELTASTEPPING_DEBUG)
	printBs(rm);
      // Clear the bucket
      minBuck->clear();
      // Relax the light edges requests (may reintroduce some nodes)
      relaxRequests(req);
    }
    // Process requests for heavy edges
    req = findRequests(rm, 1);
    relaxRequests(req);
    // Recycle buckets
    recycleBucks();
  }
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::sequential");
}
  
//===----------------------------------------------------------===//
// Parallel algorithm
//===----------------------------------------------------------===//
void deltaStepping::parallel()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltastepping::parallel");
  preprocessingPrl();

  // Global vars
  unsigned gMinBuckIdx = getMinBuckIdx();
  buckT* gMinBuck = &bucks.at(gMinBuckIdx);
  unsigned nextGMinBuckIdx = maxUns;
  unsigned prevGBuckSz = 0;
  unsigned gMinBuckStartIdx = 0;
  
  // It seems to be a good idea to not place the parallel directive inside the
  // main loop, to avoid thread management overheads.
  #pragma omp parallel num_threads(numThreads)
  {
    lBucksT lBucks; // Local buckets
    while (gMinBuckIdx != maxUns) {
      #pragma omp single nowait
      prevGBuckSz = gMinBuck->size();
      #pragma omp for nowait schedule(dynamic, 64)
      for (unsigned i = gMinBuckStartIdx; i < gMinBuck->size(); ++i) {
	nodeIdT srcNode = gMinBuck->at(i);
	relaxEdgesPrl(srcNode, lBucks);
      }
      
      #pragma omp barrier
      // Copy local buckets to global one
      LOG(ALG_DELTASTEPPING_DEBUG, "Printing local buckets");
      for (unsigned i = 0; i < lBucks.size(); ++i) {
	if (!lBucks.at(i).empty()) {
          #pragma omp critical
	  {
	    LOG(ALG_DELTASTEPPING_DEBUG, "gbuck before copy:");
	    if (ALG_DELTASTEPPING_DEBUG) {
	      printBuck(bucks.at(i));
	    }
	    lBuckT& lRefBuck = lBucks.at(i);
	    buckT& gRefBuck = bucks.at(i);
	    gRefBuck.insert(gRefBuck.end(), lRefBuck.begin(), lRefBuck.end());
	    LOG(ALG_DELTASTEPPING_DEBUG, "gbuck after copy:");
	    if (ALG_DELTASTEPPING_DEBUG) {
	      printBuck(bucks.at(i));
	    }
	  }
	}
      }
      #pragma omp barrier

      #pragma omp single nowait
      {
	unsigned updtGBuckSz = gMinBuck->size();
	if (prevGBuckSz == updtGBuckSz) { // No reinsertions!
	  LOG(ALG_DELTASTEPPING_DEBUG, "NO REINSERTIONS IN BUCKET!");
	  gMinBuck->clear();
	  gMinBuckIdx = getMinBuckIdx();
	  gMinBuck = &bucks.at(gMinBuckIdx);
	  gMinBuckStartIdx = 0;
	}
	else {
	  LOG(ALG_DELTASTEPPING_DEBUG, "FOUND REINSERTION! JUST UPDATE THE NEW STARTING INDEX");
	  gMinBuckStartIdx = prevGBuckSz;
	}
      }
      
      lBucks.clear();
      #pragma omp barrier
    }
  }
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltastepping::parallel");
}

//===----------------------------------------------------------===//
// Parallel algorithm with bucket fusion
//===----------------------------------------------------------===//
void deltaStepping::parallelBucketFusion()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::parallelBucketFusion");
  preprocessingPrl();

  // Global vars
  unsigned gMinBuckIdx = getMinBuckIdx();
  buckT* gMinBuck = &bucks.at(gMinBuckIdx);
  unsigned nextGMinBuckIdx = maxUns;
  unsigned prevGBuckSz = 0;
  unsigned gMinBuckStartIdx = 0;
  
  // It seems to be a good idea to not place the parallel directive inside the
  // main loop, to avoid thread management overheads.
  #pragma omp parallel num_threads(numThreads)
  {
    lBucksT lBucks; // Local buckets
    while (gMinBuckIdx != maxUns) {
      #pragma omp single
      prevGBuckSz = gMinBuck->size();
      #pragma omp for nowait schedule(dynamic, 64)
      for (unsigned i = gMinBuckStartIdx; i < gMinBuck->size(); ++i) {
	nodeIdT srcNode = gMinBuck->at(i);
	relaxEdgesPrl(srcNode, lBucks);
      }

      // Bucket fusion
      if (!lBucks.empty()) {
	unsigned lbIdx = 0;
	while (!lBucks[lbIdx].empty() &&
	       lBucks[lbIdx].size() < kminBuckThreshold) {
	  // This copy is necessary, because we want to remove nodes
	  //   from the local bucket, which is not a normal procedure.
	  //
	  // In bucket fusion, if the bucket is small enough, we do all
	  //   the work locally, instead of copying it to the global
	  //   bucket for later processing.
	  auto lBuckCopy = lBucks[lbIdx];
	  lBucks[lbIdx].resize(0);
	  for (auto srcNode : lBuckCopy) {
	    relaxEdgesPrl(srcNode, lBucks);
	  }
	}
      }
      
      #pragma omp barrier
      // Copy local buckets to global one
      LOG(ALG_DELTASTEPPING_DEBUG, "Printing local buckets");
      for (unsigned i = 0; i < lBucks.size(); ++i) {
	if (!lBucks.at(i).empty()) {
          #pragma omp critical
	  {
	    LOG(ALG_DELTASTEPPING_DEBUG, "gbuck before copy:");
	    if (ALG_DELTASTEPPING_DEBUG) {
	      printBuck(bucks.at(i));
	    }
	    lBuckT& lRefBuck = lBucks.at(i);
	    buckT& gRefBuck = bucks.at(i);
	    gRefBuck.insert(gRefBuck.end(), lRefBuck.begin(), lRefBuck.end());
	    LOG(ALG_DELTASTEPPING_DEBUG, "gbuck after copy:");
	    if (ALG_DELTASTEPPING_DEBUG) {
	      printBuck(bucks.at(i));
	    }
	  }
	}
      }
      #pragma omp barrier

      #pragma omp single nowait
      {
	unsigned updtGBuckSz = gMinBuck->size();
	if (prevGBuckSz == updtGBuckSz) { // No reinsertions!
	  LOG(ALG_DELTASTEPPING_DEBUG, "NO REINSERTIONS IN BUCKET!");
	  gMinBuck->clear();
	  gMinBuckIdx = getMinBuckIdx();
	  gMinBuck = &bucks.at(gMinBuckIdx);
	  gMinBuckStartIdx = 0;
	}
	else {
	  LOG(ALG_DELTASTEPPING_DEBUG, "FOUND REINSERTION! JUST UPDATE THE NEW STARTING INDEX");
	  gMinBuckStartIdx = prevGBuckSz;
	}
      }
      
      lBucks.clear();
      #pragma omp barrier
    }
  }
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::parallelBucketFusion");
}


//===----------------------------------------------------------===//
// Auxiliary procedures
//===----------------------------------------------------------===//

void deltaStepping::preprocessingPrl()
{
  initBucksDists();
}
  
void deltaStepping::preprocessing()
{
  // Allocate buckets and distances
  initBucksDists();
  rm.resize(diGraph->getNumNodes());
}

unsigned deltaStepping::getMinBuckIdx() {
  for (unsigned i = 0; i < bucks.size(); ++i) {
    if (!bucks.at(i).empty()) {
      return i;
    }
  }
  return maxUns;
}
  

buckT* deltaStepping::getMinBuck()
{
  for (unsigned i = 0; i < bucks.size(); ++i) {
    if (!bucks.at(i).empty()) {
      return &bucks.at(i);
    }
  }
  return nullptr;
}

void deltaStepping::initBucksDists()
{
  initBucks();
  initDists();
}

// Initialize buckets
void deltaStepping::initBucks()
{
  const unsigned numBuckets = 
    static_cast<unsigned>(ceil(diGraph->getMaxEdgeWeight() / delta) + 1);
  LOGATT(ALG_DELTASTEPPING_DEBUG, numBuckets);

  bucks.resize(numBuckets);
}

// Initialize tentative distances
// TODO: make sure this is correct.
void deltaStepping::initDists()
{
  // Initialize the distances to infinity, except for sourceNode
  dists.resize(diGraph->size(), infDist);
  relax(sourceNode, 0);
}

// TODO: to optimize this function, find both light and heavy requests at once,
// for the nodes being searched.
reqT deltaStepping::findRequests(const buckT& curBuck, const unsigned mode)
{
  switch (mode) {
    case 0:
      return findRequestsAux(curBuck, isLight);
    case 1:
      return findRequestsAux(curBuck, isHeavy);
    default:
      throw std::invalid_argument{
        string("(findRequests) Invalid mode ") + to_string(mode)};
  }
}

reqT deltaStepping::findRequestsAux(const buckT& curBuck, 
                                    bool (*f)(weightT))
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- findRequestsAux");
  reqT req;
  for (auto srcNodeId : curBuck) {
    auto* edges = diGraph->at(srcNodeId)->getOutEdges();
    if (edges != nullptr) {
      for (unsigned i = 0; i < edges->size(); ++i) {
	nodeIdT& destNodeId = edges->at(i).first;
	weightT& edgeWeight = edges->at(i).second;
	if (f(edgeWeight)) {
	  req.push_back(std::make_pair(destNodeId,
				       dists.at(srcNodeId) + edgeWeight));
	}
      }
    }
  }
  return req;
}

// TODO: to optimize this function, find both light and heavy requests at once,
// for the nodes being searched.
reqT deltaStepping::findRequests(const boost::dynamic_bitset<>& curBuck,
                                 const unsigned mode)
{
  switch (mode) {
    case 0:
      return findRequestsAux(curBuck, isLight);
    case 1:
      return findRequestsAux(curBuck, isHeavy);
    default:
      throw std::invalid_argument{
        string("(findRequests) Invalid mode ") + to_string(mode)};
  }
}

reqT deltaStepping::findRequestsAux(const boost::dynamic_bitset<>& curBuck, 
                                    bool (*f)(weightT))
{
  reqT req;
  for (unsigned srcNodeId = 0; srcNodeId < curBuck.size(); ++srcNodeId) {
    if (curBuck[srcNodeId]) {
      auto* edges = diGraph->at(srcNodeId)->getOutEdges();
      if (edges != nullptr) {
        for (unsigned i = 0; i < edges->size(); ++i) {
	  nodeIdT& destNodeId = edges->at(i).first;
          weightT& edgeWeight = edges->at(i).second;
          if (f(edgeWeight)) {
            req.push_back(std::make_pair(destNodeId,
					 dists.at(srcNodeId) + edgeWeight));
          }
        }
      }
    }
  }
  return req;
}

void deltaStepping::relaxRequests(reqT& reqs)
{
  for (auto req : reqs)
    relax(req.first, req.second);
}

inline
void deltaStepping::relaxEdgesPrl(nodeIdT srcNodeId, lBucksT& lBucks)
{
  auto* edges = diGraph->at(srcNodeId)->getOutEdges();
  if (edges != nullptr) {
    for (unsigned i = 0; i < edges->size(); ++i) {
      nodeIdT& destNodeId = edges->at(i).first;
      weightT& edgeWeight = edges->at(i).second;
      
      distT& oldDist = dists.at(destNodeId);
      distT newDist = dists.at(srcNodeId) + edgeWeight;
      if (newDist < oldDist) {
	// FIXME: this might not be thread-safe
	dists.at(destNodeId) = newDist;
	unsigned buckPos = newDist / delta;
	if (buckPos >= lBucks.size()) {
	  lBucks.resize(buckPos + 1);
	}
	lBucks.at(buckPos).push_back(destNodeId);
      }
    }
  }
}

void deltaStepping::relax(nodeIdT nid, distT newDist)
{
  LOG(ALG_DELTASTEPPING_DEBUG, "relaxing node %u with tentative distance %u",
      nid, newDist);
  if (newDist < dists.at(nid)) {
    bucks.at(newDist / delta).push_back(nid);
    dists.at(nid) = newDist;
  }
}

bool deltaStepping::isLight(weightT w)
{
  return w <= delta;
}

bool deltaStepping::isHeavy(weightT w)
{
  return w > delta;
}

void deltaStepping::bitsetListUnion(boost::dynamic_bitset<>& bs, 
                                    const buckT& ls)
{
  for (auto nid : ls) {
    bs[nid] = 1;
  }
}

void deltaStepping::recycleBucks()
{
  unsigned i = 0;
  while (bucks.at(i).empty() && i < bucks.size()) {
    ++i;
  }
  bucks.setBeggining(i + bucks.getBeggining());
}

//===----------------------------------------------------------===//
// Debugging procedures
//===----------------------------------------------------------===//
void deltaStepping::printBuck(buckT& buck)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing bucket\n";
  for (auto nid : buck) {
    std::cerr << nid << ' ';
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void deltaStepping::printReq(reqT& reqs)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing Request\n";
  for (auto req : reqs) {
    std::cerr << '(' << req.first << ", " << req.second << ") ";
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void deltaStepping::printBs(boost::dynamic_bitset<>& bs)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing Bitset\n";
  for (unsigned i = 0; i < bs.size(); ++i) {
    if (bs[i]) {
      std::cerr << i << ' ';
    }
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void deltaStepping::printDists(distsT& dists)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing (Tentative) Distances\n";
  for (unsigned i = 0; i < dists.size(); ++i) {
    if (dists.at(i) != infDist) {
      std::cerr << i << ": " << dists.at(i) << ", ";
    }
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void deltaStepping::assertEqualRes(void (deltaStepping::* f1)(),
				   void (deltaStepping::* f2)())
{
  LOG(ALG_DELTASTEPPING_ASSERT, "Start -- assertEqualRes");
  (this->*f1)();
  distsT dists1 = dists;
  (this->*f2)();
  distsT& dists2 = dists;

  assert(compareDists(dists1, dists2));
  LOG(ALG_DELTASTEPPING_ASSERT, "End -- assertEqualRes");
}

bool deltaStepping::compareDists(distsT& d1, distsT& d2)
{
  if (d1.size() != d2.size()) {
    return false;
  }
  for (unsigned i = 0; i < d1.size(); ++i) {
    if (d1.at(i) != d2.at(i)) {
      return false;
    }
  }
  return true;
}

}

