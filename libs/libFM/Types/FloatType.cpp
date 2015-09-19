#include "FloatType.hpp"
#include "TermIF.hpp"
#include "Complex.hpp"
#include "ArrayFormatInfo.hpp"
#include "ThreadContext.hpp"
#include <math.h>

using namespace FM;

template <typename T>
bool FloatType<T>::isSymmetric(const Object &a) {
  if (!a.isSquare()) throw Exception("Cannot test non-square matrices for symmetry");
  ndx_t N = a.rows();
  const T* ap = this->ro(a);
  for (ndx_t i=0;i<N;i++)
    for (ndx_t j=0;j<i;j++)
      if (ap[i*N+j] != complex_conj(ap[j*N+i])) return false;
  return true;
}

template class FM::FloatType<float>;
template class FM::FloatType<double>;
template class FM::FloatType<Complex<float> >;
template class FM::FloatType<Complex<double> >;
