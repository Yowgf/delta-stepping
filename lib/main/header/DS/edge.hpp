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

template <typename n1T, typename n2T, typename wT>
class wEdge : public std::tuple<n1T, n2T, wT> {
public:
  n1T node1() { return std::get<0>(*this); }
  n2T node2() { return std::get<1>(*this); }
  wT weight() { return std::get<2>(*this); }
};

}

#endif
