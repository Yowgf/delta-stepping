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
  : sourceNode(0),
    kminBuckThreshold(1000),
    locks(nullptr)
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
  
void deltaStepping::run(digraph* inGraph, const char* mode, 
			const float delta, const unsigned numThreads)
{
  initInternalVars(inGraph, mode, delta, numThreads);

  const string modeStr{mode};
  if (modeStr == "original")
    original();
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
// Algorithm closest to the original descrition by Meyer, 1998
//===----------------------------------------------------------===//
void deltaStepping::original()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::original");
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
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::original");
}
  
//===----------------------------------------------------------===//
// Parallel algorithm
//===----------------------------------------------------------===//
void deltaStepping::parallel()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltastepping::parallel");
  preprocessingPrl();

  // Shared vars
  unsigned gMinBuckIdx = getMinBuckIdx();
  buckT* gMinBuck = &bucks.at(gMinBuckIdx);
  unsigned nextGMinBuckIdx = maxUns;
  unsigned prevGBuckSz = 0;
  unsigned gMinBuckStartIdx = 0;
  
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
      copyToGBuck(bucks, lBucks);
      lBucks.clear();
      #pragma omp barrier
      #pragma omp single nowait
      {
	unsigned updtGBuckSz = gMinBuck->size();
	if (prevGBuckSz == updtGBuckSz) { // No reinsertions!
	  gMinBuck->clear();
	  gMinBuckIdx = getMinBuckIdx();
	  gMinBuck = &bucks.at(gMinBuckIdx);
	  gMinBuckStartIdx = 0;
	}
	else {
	  gMinBuckStartIdx = prevGBuckSz;
	}
      }
      #pragma omp barrier
    }
  }
  postprocessingPrl();
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltastepping::parallel");
}

//===----------------------------------------------------------===//
// Parallel algorithm with bucket fusion
//===----------------------------------------------------------===//
void deltaStepping::parallelBucketFusion()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::parallelBucketFusion");
  
  preprocessingPrl();

  // Shared vars
  unsigned gMinBuckIdx = getMinBuckIdx();
  buckT* gMinBuck = &bucks.at(gMinBuckIdx);
  unsigned nextGMinBuckIdx = maxUns;
  unsigned prevGBuckSz = 0;
  unsigned gMinBuckStartIdx = 0;
  
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
      bucketFusion(lBucks);
      copyToGBuck(bucks, lBucks);
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

  postprocessingPrl();
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::parallelBucketFusion");
}


//===----------------------------------------------------------===//
// Auxiliary procedures
//===----------------------------------------------------------===//
inline
void deltaStepping::preprocessingPrl()
{
  initBucksDists();
  locks = new omp_lock_t[bucks.size()];
  for (unsigned i = 0; i < bucks.size(); ++i) {
    omp_init_lock(&locks[i]);
  }
}

inline
void deltaStepping::preprocessing()
{
  // Allocate buckets and distances
  initBucksDists();
  rm.resize(diGraph->getNumNodes());
}

inline
void deltaStepping::postprocessingPrl()
{
  delete[] locks;
}

inline
unsigned deltaStepping::getMinBuckIdx() {
  for (unsigned i = 0; i < bucks.size(); ++i) {
    if (!bucks.at(i).empty()) {
      return i;
    }
  }
  return maxUns;
}
  
inline
buckT* deltaStepping::getMinBuck()
{
  for (unsigned i = 0; i < bucks.size(); ++i) {
    if (!bucks.at(i).empty()) {
      return &bucks.at(i);
    }
  }
  return nullptr;
}

inline
void deltaStepping::bucketFusion(lBucksT& lBucks)
{
  if (!lBucks.empty()) {
    const unsigned lbIdx = 0;
    while (!lBucks[lbIdx].empty() &&
	   lBucks[lbIdx].size() < kminBuckThreshold) {
      // This copy is necessary, because we want to remove nodes from the
      //   local bucket, which is not a normal procedure. Generally, we just
      //   dump new nodes on the global bucket.
      //
      // In bucket fusion, if the bucket is small enough, we do all the work
      //   locally, instead of copying it to the global bucket for later
      //   processing.
      auto lBuckCopy = lBucks[lbIdx];
      lBucks[lbIdx].resize(0);
      for (auto srcNode : lBuckCopy) {
	relaxEdgesPrl(srcNode, lBucks);
      }
    }
  }
}

