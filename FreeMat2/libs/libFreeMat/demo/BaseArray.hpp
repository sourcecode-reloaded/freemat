#ifndef __BaseArray_hpp__
#define __BaseArray_hpp__

#include "NTuple.hpp"

template <typename T>
class NDimIterator;

template <typename T>
class ConstNDimIterator;

template <typename T>
class BaseArray {
public:
  virtual const NTuple dimensions() const = 0;
  virtual const T operator[](const NTuple& pos) const = 0;
  virtual T& operator[](const NTuple& pos) = 0;
  virtual const T operator[](index_t pos) const = 0;
  virtual T& operator[](index_t pos) = 0;
  virtual NDimIterator<T>* getNDimIterator(int dim)  {
    return new NDimIterator<T>(this,dim);
  }
  virtual ConstNDimIterator<T>* getConstNDimIterator(int dim) {
    return new ConstNDimIterator<T>(this,dim);
  }
  virtual ~BaseArray() {}
};

#endif
