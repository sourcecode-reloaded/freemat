#ifndef __NumericArray_hpp__
#define __NumericArray_hpp__

#include "BasicArray.hpp"

template <typename T>
class NumericArray {
  BasicArray<T> m_real;
  BasicArray<T> m_imag;
public:
  NumericArray(const NTuple &dims) : m_real(dims) {}
  NumericArray(NumericArray<T> &ref, const RangeSet& slice) :
    m_real(ref.real(),slice), m_imag(ref.imag(),slice) { }
  inline const NTuple dimensions() const {
    return m_real.dimensions();
  }
  inline BasicArray<T>& real() {
    return m_real;
  }
  inline const BasicArray<T>& real() const {
    return m_real;
  }
  inline BasicArray<T>& imag() {
    return m_imag;
  }
  inline const BasicArray<T>& imag() const {
    return m_imag;
  }
};

#endif
