#ifndef __CommonIterator_hpp__
#define __CommonIterator_hpp__

#include "NTuple.hpp"
#include <QtGlobal>

template <typename S, typename T>
class BaseIterator {
  S *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
  index_t m_stride;
public:
  BaseIterator(S *ptr, int dim) : 
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_dim.map(m_pos);
    m_stride = m_dim.stride(m_pinDim);
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_dim.map(m_pos);
  }
  inline void next() {m_ndx += m_stride;}
  inline void prev() {m_ndx -= m_stride;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline void set(const T& val) {m_ptr->set(m_ndx,val);}
  inline const T get() const {return m_ptr->get(m_ndx);}
  ~BaseIterator() {}
};

template <typename S, typename T>
class ConstBaseIterator {
  const S *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
  index_t m_stride;
public:
  ConstBaseIterator(const S *ptr, int dim) : 
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_dim.map(m_pos);
    m_stride = m_dim.stride(m_pinDim);
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_dim.map(m_pos);
  }
  inline void next() {m_ndx += m_stride;}
  inline void prev() {m_ndx -= m_stride;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline const T get() const {return m_ptr->get(m_ndx);}
  ~ConstBaseIterator() {}
};

template <typename S, typename T>
class Transformer {
  BaseIterator<S,T> m_dest;
  ConstBaseIterator<S,T> m_source;
public:
  inline Transformer(S* dest, const S* src, int destdim = 0, int srcdim = 0)
    : m_dest(dest,destdim), m_source(src,srcdim) {}
  inline index_t size() const {return qMin(m_source.size(),m_dest.size());}
  inline bool isValid() const {return (m_dest.isValid() && m_source.isValid());}
  inline void next() {m_dest.next(); m_source.next();}
  inline void nextSlice() {m_dest.nextSlice(); m_source.nextSlice();}
  inline const T get() const {return m_source.get();}
  inline void set(const T& val) {m_dest.set(val);}
};

#endif



