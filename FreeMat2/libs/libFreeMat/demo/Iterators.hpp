#ifndef __Iterators_hpp__
#define __Iterators_hpp__

#include "NTuple.hpp"
#include "BaseArray.hpp"

template <typename S>
class NDimIterator {
  BaseArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
public:
  NDimIterator(BaseArray<S> *ptr, int dim) :
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim), 
    m_dim(ptr->dimensions()) {}
  inline void nextSlice() {m_dim.increment(m_pos,m_pinDim);}
  inline const S& operator[](index_t ndx) const {
    NTuple out(m_pos);
    out[m_pinDim] = ndx;
    return (*m_ptr)[out];
  }
  inline S& operator[](index_t ndx) {
    NTuple out(m_pos);
    out[m_pinDim] = ndx;
    return (*m_ptr)[out];
  }
  inline const S& at(index_t ndx) const {
    NTuple out(m_pos);
    out[m_pinDim] = ndx;
    return (*m_ptr)[out];
  }
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
};

template <typename S>
class ConstNDimIterator {
  const BaseArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
public:
  ConstNDimIterator(const BaseArray<S> *ptr, int dim) :
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim), 
    m_dim(ptr->dimensions()) {}
  inline void nextSlice() {m_dim.increment(m_pos,m_pinDim);}
  inline const S& operator[](index_t ndx) const {
    NTuple out(m_pos);
    out[m_pinDim] = ndx;
    return (*m_ptr)[out];
  }
  inline const S& at(index_t ndx) const {
    NTuple out(m_pos);
    out[m_pinDim] = ndx;
    return (*m_ptr)[out];
  }
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
};

#endif
