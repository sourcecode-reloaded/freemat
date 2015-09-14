#include "Hermitian.hpp"
#include "Transpose.hpp"
#include "PODType.hpp"
#include "SingleType.hpp"
#include "Complex.hpp"

using namespace FM;

const int BLOCKSIZE = 100; // Optimize?

template <class T, int block>
static void blocked_hermitian(const T *A, T *B, ndx_t N, ndx_t M)
{
  for (ndx_t i=0;i<N;i+=block)
    for (ndx_t j=0;j<M;j+=block)
      for (ndx_t k=0;k<block;k++)
	for (int n=0;n<block;n++)
	  if (((j+n) < M) && ((i+k) < N))
	    B[(j+n)+M*(i+k)] = complex_conj(A[(i+k)+N*(j+n)]);
}

template <class ElementType>
Object FM::MatrixHermitian(const Object &a) {
  if (a.isScalar()) return a;
  if (!a.is2D()) throw Exception("Cannot transpose multidimensional arrays");
  ArrayType<ElementType> *type = a.asType<ArrayType<ElementType> >();
  Object ret = type->makeMatrix(a.cols(),a.rows());
  blocked_hermitian<ElementType,BLOCKSIZE>(type->ro(a),type->rw(ret),a.rows(),a.cols());
  return ret;
}

template Object FM::MatrixHermitian<char>(const Object &a);
template Object FM::MatrixHermitian<double>(const Object &a);
template Object FM::MatrixHermitian<float>(const Object &a);
template Object FM::MatrixHermitian<int8_t>(const Object &a);
template Object FM::MatrixHermitian<int16_t>(const Object &a);
template Object FM::MatrixHermitian<int32_t>(const Object &a);
template Object FM::MatrixHermitian<int64_t>(const Object &a);
template Object FM::MatrixHermitian<Complex<double> >(const Object &a);
template Object FM::MatrixHermitian<Complex<float> >(const Object &a);
template Object FM::MatrixHermitian<Complex<int8_t> >(const Object &a);
template Object FM::MatrixHermitian<Complex<int16_t> >(const Object &a);
template Object FM::MatrixHermitian<Complex<int32_t> >(const Object &a);
template Object FM::MatrixHermitian<Complex<int64_t> >(const Object &a);
