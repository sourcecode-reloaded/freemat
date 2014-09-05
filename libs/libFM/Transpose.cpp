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

template <class ElementType, bool _objectType>
Object FM::MatrixTranspose(const Object &a) {
  if (a.isScalar()) return a;
  if (!a.is2D()) throw Exception("Cannot transpose multidimensional arrays");
  PODType<ElementType,_objectType> *type = a.asType<PODType<ElementType,_objectType> >();
  Object ret = type->makeMatrix(a.cols(),a.rows(),a.isComplex());
  if (!_objectType && a.isComplex())
    blocked_transpose<Complex<ElementType>,BLOCKSIZE>(type->readOnlyDataComplex(a),type->readWriteDataComplex(ret),
						      a.rows(),a.cols());
  else
    blocked_transpose<ElementType,BLOCKSIZE>(type->readOnlyData(a),type->readWriteData(ret),a.rows(),a.cols());
  return ret;
}

template Object FM::MatrixTranspose<Object,true>(const Object &a);
template Object FM::MatrixTranspose<double,false>(const Object &a);
template Object FM::MatrixTranspose<bool,false>(const Object &a);
template Object FM::MatrixTranspose<char,false>(const Object &a);
template Object FM::MatrixTranspose<float,false>(const Object &a);
template Object FM::MatrixTranspose<uint8_t,false>(const Object &a);
template Object FM::MatrixTranspose<int8_t,false>(const Object &a);
template Object FM::MatrixTranspose<uint16_t,false>(const Object &a);
template Object FM::MatrixTranspose<int16_t,false>(const Object &a);
template Object FM::MatrixTranspose<uint32_t,false>(const Object &a);
template Object FM::MatrixTranspose<int32_t,false>(const Object &a);
template Object FM::MatrixTranspose<uint64_t,false>(const Object &a);
template Object FM::MatrixTranspose<int64_t,false>(const Object &a);
