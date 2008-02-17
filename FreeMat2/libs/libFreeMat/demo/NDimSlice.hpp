#ifndef __NDimSlice_hpp__
#define __NDimSlice_hpp__

#include "BaseArray.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

template <typename T>
class NDimSlice : public BaseArray<T> {
  BaseArray<T> &m_ref;
  const RangeSet &m_slices;
  NTuple m_dims;
public:
  NDimSlice(BaseArray<T> &ref, const RangeSet& slice) : 
    m_ref(ref), m_slices(slice) { 
    m_dims = m_slices.dims();
  }
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      NTuple tpos;
      m_slices.remap(pos,tpos);
      if (m_ref.dimensions().validate(tpos))
	return m_ref[tpos];
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      NTuple tpos;
      m_slices.remap(pos,tpos);
      return m_ref[tpos];
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
