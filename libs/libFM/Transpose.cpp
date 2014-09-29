#include "Transpose.hpp"
#include "PODType.hpp"
#include "SingleType.hpp"
#include "Complex.hpp"

using namespace FM;

const int BLOCKSIZE = 100; // Optimize?

template <class T, int block>
static void blocked_transpose(const T *A, T *B, int N, int M)
{
  for (int i=0;i<N;i+=block)
    for (int j=0;j<M;j+=block)
      for (int k=0;k<block;k++)
	for (int n=0;n<block;n++)
	  if (((j+n) < M) && ((i+k) < N))
	    B[(j+n)+M*(i+k)] = A[(i+k)+N*(j+n)];
}

template <class T, int block>
static void blocked_transpose_complex(const T *A, T *B, int N, int M)
{
  for (int i=0;i<N;i+=block)
    for (int j=0;j<M;j+=block)
      for (int k=0;k<block;k++)
	for (int n=0;n<block;n++)
	  if (((j+n) < M) && ((i+k) < N))
	    {
	      B[2*((j+n)+M*(i+k))] = A[2*((i+k)+N*(j+n))];
	      B[2*((j+n)+M*(i+k))+1] = A[2*((i+k)+N*(j+n))+1];
	    }
}


template <class ElementType>
Object FM::MatrixTranspose(const Object &a) {
  if (a.isScalar()) return a;
  if (!a.is2D()) throw Exception("Cannot transpose multidimensional arrays");
  ArrayType<ElementType> *type = a.asType<ArrayType<ElementType> >();
  Object ret = type->makeMatrix(a.cols(),a.rows(),a.isComplex());
  if (a.isComplex())
    blocked_transpose_complex<ElementType,BLOCKSIZE>(type->readOnlyData(a),type->readWriteData(ret),a.rows(),a.cols());
  else
    blocked_transpose<ElementType,BLOCKSIZE>(type->readOnlyData(a),type->readWriteData(ret),a.rows(),a.cols());
  return ret;
}

template Object FM::MatrixTranspose<Object>(const Object &a);
template Object FM::MatrixTranspose<double>(const Object &a);
template Object FM::MatrixTranspose<bool>(const Object &a);
template Object FM::MatrixTranspose<char>(const Object &a);
template Object FM::MatrixTranspose<float>(const Object &a);
template Object FM::MatrixTranspose<uint8_t>(const Object &a);
template Object FM::MatrixTranspose<int8_t>(const Object &a);
template Object FM::MatrixTranspose<uint16_t>(const Object &a);
template Object FM::MatrixTranspose<int16_t>(const Object &a);
template Object FM::MatrixTranspose<uint32_t>(const Object &a);
template Object FM::MatrixTranspose<int32_t>(const Object &a);
template Object FM::MatrixTranspose<uint64_t>(const Object &a);
template Object FM::MatrixTranspose<int64_t>(const Object &a);
