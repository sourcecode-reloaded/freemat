#include "DenseSolver.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include "TermIF.hpp"
#include "FMLib.hpp"

using namespace FM;

/***************************************************************************
 * Linear equation solver for real matrices
 ***************************************************************************/

void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
	    float *A, int * LDA, float * AF, int * LDAF, 
	    int * IPIV, char * EQUED, float * R, float * C, 
	    float * B, int * LDB, float * X, int * LDX, 
	    float * RCOND, float * FERR, float * BERR,
	    float * WORK, int * IWORK, int * INFO,
	    ftnlen l1, ftnlen l2, ftnlen l3) {
  return sgesvx_(FACT,TRANS,N,NRHS,A,LDA,AF,LDAF,IPIV,EQUED,R,C,B,
		 LDB,X,LDX,RCOND,FERR,BERR,WORK,IWORK,INFO,l1,l2,l3);
}

void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
	    double *A, int * LDA, double * AF, int * LDAF, 
	    int * IPIV, char * EQUED, double * R, double * C, 
	    double * B, int * LDB, double * X, int * LDX, 
	    double * RCOND, double * FERR, double * BERR,
	    double * WORK, int * IWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return dgesvx_(FACT,TRANS,N,NRHS,A,LDA,AF,LDAF,IPIV,EQUED,R,C,B,
		 LDB,X,LDX,RCOND,FERR,BERR,WORK,IWORK,INFO,l1,l2,l3);
}

void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
	    FM::Complex<float> *A, int * LDA, FM::Complex<float> * AF, int * LDAF, 
	    int * IPIV, char * EQUED, float * R, float * C, 
	    FM::Complex<float> * B, int * LDB, FM::Complex<float> * X, int * LDX, 
	    float * RCOND, float * FERR, float * BERR,
	    FM::Complex<float> * WORK, float * RWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return cgesvx_(FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV, EQUED, R, C, B,
		 LDB, X, LDX, RCOND, FERR, BERR, WORK, RWORK, INFO, l1, l2, l3);
}

void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
	    FM::Complex<double> *A, int * LDA, FM::Complex<double> * AF, int * LDAF, 
	    int * IPIV, char * EQUED, double * R, double * C, 
	    FM::Complex<double> * B, int * LDB, FM::Complex<double> * X, int * LDX, 
	    double * RCOND, double * FERR, double * BERR,
	    FM::Complex<double> * WORK, double * RWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return zgesvx_(FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV, EQUED, R, C, B,
		 LDB, X, LDX, RCOND, FERR, BERR, WORK, RWORK, INFO, l1, l2, l3);
}


template <typename T>
static void solveLinEq(int m, int n, FM::Complex<T> *c, FM::Complex<T>* a, FM::Complex<T>* b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  //      COMPLEX*16         A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   WORK( * ), X( LDX, * )
  char FACT = 'E';
  char TRANS = 'N';
  int N = m;
  int NRHS = n;
  FM::Complex<T>* A = a;
  int LDA = m;
  MemBlock<FM::Complex<T> > AF(LDA*N);
  int LDAF = m;
  MemBlock<int> IPIV(N);
  char EQUED;
  MemBlock<T> R(N);
  MemBlock<T> C(N);
  FM::Complex<T> *B = b;
  int LDB = m;
  FM::Complex<T> *X = c;
  int LDX = m;
  T RCOND;
  MemBlock<T> FERR(n);
  MemBlock<T> BERR(n);
  MemBlock<FM::Complex<T> > WORK(2*N);
  MemBlock<T> RWORK(2*N);
  int INFO;
  Tgesvx(&FACT, &TRANS, &N, &NRHS, A, &LDA, &AF, &LDAF, &IPIV, &EQUED, &R, &C, B,
	 &LDB, X, &LDX, &RCOND, &FERR, &BERR, &WORK, &RWORK, &INFO, 1, 1, 1);
  if ((INFO == N) || (INFO == N+1) || (RCOND < lamch<T>())) {
    io->warning(FMString("Matrix is singular to working precision.  RCOND = ") + Stringify(RCOND));
  }
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
template <typename T>
static void solveLinEq(int m, int n, T *c, T* a, T *b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  char FACT = 'E';
  char TRANS = 'N';
  int N = m;
  int NRHS = n;
  T* A = a;
  int LDA = m;
  MemBlock<T> AF(LDA*N);
  int LDAF = m;
  MemBlock<int> IPIV(N);
  char EQUED;
  MemBlock<T> R(N);
  MemBlock<T> C(N);
  T *B = b;
  int LDB = m;
  T *X = c;
  int LDX = m;
  T RCOND;
  MemBlock<T> FERR(n);
  MemBlock<T> BERR(n);
  MemBlock<T> WORK(4*N);
  MemBlock<int> IWORK(4*N);
  int INFO;
  Tgesvx(&FACT, &TRANS, &N, &NRHS, A, &LDA, &AF, &LDAF, &IPIV, &EQUED, &R, &C, B,
	 &LDB, X, &LDX, &RCOND, &FERR, &BERR, &WORK, &IWORK, &INFO,1,1,1);
  if ((INFO == N) || (INFO == N+1) || (RCOND < lamch<T>()))
    io->warning(FMString("Matrix is singular to working precision.  RCOND = ") + Stringify(RCOND));
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are complex.


void Tgelsy(int* M, int *N, int *NRHS, double* A, int *LDA,
	    double *B, int *LDB, int *JPVT, double* RCOND,
	    int *RANK, double *WORK, int* LWORK, int* INFO) {
  return dgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);
}

void Tgelsy(int* M, int *N, int *NRHS, float* A, int *LDA,
	    float *B, int *LDB, int *JPVT, float* RCOND,
	    int *RANK, float *WORK, int* LWORK, int* INFO) {
  return sgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);  
}

