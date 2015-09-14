
#include "GenEigenDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "MemPtr.hpp"
#include "Complex.hpp"
#include "FloatType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

template <typename T>
void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	   T *B, int *LDB, T *ALPHAR, T *ALPHAI,
	   T *BETA, T *VL, int *LDVL, T *VR, 
	   int *LDVR, T *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 );

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	   float *B, int *LDB, float *ALPHAR, float *ALPHAI,
	   float *BETA, float *VL, int *LDVL, float *VR, 
	   int *LDVR, float *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return sggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO,l1,l2);
}

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	   double *B, int *LDB, double *ALPHAR, double *ALPHAI,
	   double *BETA, double *VL, int *LDVL, double *VR, 
	   int *LDVR, double *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return dggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO,l1,l2);
}

template <typename T>
void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	   T *B, int *LDB, T *ALPHA, T *BETA, 
	   T *VL, int *LDVL, T *VR, int *LDVR, 
	   T *WORK, int *LWORK, T *RWORK, int *INFO, ftnlen l1, ftnlen l2 );

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	   float *B, int *LDB, float *ALPHA, float *BETA, 
	   float *VL, int *LDVL, float *VR, int *LDVR, 
	   float *WORK, int *LWORK, float *RWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return cggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	   double *B, int *LDB, double *ALPHA, double *BETA, 
	   double *VL, int *LDVL, double *VR, int *LDVR, 
	   double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return zggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO,l1,l2);  
}

template <typename T>
static void realGenEigenDecompose(int n, T *v, T *d, T *a,
				  T *b, bool eigenvectors) {
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  MemBlock<T> ALPHAR(n);
  MemBlock<T> ALPHAI(n);
  MemBlock<T> BETA(n);
  T *VL = nullptr;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  T WORKSZE;
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORKSZE, &LWORK, &INFO,1,1 );
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &INFO,1,1 );
  int i;
  for (i=0;i<n;i++) {
    d[2*i] = ALPHAR[i]/BETA[i];
    d[2*i+1] = ALPHAI[i]/BETA[i];
  }
}

template <typename T>
static void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
		  int *LWORK, int *INFO , ftnlen l1, ftnlen l2);

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	   int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return ssygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO,l1,l2);
}

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	   int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return dsygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO,l1,l2);
}


template <typename T>  
static bool realGenEigenDecomposeSymmetric(int n, T *v, T *d,
					   T *a, T *b, bool eigenvectors) {
  int ITYPE = 1;
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  T *W = d;
  T WORKSIZE;
  int LWORK = -1;
  int INFO;
  Tsygv( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORKSIZE, 
	 &LWORK, &INFO,1,1 );
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tsygv( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORK, 
	 &LWORK, &INFO,1,1 );
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(T));
  return true;
}
template <typename T>
static void complexGenEigenDecompose(int n, T *v, T *d, T *a, T *b, 
				     bool eigenvectors) {
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = N;
  MemBlock<T> ALPHA(2*n);
  MemBlock<T> BETA(2*n);
  T *VL = nullptr;
  int LDVL = n;
  T *VR = v;
  int LDVR = n;
  MemBlock<T> RWORK(8*n);
  T WORKSIZE[2];
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORKSIZE[0], &LWORK, &RWORK, &INFO,1,1 );
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK*2);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &RWORK, &INFO,1,1 );
  int i;
  for (i=0;i<n;i++) 
    complex_divide<T>(ALPHA[2*i],ALPHA[2*i+1],
		      BETA[2*i],BETA[2*i+1],
		      d[2*i],d[2*i+1]);
}

void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, Complex<float> *A, 
	   int *LDA, Complex<float> *B, int *LDB, float *W, Complex<float> *WORK,
	   int *LWORK, float *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return chegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, Complex<double> *A, 
	   int *LDA, Complex<double> *B, int *LDB, double *W, Complex<double> *WORK,
	   int *LWORK, double *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return zhegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
}


