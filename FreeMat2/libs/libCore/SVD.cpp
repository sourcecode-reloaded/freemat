#include "Array.hpp"
#include "MemPtr.hpp"
#include "LAPACK.hpp"
#include "Algorithms.hpp"
#include <QtCore>


template <typename T>
static void Tgesdd(char* JOBS, int* M, int *N, T* A, int *LDA, T *S, 
		   T *U, int *LDU, T *VT, int *LDVT, T *WORK,
		   int *LWORK, int *IWORK, int *INFO);

template <>
static void Tgesdd(char* JOBS, int* M, int *N, float* A, int *LDA, float *S, 
		   float *U, int *LDU, float *VT, int *LDVT, float *WORK,
		   int *LWORK, int *IWORK, int *INFO) {
  sgesdd_(JOBS,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,IWORK,INFO);
}

template <>
static void Tgesdd(char* JOBS, int* M, int *N, double* A, int *LDA, double *S, 
		   double *U, int *LDU, double *VT, int *LDVT, double *WORK,
		   int *LWORK, int *IWORK, int *INFO) {
  dgesdd_(JOBS,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,IWORK,INFO);
}

template <typename T>
static void Tgesdd(char* JOBS, int* M, int *N, T* A, int *LDA, T *S, 
		   T *U, int *LDU, T *VT, int *LDVT, T *WORK,
		   int *LWORK, T *RWORK, int *IWORK, int *INFO);

template <>
static void Tgesdd(char* JOBS, int* M, int *N, float* A, int *LDA, float *S, 
		   float *U, int *LDU, float *VT, int *LDVT, float *WORK,
		   int *LWORK, float *RWORK, int *IWORK, int *INFO) {
  cgesdd_(JOBS,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,RWORK,IWORK,INFO);
}

template <>
static void Tgesdd(char* JOBS, int* M, int *N, double* A, int *LDA, double *S, 
		   double *U, int *LDU, double *VT, int *LDVT, double *WORK,
		   int *LWORK, double *RWORK, int *IWORK, int *INFO) {
  zgesdd_(JOBS,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,RWORK,IWORK,INFO);
}


