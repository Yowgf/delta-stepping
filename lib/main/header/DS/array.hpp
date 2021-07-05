//===----------------------------------------------------------===//
// DS module
//
// File purpose: ~array~ class declaration and definition.
//===----------------------------------------------------------===//

#ifndef ARRAY_H
#define ARRAY_H

#include <cstring>
#include <stdexcept>
#include <string>

namespace DS {

template <typename valueType>
class array {
public:
  // Creates an array of size ~size~, and fills it with ~val~
  array(const unsigned size, const valueType& val) : arrSz(size)
  {
    fill(val);
  }

  array(const int size, const valueType& val)
  {
    checkPositiveSize(size);

    arrSz = static_cast<const unsigned>(size);
    fill(val);
  }

  array(const unsigned size) : array(size, 0) {}
  array(const int size) : array(size, 0) {}

  array() : arrSz(0), innerArr(nullptr) {}

  // Copy-constructors
  array(array& rhs)
  {
    arrSz = rhs.arrSz;
    if (innerArr) {
      free(innerArr);
    }
    register unsigned i = 0;
    for(i = 0; i < arrSz; ++i) {
      innerArr[i] = rhs.at(i);
    }        
  }

  array& operator =(array& rhs)
  {
    array{rhs};
    return *this;
  }
  // End copy-constructors
  
  ~array()
  {
    delete[] innerArr;
  }

  // Access
  valueType& at(const unsigned pos)
  {
    if (pos > arrSz) {
      throw std::out_of_range{
        std::string("Position ") + std::to_string(pos) +
          " is out of range"};
    }
    return innerArr[pos];
  }

  // Container information
  const unsigned size()
  {
    return arrSz;
  }

private:
  unsigned arrSz;
  valueType* innerArr;

  void fill(const valueType& val)
  {
    innerArr = new valueType[arrSz];
    
    register unsigned int i = 0;
    for (; i < arrSz; ++i) {
      innerArr[i] = val;
    }
  }

  void checkPositiveSize(const int size)
  {
    if (size < 0) {
      throw std::invalid_argument{
        std::string("negative ~size~: ") + std::to_string(size)};
    }
  }
      
};

}


#endif

