#include "Hermitian.hpp"
#include "Transpose.hpp"
#include "PODType.hpp"
#include "SingleType.hpp"
#include "Complex.hpp"

using namespace FM;

const int BLOCKSIZE = 100; // Optimize?

template <class T, int block>
static void blocked_hermitian(const Complex<T> *A, Complex<T> *B, int N, int M)
{
  for (int i=0;i<N;i+=block)
    for (int j=0;j<M;j+=block)
      for (int k=0;k<block;k++)
	for (int n=0;n<block;n++)
	  if (((j+n) < M) && ((i+k) < N))
	    B[(j+n)+M*(i+k)] = complex_conj(A[(i+k)+N*(j+n)]);
}

template <class ElementType>
Object FM::MatrixHermitian(const Object &a) {
  if (a.isScalar()) return a;
  if (!a.is2D()) throw Exception("Cannot transpose multidimensional arrays");
  if (!a.isComplex()) return MatrixTranspose<ElementType>(a);
  PODType<ElementType> *type = a.asType<PODType<ElementType> >();
  Object ret = type->makeMatrix(a.cols(),a.rows(),a.isComplex());
  blocked_hermitian<ElementType,BLOCKSIZE>(type->readOnlyDataComplex(a),type->readWriteDataComplex(ret),
					   a.rows(),a.cols());
  return ret;
}

template Object FM::MatrixHermitian<double>(const Object &a);
template Object FM::MatrixHermitian<bool>(const Object &a);
template Object FM::MatrixHermitian<char>(const Object &a);
template Object FM::MatrixHermitian<float>(const Object &a);
template Object FM::MatrixHermitian<uint8_t>(const Object &a);
template Object FM::MatrixHermitian<int8_t>(const Object &a);
template Object FM::MatrixHermitian<uint16_t>(const Object &a);
template Object FM::MatrixHermitian<int16_t>(const Object &a);
template Object FM::MatrixHermitian<uint32_t>(const Object &a);
template Object FM::MatrixHermitian<int32_t>(const Object &a);
template Object FM::MatrixHermitian<uint64_t>(const Object &a);
template Object FM::MatrixHermitian<int64_t>(const Object &a);
