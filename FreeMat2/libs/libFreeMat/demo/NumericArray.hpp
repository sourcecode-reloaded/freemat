#ifndef __NumericArray_hpp__
#define __NumericArray_hpp__

#include "NDimArray.hpp"

template <typename T>
class NumericArray {
  NDimArray<T> m_real;
  NDimArray<T> m_imag;
public:
  NumericArray(const NTuple &dims) : m_real(dims) {}
  NumericArray(NumericArray<T> &ref, const RangeSet& slice) :
    m_real(ref.real(),slice), m_imag(ref.imag(),slice) { }
  inline const NTuple dimensions() const {
    return m_real.dimensions();
  }
  inline NDimArray<T>& real() {
    return m_real;
  }
  inline const NDimArray<T>& real() const {
    return m_real;
  }
  inline NDimArray<T>& imag() {
    return m_imag;
  }
  inline const NDimArray<T>& imag() const {
    return m_imag;
  }
};

#endif