template <typename T>
static void TSVD(int nrows, int ncols, BasicArray<T> &U, BasicArray<T> &VT, 
		 BasicArray<T> &S, BasicArray<T> &A, bool compact, bool vectors) {
  //      SUBROUTINE SGESDD( JOBZ, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK,
  //     $                   LWORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     October 31, 1999
  //*
  //*     .. Scalar Arguments ..
  //      CHARACTER          JOBZ
  //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            IWORK( * )
  //      REAL               A( LDA, * ), S( * ), U( LDU, * ),
  //     $                   VT( LDVT, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGESDD computes the singular value decomposition (SVD) of a real
  //*  M-by-N matrix A, optionally computing the left and right singular
  //*  vectors.  If singular vectors are desired, it uses a
  //*  divide-and-conquer algorithm.
  //*
  //*  The SVD is written
  //*
  //*       A = U * SIGMA * transpose(V)
  //*
  //*  where SIGMA is an M-by-N matrix which is zero except for its
  //*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
  //*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
  //*  are the singular values of A; they are real and non-negative, and
  //*  are returned in descending order.  The first min(m,n) columns of
  //*  U and V are the left and right singular vectors of A.
  //*
  //*  Note that the routine returns VT = V**T, not V.
  //*
  //*  The divide and conquer algorithm makes very mild assumptions about
  //*  floating point arithmetic. It will work on machines with a guard
  //*  digit in add/subtract, or on those binary machines without guard
  //*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
  //*  Cray-2. It could conceivably fail on hexadecimal or decimal machines
  //*  without guard digits, but we know of none.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  JOBZ    (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix U:
  //*          = 'A':  all M columns of U and all N rows of V**T are
  //*                  returned in the arrays U and VT;
  //*          = 'S':  the first min(M,N) columns of U and the first
  //*                  min(M,N) rows of V**T are returned in the arrays U
  //*                  and VT;
  //*          = 'O':  If M >= N, the first N columns of U are overwritten
  //*                  on the array A and all rows of V**T are returned in
  //*                  the array VT;
  //*                  otherwise, all columns of U are returned in the
  //*                  array U and the first M rows of V**T are overwritten
  //*                  in the array VT;
  //*          = 'N':  no columns of U or rows of V**T are computed.
  //*
  char JOBZ;
  if (!vectors)
    JOBZ = 'N';
  else {
    if (!compact)
      JOBZ = 'A';
    else
      JOBZ = 'S';
  }
  //*  M       (input) INTEGER
  //*          The number of rows of the input matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the input matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit,
  //*          if JOBZ = 'O',  A is overwritten with the first N columns
  //*                          of U (the left singular vectors, stored
  //*                          columnwise) if M >= N;
  //*                          A is overwritten with the first M rows
  //*                          of V**T (the right singular vectors, stored
  //*                          rowwise) otherwise.
  //*          if JOBZ .ne. 'O', the contents of A are destroyed.
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  S       (output) REAL array, dimension (min(M,N))
  //*          The singular values of A, sorted so that S(i) >= S(i+1).
  //*
  //*  U       (output) REAL array, dimension (LDU,UCOL)
  //*          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
  //*          UCOL = min(M,N) if JOBZ = 'S'.
  //*          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
  //*          orthogonal matrix U;
  //*          if JOBZ = 'S', U contains the first min(M,N) columns of U
  //*          (the left singular vectors, stored columnwise);
  //*          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.
  //*
  //*  LDU     (input) INTEGER
  //*          The leading dimension of the array U.  LDU >= 1; if
  //*          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.
  //*
  int LDU = nrows;
  //*  VT      (output) REAL array, dimension (LDVT,N)
  //*          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
  //*          N-by-N orthogonal matrix V**T;
  //*          if JOBZ = 'S', VT contains the first min(M,N) rows of
  //*          V**T (the right singular vectors, stored rowwise);
  //*          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.
  //*
  //*  LDVT    (input) INTEGER
  //*          The leading dimension of the array VT.  LDVT >= 1; if
  //*          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
  //*          if JOBZ = 'S', LDVT >= min(M,N).
  //*
  int LDVT;
  if (!compact)
    LDVT = ncols;
  else
    LDVT = (ncols < nrows) ? ncols : nrows;
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
  //*
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 1.
  //*          If JOBZ = 'N',
  //*            LWORK >= 3*min(M,N) + max(max(M,N),6*min(M,N)).
  //*          If JOBZ = 'O',
  //*            LWORK >= 3*min(M,N)*min(M,N) + 
  //*                     max(max(M,N),5*min(M,N)*min(M,N)+4*min(M,N)).
  //*          If JOBZ = 'S' or 'A'
  //*            LWORK >= 3*min(M,N)*min(M,N) +
  //*                     max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)).
  //*          For good performance, LWORK should generally be larger.
  //*          If LWORK < 0 but other input arguments are legal, WORK(1)
  //*          returns the optimal LWORK.
  //*
  //*  IWORK   (workspace) INTEGER array, dimension (8*min(M,N))
  //*
  int minMN = (M < N) ? M : N;
  MemBlock<int> IWORKBlock(8*minMN);
  int *IWORK = &IWORKBlock;
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit.
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
  //*          > 0:  SBDSDC did not converge, updating process failed.
  //*
  int INFO;
  // LWORK Calculation does not work in Lapack3
  int LWORK = qMax(3*qMin(M,N) + qMax(qMax(M,N),6*qMin(M,N)),
		  3*qMin(M,N)*qMin(M,N) +
		  qMax(qMax(M,N),4*qMin(M,N)*qMin(M,N)+4*qMin(M,N)));
  MemBlock<T> WORKBlock(LWORK);
  T* WORK = &WORKBlock;
  Tgesdd( &JOBZ, &M, &N, A.data(), &LDA, S.data(), U.data(), &LDU, VT.data(), &LDVT, 
	  WORK, &LWORK, IWORK, &INFO);
}