inline
void deltaStepping::initBucksDists()
{
  initBucks();
  initDists();
}

inline
void deltaStepping::initBucks()
{
  const unsigned numBuckets = 
    static_cast<unsigned>(ceil(diGraph->getMaxEdgeWeight() / delta) + 1);
  LOGATT(ALG_DELTASTEPPING_DEBUG, numBuckets);

  bucks.resize(numBuckets);
}

inline
void deltaStepping::initDists()
{
  // Initialize the distances to infinity, except for sourceNode
  dists.resize(diGraph->size(), infDist);
  relax(sourceNode, 0);
}

inline
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

inline
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

inline
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

inline
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

inline
void deltaStepping::relaxRequests(reqT& reqs)
{
  for (auto req : reqs)
    relax(req.first, req.second);
}

inline
void deltaStepping::relaxEdgesPrl(nodeIdT srcNodeId, lBucksT& lBucks)
{
  LOG(ALG_DELTASTEPPING_DEBUG, "%u: Start -- relaxEdgesPrl", omp_get_thread_num());
  auto* edges = diGraph->at(srcNodeId)->getOutEdges();
  if (edges != nullptr) {
    for (unsigned i = 0; i < edges->size(); ++i) {
      nodeIdT& destNodeId = edges->at(i).first;
      weightT& edgeWeight = edges->at(i).second;
      
      distT& oldDist = dists.at(destNodeId);
      distT newDist = dists.at(srcNodeId) + edgeWeight;
      if (newDist < oldDist) {
	#pragma omp atomic write
	dists[destNodeId] = newDist;
	unsigned buckPos = newDist / delta;
	if (buckPos >= lBucks.size()) {
	  lBucks.resize(buckPos + 1);
	}
	lBucks.at(buckPos).push_back(destNodeId);
      }
    }
  }
  LOG(ALG_DELTASTEPPING_DEBUG, "%u: End -- relaxEdgesPrl", omp_get_thread_num());
}

inline
void deltaStepping::copyToGBuck(bucksT& gBuck, lBucksT& lBucks)
{
  LOG(ALG_DELTASTEPPING_DEBUG, "%u: Start -- copyToGBuck", omp_get_thread_num());
  for (unsigned i = 0; i < lBucks.size(); ++i) {
    if (!lBucks.at(i).empty()) {
      omp_set_lock(&locks[i]);
      lBuckT& lRefBuck = lBucks.at(i);
      buckT& gRefBuck = bucks.at(i);      
      gRefBuck.insert(gRefBuck.end(), lRefBuck.begin(), lRefBuck.end());
      omp_unset_lock(&locks[i]);
    }
  }
  LOG(ALG_DELTASTEPPING_DEBUG, "%u: End -- copyToGBuck", omp_get_thread_num());
}

inline
void deltaStepping::relax(nodeIdT nid, distT newDist)
{
  LOG(ALG_DELTASTEPPING_DEBUG, "relaxing node %u with tentative distance %u",
      nid, newDist);
  if (newDist < dists.at(nid)) {
    bucks.at(newDist / delta).push_back(nid);
    dists.at(nid) = newDist;
  }
}

inline
bool deltaStepping::isLight(weightT w)
{
  return w <= delta;
}

inline
bool deltaStepping::isHeavy(weightT w)
{
  return w > delta;
}

inline
void deltaStepping::bitsetListUnion(boost::dynamic_bitset<>& bs, 
                                    const buckT& ls)
{
  for (auto nid : ls) {
    bs[nid] = 1;
  }
}

inline
void deltaStepping::recycleBucks()
{
  unsigned i = 0;
  while (bucks.at(i).empty() && i < bucks.size()) {
    ++i;
  }
  bucks.setBegin(i + bucks.getBegin());
}

//===----------------------------------------------------------===//
// Debugging procedures
//===----------------------------------------------------------===//
inline
void deltaStepping::printBuck(buckT& buck)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing bucket\n";
  for (auto nid : buck) {
    std::cerr << nid << ' ';
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

inline
void deltaStepping::printReq(reqT& reqs)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing Request\n";
  for (auto req : reqs) {
    std::cerr << '(' << req.first << ", " << req.second << ") ";
  }
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

inline
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

inline
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

inline
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

inline
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