template <typename T>
static bool complexGenEigenDecomposeSymmetric(int n, T *v, T *d, T *a, T *b, 
					      bool eigenvectors) {
  int ITYPE = 1;
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  T *W = d;
  MemBlock<T> RWORK(MAX(1,3*N-2));
  int INFO;
  int LWORK;
  LWORK = MAX(1,2*N-1);
  MemBlock<T> WORK(2*LWORK);
  Thegv(&ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORK,
	&LWORK, &RWORK, &INFO,1,1 );    
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,2*n*n*sizeof(T));
  return true;

}
template <typename T, FM::DataCode codeNum>
static void HandleEigenVectorsRealMatrix(const Object &eigenvals, const Object &Vp, 
					 ndx_t N, Object &D, Object &V, FloatType<T,codeNum> *ft) {
  bool complexEigenvalues = ft->anyNonzeroImaginary(eigenvals);
  if (!complexEigenvalues) {
    D = ft->asDiagonalMatrix(ft->realPart(eigenvals));
    V = Vp;
  } else {
    D = ft->asDiagonalMatrix(eigenvals);
    V = ft->makeMatrixComplex(N,N);
    ndx_t i = 0;
    const Complex<T>* ep = ft->roComplex(eigenvals);
    const T* vp = ft->ro(Vp);
    Complex<T>* op = ft->rwComplex(V);
    while (i < N) {
      if (ep[i].i != 0) {
	for (ndx_t j=0;j<N;j++) {
	  op[j+i*N] = Complex<T>(vp[j+i*N],vp[j+(i+1)*N]);
	  op[j+(i+1)*N] = Complex<T>(vp[j+i*N],-vp[j+(i+1)*N]);
	}
	i += 2;
      } else {
	for (ndx_t j=0;j<N;j++)
	  op[j+i*N] = Complex<T>(vp[j+i*N],0);
	i++;
      }
    }
  }
}

template <typename T, FM::DataCode codeNum>
static void TEigenDecomposeFullGeneral(Object A, Object &V, Object &D,
				      bool balanceFlag, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  if (!A.isComplex()) {
    Object eigenvals = ft->makeMatrixComplex(N,1);
    // A temporary vector to store the eigenvalues
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    Object Vp = ft->makeMatrix(N,N);
    realEigenDecompose(int(N), ft->rw(Vp), ft->rw(eigenvals), 
		       ft->rw(A), true, balanceFlag);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V, ft);
  } else {
    Object eigenvals = ft->makeMatrixComplex(N,1);
    V = ft->makeMatrixComplex(N,N);
    complexEigenDecompose(int(N), ft->rw(V), ft->rw(eigenvals), 
			  ft->rw(A), true, balanceFlag);
    D = ft->asDiagonalMatrix(eigenvals);
  }
}

void FM::EigenDecomposeFullGeneral(const Object &A, Object& V, Object& D, bool balanceFlag, ThreadContext *ctxt) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.typeCode()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle: 
    TEigenDecomposeFullGeneral<float>(A,V,D,balanceFlag,ctxt->_single);
    return;
  case TypeDouble:
    TEigenDecomposeFullGeneral<double>(A,V,D,balanceFlag,ctxt->_double);
    return;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T, FM::DataCode codeNum>
static void TEigenDecomposeCompactGeneral(Object A, Object& D, 
					  bool balanceFlag, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  if (!A.isComplex()) {
    D = ft->makeMatrixComplex(N,1);
    realEigenDecompose(int(N), (T*) nullptr, ft->rw(D), 
		       ft->rw(A), false, balanceFlag);
    if (!ft->anyNonzeroImaginary(D))
      D = ft->realPart(D);
  } else {
    D = ft->makeMatrixComplex(N,1);
    complexEigenDecompose(int(N), (T*) nullptr, ft->rw(D),
			  ft->rw(A), false, balanceFlag);
  }
}

void FM::EigenDecomposeCompactGeneral(Object A, Object& D, bool balanceFlag, ThreadContext *ctxt) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  switch (A.typeCode()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle:
    TEigenDecomposeCompactGeneral<float>(A,D,balanceFlag,ctxt->_single);
    return;
  case TypeDouble:
    TEigenDecomposeCompactGeneral<double>(A,D,balanceFlag,ctxt->_double);
    return;
  }
}

template <typename T, FM::DataCode codeNum>
static bool TGeneralizedEigenDecomposeCompactSymmetric(Object A, Object B, 
						       Object& D, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  if (!A.isComplex()) {
    D = ft->makeMatrix(N,1);
    if (!realGenEigenDecomposeSymmetric(int(N), (T*) nullptr, ft->rw(D), 
					ft->rw(A), ft->rw(B),false)) 
      return false;
  } else {
    D = ft->makeMatrix(N,1);
    if (!complexGenEigenDecomposeSymmetric(int(N), (T*) nullptr, ft->rw(D), 
					   ft->rw(A), ft->rw(B), false)) 
      return false;
  }
  return true;
}