void Tgelsy(int* M, int *N, int *NRHS, FM::Complex<float*> A, int *LDA,
	    FM::Complex<float> *B, int *LDB, int *JPVT, float* RCOND,
	    int *RANK, FM::Complex<float> *WORK, int* LWORK, float* RWORK,
	    int* INFO) {
  return cgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,RWORK,INFO);
}

void Tgelsy(int* M, int *N, int *NRHS, FM::Complex<double*> A, int *LDA,
	    FM::Complex<double> *B, int *LDB, int *JPVT, double* RCOND,
	    int *RANK, FM::Complex<double> *WORK, int* LWORK, double* RWORK,
	    int* INFO) {
  return zgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,RWORK,INFO);
}

/***************************************************************************
 * Least-squares solver for double matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
template <typename T>
static void solveLeastSq(int m, int n, int k, T *c, T *a, T *b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  int M = m;
  int N = n;
  int NRHS = k;
  T *A = a;
  int LDA = m;
  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<T> B(Bsize*NRHS);
  changeStride(&B,Bsize,b,m,m,NRHS);
  int LDB = Bsize;
  MemBlock<int> JPVT(N);
  T RCOND = lamch<T>();
  int RANK;
  T WORKSIZE;
  int LWORK;
  int INFO;
  LWORK = -1;
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORKSIZE, &LWORK, &INFO);
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORK, &LWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N) {
      io->warning(FMString("Matrix is rank deficient to machine precision.  RANK = ") + Stringify(RANK));
    }
  } else
    // Problem should be underdetermined, rank should be M
    if (RANK < M) {
      io->warning(FMString("Matrix is rank deficient to machine precision.  RANK = ") + Stringify(RANK));
    }
  changeStride(c,n,&B,Bsize,n,k);
}

/***************************************************************************
 * Least-squares solver for complex matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
template <typename T>
static void solveLeastSq(int m, int n, int k, FM::Complex<T> *c, FM::Complex<T> *a, FM::Complex<T> *b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  int M = m;
  int N = n;
  int NRHS = k;
  FM::Complex<T> *A = a;
  int LDA = m;
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<Complex<T> > B(Bsize*NRHS);
  changeStride(&B,Bsize,b,m,m,NRHS);
  int LDB = Bsize;
  MemBlock<int> JPVT(N);
  T RCOND = lamch<T>();
  int RANK;
  FM::Complex<T> WORKSIZE;
  int LWORK;
  MemBlock<T> RWORK(2*N);
  int INFO;
  LWORK = -1;
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORKSIZE, &LWORK, &RWORK, &INFO);
  LWORK = (int) WORKSIZE.r;
  MemBlock<Complex<T> > WORK(LWORK);
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORK, &LWORK, &RWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N)
      io->warning(FMString("Matrix is rank deficient to machine precision.  RANK = ") + Stringify(RANK));
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) 
      io->warning(FMString("Matrix is rank deficient to machine precision.  RANK = ") + Stringify(RANK));
  changeStride(c,n,&B,Bsize,n,k);
}

namespace FM {
  
  template <class T>
  void DenseSolve(int m, int n, int k, T *c, const T *a, const T *b, TermIF *io)
  {
    MemBlock<T> A(m*n);
    memcpy(&A,a,m*n*sizeof(T));
    MemBlock<T> B(m*k);
    memcpy(&B,b,m*k*sizeof(T));
    if (m == n)
      solveLinEq(m,k,c,&A,&B,io);
    else
      solveLeastSq(m,n,k,c,&A,&B,io);
  }

  template void DenseSolve<double>(int,int,int,double*,const double*, const double*, TermIF *);
  template void DenseSolve<float>(int,int,int,float*,const float*, const float*, TermIF*);
  template void DenseSolve<Complex<double> >(int,int,int, Complex<double>*,const Complex<double>*, const Complex<double>*, TermIF *);
  template void DenseSolve<Complex<float> >(int,int,int, Complex<float>*,const Complex<float>*, const Complex<float>*, TermIF*);
  
}
