#ifndef __BaseArray_hpp__
#define __BaseArray_hpp__

#include "NTuple.hpp"

template <typename T>
class BaseArray {
public:
  virtual const NTuple& dimensions() const = 0;
  virtual const T& operator[](const NTuple& pos) const = 0;
  virtual T& operator[](const NTuple& pos) = 0;
  virtual const T& operator[](index_t pos) const = 0;
  virtual T& operator[](index_t pos) = 0;
  virtual ~BaseArray() {}
};

#endif
