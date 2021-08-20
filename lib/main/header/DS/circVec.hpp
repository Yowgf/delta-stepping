//===----------------------------------------------------------===//
// DS module
//
// File purpose: ~circVec~ class declaration and definition.
//===----------------------------------------------------------===//


#ifndef CIRCVEC_H
#define CIRCVEC_H

#include <vector>

namespace DS {

template <class valueT>
class circVec : public std::vector<valueT> {
public:
  explicit circVec() : vecBeggining(0)
  {}

  explicit circVec(unsigned sz, const valueT& val = valueT())
    : vecBeggining(0), std::vector<valueT>(sz, val)
  {}

  valueT& at(unsigned pos)  {
    return this->data()[(vecBeggining + pos) % this->size()];
  }

  void setBeggining(unsigned newVal)
  {
    vecBeggining = newVal;
  }

  unsigned getBeggining() {
    return vecBeggining;
  }

private:
  unsigned vecBeggining;

};
  
}

#endif
