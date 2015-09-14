#include "MatrixOps.hpp"
#include "BLAS.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include "ThreadContext.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"
#include "DenseSolver.hpp"
#include <cmath>

namespace FM {

  template <class T>
  static Object MatMul(const Object &a, const Object &b) {
    FloatType<T> *dt = a.asType<FloatType<T> >();
    Object ret = dt->makeMatrix(a.rows(),b.cols());
    BLAS_gemm(int(a.rows()),int(a.cols()),int(b.cols()),dt->ro(a),dt->ro(b),dt->rw(ret));
    return ret;
  }
  
  Object MatrixMultiply(const Object &a, const Object &b, ThreadContext *ctxt) {
    if (a.cols() != b.rows()) throw Exception("Matrix multiplication requires arguments to be conformant");
    if (a.typeCode() != b.typeCode()) {
      Object acopy(a);
      Object bcopy(b);
      MatrixPromoteTypes(acopy,bcopy,ctxt);
      return MatrixMultiply(acopy,bcopy,ctxt);
    }
    switch (a.typeCode()) {
    case TypeDouble:
      return MatMul<double>(a,b);
    case TypeSingle:
      return MatMul<float>(a,b);
    case TypeZDouble:
      return MatMul<Complex<double> >(a,b);
    case TypeZSingle:
      return MatMul<Complex<float> >(a,b);
    default:
      throw Exception("Unsupported type " + a.type()->name() + " for matrix multiplication");
    }
  }
  
  template <typename T, FM::DataCode codeNum>
  static Object TInvertMatrix(const Object &A,
			      void (*getrf)(int*,int*,T*,int*,int*,int*),
			      void (*getri)(int*,T*,int*,int*,T*,int*,int*),
			      FloatType<T> *ft) {
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
    if (INFO > 0) 
      ft->fill(Acopy, std::numeric_limits<T>::infinity());
    return Acopy;
  }
  
  Object MatrixInvert(const Object &a, ThreadContext *ctxt) {
    if (!a.is2D()) throw Exception("Cannot invert N-dimensional arrays.");
    if (!a.isSquare()) throw Exception("Cannot invert non-square matrices.");
    if (a.isEmpty()) return a;
    switch (a.typeCode()) {
    default: throw Exception("Invert does not support this data class");
    case TypeSingle:
      return TInvertMatrix<float,TypeSingle>(a,sgetrf_,sgetri_,ctxt->_single);
    case TypeZSingle:
      return TInvertMatrix<Complex<float>,TypeZSingle>(a,cgetrf_,cgetri_,ctxt->_zsingle);
    case TypeDouble:
      return TInvertMatrix<double,TypeDouble>(a,dgetrf_,dgetri_,ctxt->_double);
    case TypeZDouble:
      return TInvertMatrix<Complex<double>,TypeZDouble>(a,zgetrf_,zgetri_,ctxt->_zdouble);
    }
  }

  Object MatrixLeftDivide(const Object &a, const Object &b, ThreadContext *ctxt) {
    if (a.rows() != b.rows()) throw Exception("Matrix left division (e.g., A\b) requires A and b to have the same number of rows");
    if (a.typeCode() != b.typeCode()) {
      Object acopy(a);
      Object bcopy(b);
      MatrixPromoteTypes(acopy,bcopy,ctxt);
      return MatrixLeftDivide(acopy,bcopy,ctxt);
    }
    Object ret(ctxt);
    switch (a.typeCode()) {
    case TypeDouble:
      ret = ctxt->_double->makeMatrix(a.cols(),b.cols());
      DenseSolve(a.irows(),a.icols(),b.icols(),ctxt->_double->rw(ret),ctxt->_double->ro(a),ctxt->_double->ro(b),ctxt->_io);
      break;
    case TypeSingle:
      ret = ctxt->_single->makeMatrix(a.cols(),b.cols());
      DenseSolve(a.irows(),a.icols(),b.icols(),ctxt->_single->rw(ret),ctxt->_single->ro(a),ctxt->_single->ro(b),ctxt->_io);
      break;
    case TypeZDouble:
      ret = ctxt->_zdouble->makeMatrix(a.cols(),b.cols());
      DenseSolve(a.irows(),a.icols(),b.icols(),ctxt->_zdouble->rw(ret),ctxt->_zdouble->ro(a),ctxt->_zdouble->ro(b),ctxt->_io);
      break;
    case TypeZSingle:
      ret = ctxt->_zsingle->makeMatrix(a.cols(),b.cols());
      DenseSolve(a.irows(),a.icols(),b.icols(),ctxt->_zsingle->rw(ret),ctxt->_zsingle->ro(a),ctxt->_zsingle->ro(b),ctxt->_io);
      break;
    default:
      throw Exception("Unhandled types for matrix left divide operation");
    }
    return ret;
  }

  Object MatrixRightDivide(const Object &a, const Object &b, ThreadContext *ctxt) {
    if (b.cols() != a.cols()) throw Exception("Matrix right division (e.g., a/B) requires a and B to have the same number of cols");
    Object A = a.type()->Transpose(a);
    Object B = b.type()->Transpose(b);
    Object ret = MatrixLeftDivide(B,A,ctxt);
    return ret.type()->Transpose(ret);
  }

  void MatrixPromoteTypes(Object &a, Object &b, ThreadContext *ctxt) {
    // Rule 0
    if (a.typeCode() == b.typeCode()) return;
    // Rule 1
    if ((a.typeCode() == TypeZSingle) || (b.typeCode() == TypeZSingle)) {
      a = ctxt->_zsingle->convert(a);
      b = ctxt->_zsingle->convert(b);
      return;
    }
    // Rule 2
    if (((a.typeCode() == TypeSingle) && (b.typeCode() == TypeDouble)) ||
	((a.typeCode() == TypeDouble) && (b.typeCode() == TypeSingle))) {
      a = ctxt->_single->convert(a);
      b = ctxt->_single->convert(b);
      return;
    }
    // Rule 3
    if (((a.typeCode() == TypeSingle) && (b.typeCode() == TypeZDouble)) ||
	((a.typeCode() == TypeZDouble) && (b.typeCode() == TypeSingle))) {
      a = ctxt->_zsingle->convert(a);
      b = ctxt->_zsingle->convert(b);
      return;
    }
    // Rule 4
    a = ctxt->_zdouble->convert(a);
    b = ctxt->_zdouble->convert(b);
  }
}
