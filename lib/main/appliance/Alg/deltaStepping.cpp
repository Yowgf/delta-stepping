//===----------------------------------------------------------===//
// Alg module
//
// File purpose: ~deltaStepping~ class implementation. See header
// file for more detail
//===----------------------------------------------------------===//

#include "Alg/deltaStepping.hpp"
#include "Utils/str.hpp"
#include "Utils/num.hpp"

#ifdef _OPENMP
#include "omp.h"
#endif

#include <cmath>
#include <stdexcept>

using namespace std;
using namespace Utils;

namespace Alg {

using digraph = DS::digraph<nodeIdT>;

deltaStepping::deltaStepping()
  : sourceNode(0) // TODO: find more sofisticated way to decide on this
{}

void deltaStepping::printOutToFile(const char* outFileName)
{
  outFile.open(outFileName, ios_base::out | ios_base::trunc);
  printOutToStream(outFile);
  outFile.close();
}

void deltaStepping::printOutToStream(std::ostream& os)
{
  for (unsigned i = 0; i < dists.size(); ++i) {
    os << sourceNode << ' ' << dists.at(i) << '\n';
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
			const unsigned numThreads)
{
  // Intialize internal variables
  initInternalVars(inGraph, mode, numThreads);

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
}

//===----------------------------------------------------------===//
// Aux functions for the constructor.
//===----------------------------------------------------------===//

void deltaStepping::initInternalVars(digraph* inGraph, const char* mode,
				     const unsigned numThreads)
{
  if (numThreads < 1) {
    throw std::logic_error{string("Invalid number of threads '") +
			     to_string(numThreads) + "'"};
  }
  this->numThreads = numThreads;
  rm.resize(inGraph->getNumNodes());
  tlBucks.assign(numThreads, maxUns);
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
      if (ALG_DELTASTEPPING_DEBUG)
	printDists(dists);
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

  unsigned numActiveThreads = getMinBuckPrl(tlBucks);
  while (numActiveThreads != 0) {
    LOG(ALG_DELTASTEPPING_DEBUG, "Outer Loop!");

#   pragma omp parallel num_threads(numActiveThreads)
    {
      unsigned myRank = omp_get_thread_num();
      unsigned myBuckIdx = tlBucks.at(myRank);


      if (myBuckIdx != maxUns) {
	buckT* myBuck = &bucks.at(myBuckIdx);
	nodeIdT srcNodeId;
	srcNodeId = myBuck->front();

#       pragma omp critical
	myBuck->pop_front();

	auto srcNode = diGraph->at(srcNodeId);
	auto* edges = srcNode->getOutEdges();
	if (edges != nullptr) {
	  for (unsigned edgeIdx = 0; edgeIdx < edges->size(); ++edgeIdx) {
	    LOG(ALG_DELTASTEPPING_DEBUG, "Inner Loop. EdgeIdx: %u", edgeIdx);
	  
	    nodeIdT& destNodeId = edges->at(edgeIdx).first;
	    weightT& edgeWeight = edges->at(edgeIdx).second;
	    // FIXME: use atomic pragmas instead of wrapping everything in critical
#           pragma omp critical(parallelCriticalUpdateDists)
	    {
	      distT newDist = dists.at(srcNodeId) + edgeWeight;
	      if (newDist < dists.at(destNodeId)) {
	        bucks.at(newDist / delta).push_back(destNodeId);
	        dists.at(destNodeId) = newDist;
	      }
	    }
	  }
	}
      }
    }
    
    numActiveThreads = getMinBuckPrl(tlBucks);
  }
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltastepping::parallel");
}

  
void deltaStepping::parallelListDeprecated()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::parallelListDeprecated");
  
  preprocessing();

  buckT* minBuck = nullptr;
  while (minBuck = getMinBuck()) {

    req.clear();
    
#   pragma omp parallel for					\
  num_threads(num<unsigned>::min(numThreads, minBuck->size()))
    for (unsigned buckIdx = 0; buckIdx < minBuck->size(); ++buckIdx) {
      nodeIdT srcNodeId;
      srcNodeId = minBuck->front();
#     pragma omp critical
      minBuck->pop_front();

      auto srcNode = diGraph->at(srcNodeId);
      auto* edges = srcNode->getOutEdges();
      if (edges != nullptr) {
	for (unsigned edgeIdx = 0; edgeIdx < edges->size(); ++edgeIdx) {
	  nodeIdT& destNodeId = edges->at(edgeIdx).first;
	  weightT& edgeWeight = edges->at(edgeIdx).second;

	  distT newDist = dists.at(srcNodeId) + edgeWeight;
	  if (newDist < dists.at(destNodeId)) {
#           pragma omp critical
	    {
	      if (dists.at(destNodeId) != infDist) {
		bucks.at(dists.at(destNodeId) / delta).remove(destNodeId);
	      }
	      bucks.at(newDist / delta).push_back(destNodeId);
	      dists.at(destNodeId) = newDist;
	    }
	  }
	}
      }
    }
  }
  
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::parallelListDeprecated");
}


//===----------------------------------------------------------===//
// Parallel algorithm with bucket fusion
//===----------------------------------------------------------===//
void deltaStepping::parallelBucketFusion()
{
  LOG(ALG_DELTASTEPPING_DEBUG, "Start -- deltaStepping::parallelBucketFusion");
  LOG(ALG_DELTASTEPPING_DEBUG, "End -- deltaStepping::parallelBucketFusion");
}


//===----------------------------------------------------------===//
// Auxiliary procedures
//===----------------------------------------------------------===//

void deltaStepping::preprocessingPrl()
{
  preprocessing();
}
  
void deltaStepping::preprocessing()
{
  // Allocate buckets and distances
  initBucksDists();
}

unsigned deltaStepping::getMinBuckPrl(tlBucksT& tlBucks) {
  unsigned bIdx = 0;
  unsigned tlbIdx = 0;
  for (bIdx = 0; bIdx < bucks.size() && tlbIdx < tlBucks.size(); ++bIdx) {
    if (!bucks.at(bIdx).empty()) {
      tlBucks.at(tlbIdx++) = bIdx;
    }
  }
  unsigned numActiveThreads = tlbIdx;
  LOGATT(ALG_DELTASTEPPING_DEBUG, numActiveThreads);
  // Replace the old buckets with a special value, if didnt find new ones.
  while (tlbIdx < tlBucks.size()) {
    tlBucks.at(tlbIdx++) = maxUns;
  }
  return numActiveThreads;
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

void deltaStepping::relax(nodeIdT nid, distT newDist)
{
  LOG(ALG_DELTASTEPPING_DEBUG, "relaxing node %u with tentative distance %u",
      nid, newDist);
  if (dists.at(nid) != infDist) {
    if (newDist < dists.at(nid)) {
      bucks.at(dists.at(nid) / delta).remove(nid);
      bucks.at(newDist / delta).push_back(nid);
      dists.at(nid) = newDist;
    }
  }
  else {
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

// FIXME: deprecated
bool deltaStepping::isEmpty(tlBucksT& tlBucks)
{
  return tlBucks.front() == maxUns;
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

void deltaStepping::printTlBucks(tlBucksT& tlBucks)
{
  std::cerr << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cerr << "Printing tlBucks\n";
  for (auto buckId : tlBucks) {
    std::cerr << buckId << ' ';
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

}

