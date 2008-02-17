#ifndef __PermArray_hpp__
#define __PermArray_hpp__

#include "BaseArray.hpp"

template <typename T>
class PermArray : public BaseArray<T> {
  BaseArray<T> &m_ref;
  NPerm m_perm;
  NTuple m_dims;
public:
  PermArray(BaseArray<T> &ref, const NPerm &perm) :
    m_ref(ref), m_perm(perm), m_dims(ref.dimensions()[m_perm]) {}
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      return(m_ref[pos[m_perm]]);
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      return(m_ref[pos[m_perm]]);
    }
    throw Exception("Out of range");
  }
  inline const T operator[](index_t p) const {
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
