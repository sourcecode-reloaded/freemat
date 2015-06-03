#include "MatrixInvert.hpp"
#include "FloatType.hpp"
#include "MemPtr.hpp"
#include "LAPACK.hpp"
#include "SingleType.hpp"
#include "DoubleType.hpp"
#include <cmath>

using namespace FM;

template <typename T, FM::DataCode codeNum>
static Object TInvertMatrix(const Object &A,
			    void (*getrf)(int*,int*,T*,int*,int*,int*),
			    void (*getri)(int*,T*,int*,int*,T*,int*,int*),
			    FloatType<T,codeNum> *ft) {
  int M = int(A.rows());
  int N = int(A.columns());
  Object Acopy(A);
  int LDA = M;
  MemBlock<int> Ipiv(qMin(M,N));
  int INFO;
  // Do the decomposition
  getrf(&M,&N,ft->rw(Acopy),&LDA,&Ipiv,&INFO);
  // Compute the inverse
  T WORKSIZE;
  int IWORKSIZE = -1;
  getri(&N,ft->rw(Acopy),&LDA,&Ipiv,&WORKSIZE,&IWORKSIZE,&INFO);
  IWORKSIZE = int(WORKSIZE);
  if (A.isComplex()) IWORKSIZE *= 2;
  MemBlock<T> work(IWORKSIZE);
  getri(&N,ft->rw(Acopy),&LDA,&Ipiv,&work,&IWORKSIZE,&INFO);
  if (INFO > 0) {
    if (A.isComplex())
      ft->fillComplex(Acopy,Complex<T>(std::numeric_limits<T>::infinity(),0));
    else
		ft->fill(Acopy, std::numeric_limits<T>::infinity());
  }
  return Acopy;
}

Object FM::MatrixInvert(const Object &a, ThreadContext *ctxt) {
  if (!a.is2D()) throw Exception("Cannot invert N-dimensional arrays.");
  if (!a.isSquare()) throw Exception("Cannot invert non-square matrices.");
  if (a.isEmpty()) return a;
  switch (a.typeCode()) {
  default: throw Exception("Invert does not support this data class");
  case TypeSingle:
    if (a.isComplex())
      return TInvertMatrix<float,TypeSingle>(a,cgetrf_,cgetri_,ctxt->_single);
    else
      return TInvertMatrix<float,TypeSingle>(a,sgetrf_,sgetri_,ctxt->_single);
  case TypeDouble:
    if (a.isComplex())
      return TInvertMatrix<double,TypeDouble>(a,zgetrf_,zgetri_,ctxt->_double);
    else
      return TInvertMatrix<double,TypeDouble>(a,dgetrf_,dgetri_,ctxt->_double);
  }
}
