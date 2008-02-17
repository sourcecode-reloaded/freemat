#ifndef __Iterators_hpp__
#define __Iterators_hpp__

#include "NTuple.hpp"
#include "BaseArray.hpp"

// A generic NDim iterator -- uses the operator[] interface
// with NTuples -- so it works with any BaseArray derived class,
// but is quite slow.  Subclass from this and implement any
// specific variants on the iterator class.
template <typename S>
class NDimIterator {
protected:
  BaseArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
public:
  NDimIterator(BaseArray<S> *ptr, int dim) :
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim), 
    m_dim(ptr->dimensions()) {}
  virtual void nextSlice() {m_dim.increment(m_pos,m_pinDim);}
  virtual void set(const S& val) {
    (*m_ptr)[m_pos] = val;
  }
  virtual const S get() const {
    return (*m_ptr)[m_pos];
  }
  virtual void advance() {m_dim[m_pinDim]++;}
  virtual bool isValid() const {return (m_pos <= m_dim);}
  virtual index_t size() const {return m_dim[m_pinDim];}
  virtual ~NDimIterator() {}
};

template <typename S>
class ConstNDimIterator {
protected:
  const BaseArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
public:
  ConstNDimIterator(const BaseArray<S> *ptr, int dim) :
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim), 
    m_dim(ptr->dimensions()) {}
  virtual void nextSlice() {m_dim.increment(m_pos,m_pinDim);}
  virtual const S get() const {
    return (*m_ptr)[m_pos];
  }
  virtual void advance() {m_dim[m_pinDim]++;}
  virtual bool isValid() const {return (m_pos <= m_dim);}
  virtual index_t size() const {return m_dim[m_pinDim];}
  virtual ~ConstNDimIterator() {}
};

#endif
