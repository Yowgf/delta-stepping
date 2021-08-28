//===----------------------------------------------------------===//
// DS module
//
// File purpose: declaration and implementation of weightedEdge
//   class.
//===----------------------------------------------------------===//

#ifndef WEDGE_H
#define WEDGE_H

#include <tuple>

namespace DS {

template <class n1T, class n2T, class wT>
class wEdge {
public:
  std::tuple<n1T, n2T, wT> innerTup;

  wEdge(n1T n1, n2T n2, wT w) { innerTup = std::make_tuple(n1, n2, w); }

  n1T& node1() { return std::get<0>(innerTup); }
  n2T& node2() { return std::get<1>(innerTup); }
  wT& weight() { return std::get<2>(innerTup); }
};

}

#endif
