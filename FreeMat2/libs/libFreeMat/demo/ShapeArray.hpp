#ifndef __ShapeArray_hpp__
#define __ShapeArray_hpp__

#include "BaseArray.hpp"

template <typename T>
class ShapeArray : public BaseArray<T> {
  BaseArray<T> &m_ref;
  NTuple m_dims;
public:
  ShapeArray(BaseArray<T> &ref, const NTuple &dims) :
    m_ref(ref), m_dims(dims) {
    if (m_dims.count() != dims.count())
      throw Exception("Illegal shape operation");
  }
  inline const NTuple& dimensions() const {
    return m_dims;
  }
  inline const T& operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      return(m_ref[m_dims.map(pos)]);
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      return(m_ref[m_dims.map(pos)]);
    }
    throw Exception("Out of range");
  }
  inline const T& operator[](index_t p) const {
    NTuple tpos;
    m_dims.map(p,tpos);
    return (*this)[tpos];
  }
  inline T& operator[](index_t p) {
    NTuple tpos;
    m_dims.map(p,tpos);
    return (*this)[tpos];
  }  
};

#endif