bool GeneralizedEigenDecomposeCompactSymmetric(Object A, Object B, Object& D, ThreadContext *ctxt) {
  switch (A.typeCode()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle:
    return TGeneralizedEigenDecomposeCompactSymmetric<float>(A,B,D,ctxt->_single);
  case TypeDouble:
    return TGeneralizedEigenDecomposeCompactSymmetric<double>(A,B,D,ctxt->_double);
  }
  return false;
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */

template <typename T, FM::DataCode codeNum>
static bool TGeneralizedEigenDecomposeFullSymmetric(Object A, Object B, Object &V, Object &D,
						    FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  Object eigenvals = ft->makeMatrix(N,1);
  if (!A.isComplex()) {
      // A temporary vector to store the eigenvalues
    V = ft->makeMatrix(N,N);
    if (!realGenEigenDecomposeSymmetric(int(N), ft->rw(V), ft->rw(eigenvals), 
					ft->rw(A), ft->rw(B), true))
      return false;
  } else {
    V = ft->makeMatrixComplex(N,N);
    if (!complexGenEigenDecomposeSymmetric(int(N), ft->rw(V), ft->rw(eigenvals),
					   ft->rw(A), ft->rw(B), true))
      return false;
  }
  D = ft->asDiagonalMatrix(eigenvals);
  return true;
}

bool GeneralizedEigenDecomposeFullSymmetric(Object A, Object B, Object& V, Object& D, ThreadContext *ctxt) {
  switch (A.typeCode()) {
  default:
    throw Exception("Unsupported type for eigendecomposition");
  case TypeSingle:
    return TGeneralizedEigenDecomposeFullSymmetric<float>(A,B,V,D,ctxt->_single);
  case TypeDouble:
    return TGeneralizedEigenDecomposeFullSymmetric<double>(A,B,V,D,ctxt->_double);
  }
  return false;
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <typename T, FM::DataCode codeNum>
static void TGeneralizedEigenDecomposeFullGeneral(Object A, Object B, Object &V, Object &D,
						 FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  // A temporary vector to store the eigenvalues
  Object eigenvals = ft->makeMatrixComplex(N,1);
  if (!A.isComplex()) {
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    Object Vp = ft->makeMatrix(N,N);
    realGenEigenDecompose(int(N), ft->rw(Vp), ft->rw(eigenvals), 
			  ft->rw(A),ft->rw(B),true);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V, ft);
  } else {
    V = ft->makeMatrixComplex(N,N);
    complexGenEigenDecompose(int(N), ft->rw(V), ft->rw(eigenvals), 
			     ft->rw(A), ft->rw(B), true);
    D = ft->asDiagonalMatrix(eigenvals);
  }
}

void GeneralizedEigenDecomposeFullGeneral(Object A, Object B, Object& V, Object& D, ThreadContext *ctxt) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.typeCode()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle:
    return TGeneralizedEigenDecomposeFullGeneral<float>(A,B,V,D,ctxt->_single);
  case TypeDouble:
    return TGeneralizedEigenDecomposeFullGeneral<double>(A,B,V,D,ctxt->_double);
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T, FM::DataCode codeNum>
static void TGeneralizedEigenDecomposeCompactGeneral(Object A, Object B, Object& D, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  D = ft->makeMatrixComplex(N,1);
  if (!A.isComplex()) {
    realGenEigenDecompose(int(N), (T*) nullptr, ft->rw(D), ft->rw(A), ft->rw(B), false);
    if (!ft->anyNonzeroImaginary(D)) D = ft->realPart(D);
  } else 
    complexGenEigenDecompose(int(N), (T*) nullptr, ft->rw(D), ft->rw(A), ft->rw(B), false);
}

void GeneralizedEigenDecomposeCompactGeneral(Object A, Object B, Object& D, ThreadContext *ctxt) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.typeCode()) {
  default: 
    throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle: 
    TGeneralizedEigenDecomposeCompactGeneral<float>(A,B,D,ctxt->_single);
    break;
  case TypeDouble:
    TGeneralizedEigenDecomposeCompactGeneral<double>(A,B,D,ctxt->_double);
    break;
  }
}