template <typename T>
static void TSVD(int nrows, int ncols, BasicArray<T> &U, BasicArray<T> &VT, 
		 BasicArray<T> &S, BasicArray<T> &a_real, BasicArray<T> &a_imag, 
		 bool compact, bool vectors) {
  //       SUBROUTINE CGESDD( JOBZ, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK,
  //      $                   LWORK, RWORK, IWORK, INFO )
  // *
  // *  -- LAPACK driver routine (version 3.0) --
  // *     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  // *     Courant Institute, Argonne National Lab, and Rice University
  // *     October 31, 1999
  // *
  // *     .. Scalar Arguments ..
  //       CHARACTER          JOBZ
  //       INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  // *     ..
  // *     .. Array Arguments ..
  //       INTEGER            IWORK( * )
  //       REAL               RWORK( * ), S( * )
  //       COMPLEX            A( LDA, * ), U( LDU, * ), VT( LDVT, * ),
  //      $                   WORK( * )
  // *     ..
  // *
  // *  Purpose
  // *  =======
  // *
  // *  CGESDD computes the singular value decomposition (SVD) of a complex
  // *  M-by-N matrix A, optionally computing the left and/or right singular
  // *  vectors, by using divide-and-conquer method. The SVD is written
  // *
  // *       A = U * SIGMA * conjugate-transpose(V)
  // *
  // *  where SIGMA is an M-by-N matrix which is zero except for its
  // *  min(m,n) diagonal elements, U is an M-by-M unitary matrix, and
  // *  V is an N-by-N unitary matrix.  The diagonal elements of SIGMA
  // *  are the singular values of A; they are real and non-negative, and
  // *  are returned in descending order.  The first min(m,n) columns of
  // *  U and V are the left and right singular vectors of A.
  // *
  // *  Note that the routine returns VT = V**H, not V.
  // *
  // *  The divide and conquer algorithm makes very mild assumptions about
  // *  floating point arithmetic. It will work on machines with a guard
  // *  digit in add/subtract, or on those binary machines without guard
  // *  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
  // *  Cray-2. It could conceivably fail on hexadecimal or decimal machines
  // *  without guard digits, but we know of none.
  // *
  // *  Arguments
  // *  =========
  // *
  // *  JOBZ    (input) CHARACTER*1
  // *          Specifies options for computing all or part of the matrix U:
  // *          = 'A':  all M columns of U and all N rows of V**H are
  // *                  returned in the arrays U and VT;
  // *          = 'S':  the first min(M,N) columns of U and the first
  // *                  min(M,N) rows of V**H are returned in the arrays U
  // *                  and VT;
  // *          = 'O':  If M >= N, the first N columns of U are overwritten
  // *                  on the array A and all rows of V**H are returned in
  // *                  the array VT;
  // *                  otherwise, all columns of U are returned in the
  // *                  array U and the first M rows of V**H are overwritten
  // *                  in the array VT;
  // *          = 'N':  no columns of U or rows of V**H are computed.
  // *
  char JOBZ;
  if (!vectors)
    JOBZ = 'N';
  else {
    if (!compact)
      JOBZ = 'A';
    else
      JOBZ = 'S';
  }
  // *  M       (input) INTEGER
  // *          The number of rows of the input matrix A.  M >= 0.
  // *
  int M = nrows;
  // *  N       (input) INTEGER
  // *          The number of columns of the input matrix A.  N >= 0.
  // *
  int N = ncols;
  // *  A       (input/output) COMPLEX array, dimension (LDA,N)
  // *          On entry, the M-by-N matrix A.
  // *          On exit,
  // *          if JOBZ = 'O',  A is overwritten with the first N columns
  // *                          of U (the left singular vectors, stored
  // *                          columnwise) if M >= N;
  // *                          A is overwritten with the first M rows
  // *                          of V**H (the right singular vectors, stored
  // *                          rowwise) otherwise.
  // *          if JOBZ .ne. 'O', the contents of A are destroyed.
  // *
  BasicArray<T> A(MergeComplex(a_real,a_imag));
  // *  LDA     (input) INTEGER
  // *          The leading dimension of the array A.  LDA >= max(1,M).
  // *
  int LDA = nrows;    
  // *  S       (output) REAL array, dimension (min(M,N))
  // *          The singular values of A, sorted so that S(i) >= S(i+1).
  // *
  // *  U       (output) COMPLEX array, dimension (LDU,UCOL)
  // *          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
  // *          UCOL = min(M,N) if JOBZ = 'S'.
  // *          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
  // *          unitary matrix U;
  // *          if JOBZ = 'S', U contains the first min(M,N) columns of U
  // *          (the left singular vectors, stored columnwise);
  // *          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.
  // *
  // *  LDU     (input) INTEGER
  // *          The leading dimension of the array U.  LDU >= 1; if
  // *          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.
  // *
  int LDU = nrows;    
  // *  VT      (output) COMPLEX array, dimension (LDVT,N)
  // *          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
  // *          N-by-N unitary matrix V**H;
  // *          if JOBZ = 'S', VT contains the first min(M,N) rows of
  // *          V**H (the right singular vectors, stored rowwise);
  // *          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.
  // *
  // *  LDVT    (input) INTEGER
  // *          The leading dimension of the array VT.  LDVT >= 1; if
  // *          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
  // *          if JOBZ = 'S', LDVT >= min(M,N).
  // *
  int LDVT;
  if (!compact)
    LDVT = ncols;
  else
    LDVT = (ncols < nrows) ? ncols : nrows;
  // *  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  // *          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  // *
  // *  LWORK   (input) INTEGER
  // *          The dimension of the array WORK. LWORK >= 1.
  // *          if JOBZ = 'N', LWORK >= 2*min(M,N)+max(M,N).
  // *          if JOBZ = 'O',
  // *                LWORK >= 2*min(M,N)*min(M,N)+2*min(M,N)+max(M,N).
  // *          if JOBZ = 'S' or 'A',
  // *                LWORK >= min(M,N)*min(M,N)+2*min(M,N)+max(M,N).
  // *          For good performance, LWORK should generally be larger.
  // *          If LWORK < 0 but other input arguments are legal, WORK(1)
  // *          returns the optimal LWORK.
  // *
  // *  RWORK   (workspace) REAL array, dimension (LRWORK)
  // *          If JOBZ = 'N', LRWORK >= 7*min(M,N).
  // *          Otherwise, LRWORK >= 5*min(M,N)*min(M,N) + 5*min(M,N)
  // *
  int minMN = (M < N) ? M : N;
  MemBlock<T> RWORKBlock(5*minMN*minMN+5*minMN);
  T *RWORK = &RWORKBlock;
  // *  IWORK   (workspace) INTEGER array, dimension (8*min(M,N))
  // *
  MemBlock<int> IWORKBlock(8*minMN);
  int *IWORK = &IWORKBlock;
  // *  INFO    (output) INTEGER
  // *          = 0:  successful exit.
  // *          < 0:  if INFO = -i, the i-th argument had an illegal value.
  // *          > 0:  The updating process of SBDSDC did not converge.
  int INFO;
  T WORKSZE[2];
  int LWORK = -1;
  Tgesdd( &JOBZ, &M, &N, A.data(), &LDA, S.data(), U.data(), &LDU, VT.data(), &LDVT, 
	  WORKSZE, &LWORK, RWORK, IWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORKBlock(2*LWORK);
  T* WORK = &WORKBlock;
  Tgesdd( &JOBZ, &M, &N, A.data(), &LDA, S.data(), U.data(), &LDU, VT.data(), &LDVT, 
	  WORKSZE, &LWORK, RWORK, IWORK, &INFO);
}


template <typename T>
static ArrayVector SVDFunction(BasicArray<T> &A, bool computevectors, bool compactform) {
  int nrows = int(A.rows());
  int ncols = int(A.cols());
  int mindim = (nrows < ncols) ? nrows : ncols;
  // A vector to store the singular values
  BasicArray<T> svals(NTuple(mindim,1));
  // A matrix to store the left singular vectors
  BasicArray<T> umat;
  // A matrix to store the right singular vectors
  BasicArray<T> vtmat;
  if (computevectors) 
    if (!compactform) {
      umat = BasicArray<T>(NTuple(nrows,nrows));
      vtmat = BasicArray<T>(NTuple(ncols,ncols));
    } else {
      umat = BasicArray<T>(NTuple(nrows,mindim));
      vtmat = BasicArray<T>(NTuple(mindim,ncols));
    }
  TSVD<T>(nrows,ncols,umat,vtmat,svals,A,compactform,computevectors);
  ArrayVector retval;
  if (!computevectors) {
    retval.push_back(Array(svals));
  } else {
    retval.push_back(Array(umat));
    BasicArray<T> smat;
    if (!compactform) 
      smat = BasicArray<T>(NTuple(A.rows(),A.cols()));
    else
      smat = BasicArray<T>(NTuple(mindim,mindim));
    for (index_t i=1;i!=mindim;i++)
      smat[NTuple(i,i)] = svals[i];
    retval.push_back(Array(smat));
    retval.push_back(Transpose(Array(vtmat)));
  }      
  return retval;
}

template <typename T>
static ArrayVector SVDFunction(BasicArray<T> &A_real, 
			       BasicArray<T> &A_imag,
			       bool computevectors, bool compactform) {
  int nrows = int(A_real.rows());
  int ncols = int(A_real.cols());
  int mindim = (nrows < ncols) ? nrows : ncols;
  // A vector to store the singular values
  BasicArray<T> svals(NTuple(2*mindim,1));
  // A matrix to store the left singular vectors
  BasicArray<T> umat;
  // A matrix to store the right singular vectors
  BasicArray<T> vtmat;
  if (computevectors) 
    if (!compactform) {
      umat = BasicArray<T>(NTuple(2*nrows,nrows));
      vtmat = BasicArray<T>(NTuple(2*ncols,ncols));
    } else {
      umat = BasicArray<T>(NTuple(2*nrows,mindim));
      vtmat = BasicArray<T>(NTuple(2*mindim,ncols));
    }
  TSVD<T>(nrows,ncols,umat,vtmat,svals,A_real,A_imag,compactform,computevectors);
  ArrayVector retval;
  if (!computevectors) {
    retval.push_back(Array(SplitReal(svals),SplitImag(svals)));
  } else {
    retval.push_back(Array(SplitReal(umat),SplitImag(umat)));
    BasicArray<T> smat_real;
    BasicArray<T> smat_imag;
    if (!compactform) {
      smat_real = BasicArray<T>(NTuple(A_real.rows(),A_real.cols()));
      smat_imag = BasicArray<T>(NTuple(A_real.rows(),A_real.cols()));
    } else {
      smat_real = BasicArray<T>(NTuple(mindim,mindim));
      smat_imag = BasicArray<T>(NTuple(mindim,mindim));
    }
    for (index_t i=1;i!=mindim;i++) {
      smat_real[NTuple(i,i)] = svals[2*i-1];
      smat_imag[NTuple(i,i)] = svals[2*i];
    }
    retval.push_back(Array(smat_real,smat_imag));
    retval.push_back(Hermitian(Array(SplitReal(vtmat),SplitImag(vtmat))));
  }
  return retval;
}

//!
//@Module SVD Singular Value Decomposition of a Matrix
//@@Section TRANSFORMS
//@@Usage
//Computes the singular value decomposition (SVD) of a matrix.  The 
//@|svd| function has three forms.  The first returns only the singular
//values of the matrix:
//@[
//  s = svd(A)
//@]
//The second form returns both the singular values in a diagonal
//matrix @|S|, as well as the left and right eigenvectors.
//@[
//  [U,S,V] = svd(A)
//@]
//The third form returns a more compact decomposition, with the
//left and right singular vectors corresponding to zero singular
//values being eliminated.  The syntax is
//@[
//  [U,S,V] = svd(A,0)
//@]
//@@Function Internals
//Recall that @|sigma_i| is a singular value of an @|M x N|
//matrix @|A| if there exists two vectors @|u_i, v_i| where @|u_i| is
//of length @|M|, and @|v_i| is of length @|u_i| and
//\[
//  A v_i = \sigma_i u_i
//\]
//and generally
//\[
//  A = \sum_{i=1}^{K} \sigma_i u_i*v_i',
//\]
//where @|K| is the rank of @|A|.  In matrix form, the left singular
//vectors @|u_i| are stored in the matrix @|U| as
//\[
//  U = [u_1,\ldots,u_m], V = [v_1,\ldots,v_n]
//\]
//The matrix @|S| is then of size @|M x N| with the singular
//values along the diagonal.  The SVD is computed using the 
//@|LAPACK| class of functions @|GESDD|.
//@@Examples
//Here is an example of a partial and complete singular value
//decomposition.
//@<
//A = float(randn(2,3))
//[U,S,V] = svd(A)
//U*S*V'
//svd(A)
//@>
//@@Tests
//@{ test_svd1.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd1
//  a = [1,3;4,2];
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd2.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd2
//  a = double([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd3.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd3
//  a = complex([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd4.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd4
//  a = dcomplex([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd5.m
//% Test the full version of the svd with float vectors
//function test_val = test_svd5
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd6.m
//% Test the full version of the svd with complex vectors
//function test_val = test_svd6
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n))+i*float(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n))+i*float(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n))+i*float(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd7.m
//% Test the full version of the svd with double vectors
//function test_val = test_svd7
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = double(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd8.m
//% Test the full version of the svd with dcomplex vectors
//function test_val = test_svd8
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = double(rand(n,2*n))+i*double(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(2*n,n))+i*double(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(n,n))+i*double(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd9.m
//% Test the compact version of the svd with float vectors
//function test_val = test_svd9
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd10.m
//% Test the compact version of the svd with complex vectors
//function test_val = test_svd10
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n))+i*float(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n))+i*float(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n))+i*float(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd11.m
//% Test the compact version of the svd with double vectors
//function test_val = test_svd11
//  t1all = 1;
//  p = [];
//  for n=2:100
//    a = double(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd12.m
//% Test the compact version of the svd with dcomplex vectors
//function test_val = test_svd12
//  t1all = 1;
//  p = [];
//  for n=2:100
//    a = double(rand(n,2*n))+i*double(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(2*n,n))+i*double(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(n,n))+i*double(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 10*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@@Signature
//function svd SVDFunction
//inputs A flag
//outputs U S V
//!

ArrayVector SVDFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 2)
    throw Exception("svd function takes at most two arguments");
  if (arg.size() < 1)
    throw Exception("svd function requries at least one argument - the matrix to decompose");
  Array A(arg[0]);
  bool compactform = false;
  if (arg.size() > 1) {
    Array flag(arg[1]);
    if (flag.asInteger() == 0)
      compactform = true;
  }
  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply svd to reference types.");
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  if (A.isSparse())
    throw Exception("SVD does not work with sparse matrices.");
  if (AnyNotFinite(A))
    throw Exception("SVD only defined for matrices with finite entries.");
  int nrows = int(A.rows());
  int ncols = int(A.cols());
  bool computevectors = (nargout>1);
  switch (A.dataClass()) {
  default: throw Exception("illegal argument type to svd");
  case Float:
    if (A.allReal())
      return SVDFunction(A.real<float>(),computevectors,compactform);
    else
      return SVDFunction(A.real<float>(),A.imag<float>(),computevectors,compactform);
  case Double:
    if (A.allReal())
      return SVDFunction(A.real<float>(),computevectors,compactform);
    else
      return SVDFunction(A.real<float>(),A.imag<float>(),computevectors,compactform);
  }
}
