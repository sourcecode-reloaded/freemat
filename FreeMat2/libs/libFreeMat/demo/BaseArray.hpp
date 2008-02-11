#ifndef __BaseArray_hpp__
#define __BaseArray_hpp__

#include "Iterators.hpp"

template <typename T>
class BaseArray {
public:
  virtual const NTuple& dimensions() const = 0;
  virtual const T& operator[](const NTuple& pos) const = 0;
  virtual T& operator[](const NTuple& pos) = 0;
  virtual const T& operator[](index_t pos) const = 0;
  virtual T& operator[](index_t pos) = 0;
  virtual NDimIterator<T>* getNDimIterator(int dim) = 0;
  virtual ConstNDimIterator<T>* getConstNDimIterator(int dim) const = 0;
  virtual VecIterator<T>* getVecIterator() = 0;
  virtual ConstVecIterator<T>* getConstVecIterator() const = 0;
  virtual void resize(const NTuple& pos) = 0;
  virtual void resize(index_t pos) = 0;
  virtual void reshape(const NTuple& pos) = 0;
  virtual ~BaseArray() {}
};

#endif
