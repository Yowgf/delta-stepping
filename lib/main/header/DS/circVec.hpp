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
  explicit circVec() : vecBegin(0)
  {}

  explicit circVec(unsigned sz, const valueT& val = valueT())
    : vecBegin(0), std::vector<valueT>(sz, val)
  {}

  valueT& at(unsigned pos)  {
    return this->data()[(vecBegin + pos) % this->size()];
  }

  void setBegin(unsigned newVal)
  {
    vecBegin = newVal;
  }

  unsigned getBegin() {
    return vecBegin;
  }

private:
  unsigned vecBegin;

};
  
}

#endif
