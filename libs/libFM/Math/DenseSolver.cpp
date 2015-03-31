#include "DenseSolver.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include "TermIF.hpp"
#include "FMLib.hpp"

using namespace FM;

/***************************************************************************
 * Linear equation solver for real matrices
 ***************************************************************************/

template <typename T>
static void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
		   T *A, int * LDA, T * AF, int * LDAF, 
		   int * IPIV, char * EQUED, T * R, T * C, 
		   T * B, int * LDB, T * X, int * LDX, 
		   T * RCOND, T * FERR, T * BERR,
		   T * WORK, int * IWORK, int * INFO, 
		   ftnlen l1, ftnlen l2, ftnlen l3);

template <>
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

template <>
void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
		   double *A, int * LDA, double * AF, int * LDAF, 
		   int * IPIV, char * EQUED, double * R, double * C, 
		   double * B, int * LDB, double * X, int * LDX, 
		   double * RCOND, double * FERR, double * BERR,
	    double * WORK, int * IWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return dgesvx_(FACT,TRANS,N,NRHS,A,LDA,AF,LDAF,IPIV,EQUED,R,C,B,
		 LDB,X,LDX,RCOND,FERR,BERR,WORK,IWORK,INFO,l1,l2,3);
}

template <typename T>
static void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
		   T *A, int * LDA, T * AF, int * LDAF, 
		   int * IPIV, char * EQUED, T * R, T * C, 
		   T * B, int * LDB, T * X, int * LDX, 
		   T * RCOND, T * FERR, T * BERR,
		   T * WORK, T * RWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3);

template <>
void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
		   float *A, int * LDA, float * AF, int * LDAF, 
		   int * IPIV, char * EQUED, float * R, float * C, 
		   float * B, int * LDB, float * X, int * LDX, 
		   float * RCOND, float * FERR, float * BERR,
		   float * WORK, float * RWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return cgesvx_(FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV, EQUED, R, C, B,
		 LDB, X, LDX, RCOND, FERR, BERR, WORK, RWORK, INFO, l1, l2, l3);
}

template <>
void Tgesvx(char* FACT, char* TRANS, int * N, int * NRHS, 
		   double *A, int * LDA, double * AF, int * LDAF, 
		   int * IPIV, char * EQUED, double * R, double * C, 
		   double * B, int * LDB, double * X, int * LDX, 
		   double * RCOND, double * FERR, double * BERR,
		   double * WORK, double * RWORK, int * INFO, ftnlen l1, ftnlen l2, ftnlen l3) {
  return zgesvx_(FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV, EQUED, R, C, B,
		 LDB, X, LDX, RCOND, FERR, BERR, WORK, RWORK, INFO, l1, l2, l3);
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
template <typename T>
static void realSolveLinEq(int m, int n, T *c, T* a, T *b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE DGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            IPIV( * ), IWORK( * )
  //	DOUBLE PRECISION   A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  DGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*
  //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  T* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) DOUBLE PRECISION array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  MemBlock<T> AF(LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  MemBlock<int> IPIV(N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  MemBlock<T> R(N);

  //*  C       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.

  MemBlock<T> C(N);

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  T *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  T *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) DOUBLE PRECISION
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  T RCOND;
  
  //*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  MemBlock<T> FERR(n);

  //*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  MemBlock<T> BERR(n);

  //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (4*N)
  //*          On exit, WORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If WORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          WORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.

  MemBlock<T> WORK(4*N);

  //*  IWORK   (workspace) INTEGER array, dimension (N)

  MemBlock<int> IWORK(4*N);

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  Tgesvx(&FACT, &TRANS, &N, &NRHS, A, &LDA, &AF, &LDAF, &IPIV, &EQUED, &R, &C, B,
	 &LDB, X, &LDX, &RCOND, &FERR, &BERR, &WORK, &IWORK, &INFO,1,1,1);
  if ((INFO == N) || (INFO == N+1) || (RCOND < lamch<T>()))
    io->warning(FMString("Matrix is singular to working precision.  RCOND = ") + Stringify(RCOND));
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.

template <typename T>
static void complexSolveLinEq(int m, int n, T *c, T* a, T* b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE ZGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            IPIV( * )
  //      DOUBLE PRECISION   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   RWORK( * )
  //      COMPLEX*16         A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*  A       (input/output) COMPLEX*16 array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  T* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) COMPLEX*16 array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  MemBlock<T> AF(2*LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  MemBlock<int> IPIV(N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  MemBlock<T> R(N);

  //*  C       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.
  
  MemBlock<T> C(N);

  //*  B       (input/output) COMPLEX*16 array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  T *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) COMPLEX*16 array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  T *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) DOUBLE PRECISION
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  T RCOND;
  
  //*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  MemBlock<T> FERR(n);

  //*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  MemBlock<T> BERR(n);

  //*  WORK    (workspace) COMPLEX*16 array, dimension (2*N)

  MemBlock<T> WORK(2*(2*N));
  
  //*  RWORK   (workspace/output) DOUBLE PRECISION array, dimension (2*N)
  //*          On exit, RWORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If RWORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          RWORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.

  MemBlock<T> RWORK(2*N);

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  Tgesvx(&FACT, &TRANS, &N, &NRHS, A, &LDA, &AF, &LDAF, &IPIV, &EQUED, &R, &C, B,
	 &LDB, X, &LDX, &RCOND, &FERR, &BERR, &WORK, &RWORK, &INFO, 1, 1, 1);
  if ((INFO == N) || (INFO == N+1) || (RCOND < lamch<T>())) {
    io->warning(FMString("Matrix is singular to working precision.  RCOND = ") + Stringify(RCOND));
  }
}

template <typename T>
static void Tgelsy(int* M, int *N, int *NRHS, T* A, int *LDA,
	    T *B, int *LDB, int *JPVT, T* RCOND,
	    int *RANK, T *WORK, int* LWORK, int* INFO);

template <>
void Tgelsy(int* M, int *N, int *NRHS, double* A, int *LDA,
		   double *B, int *LDB, int *JPVT, double* RCOND,
		   int *RANK, double *WORK, int* LWORK, int* INFO) {
  return dgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);
}

template <>
void Tgelsy(int* M, int *N, int *NRHS, float* A, int *LDA,
		   float *B, int *LDB, int *JPVT, float* RCOND,
		   int *RANK, float *WORK, int* LWORK, int* INFO) {
  return sgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);  
}

template <typename T>
static void Tgelsy(int* M, int *N, int *NRHS, T* A, int *LDA,
		   T *B, int *LDB, int *JPVT, T* RCOND,
		   int *RANK, T *WORK, int* LWORK, T* RWORK,
		   int* INFO);

template <>
void Tgelsy(int* M, int *N, int *NRHS, float* A, int *LDA,
		   float *B, int *LDB, int *JPVT, float* RCOND,
		   int *RANK, float *WORK, int* LWORK, float* RWORK,
		   int* INFO) {
  return cgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,RWORK,INFO);
}

template <>
void Tgelsy(int* M, int *N, int *NRHS, double* A, int *LDA,
		   double *B, int *LDB, int *JPVT, double* RCOND,
		   int *RANK, double *WORK, int* LWORK, double* RWORK,
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
static void realSolveLeastSq(int m, int n, int k, T *c, T *a, T *b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE DGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  DGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  T *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<T> B(Bsize*NRHS);
  changeStride(&B,Bsize,b,m,m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.
  MemBlock<int> JPVT(N);

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  T RCOND = lamch<T>();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  T WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*             LWORK >= MAX( MN+3*N+1, 2*MN+NRHS ),
  //*          where MN = min( M, N ).
  //*          The block algorithm requires that:
  //*             LWORK >= MAX( MN+2*N+NB*(N+1), 2*MN+NB*NRHS ),
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines DGEQP3, DTZRZF, STZRQF, DORMQR,
  //*          and DORMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
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
static void complexSolveLeastSq(int m, int n, int k, T *c, T *a, T*b, TermIF *io) {
  if ((m == 0) || (n == 0)) return;
  //	SUBROUTINE ZGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   RWORK( * )
  //	COMPLEX*16         A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) COMPLEX*16 array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  T *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<T> B(Bsize*NRHS*2);
  changeStride(&B,2*Bsize,b,2*m,2*m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  MemBlock<int> JPVT(N);

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  T RCOND = lamch<T>();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) COMPLEX*16 array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  T WORKSIZE[2];

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*            LWORK >= MN + MAX( 2*MN, N+1, MN+NRHS )
  //*          where MN = min(M,N).
  //*          The block algorithm requires that:
  //*            LWORK >= MN + MAX( 2*MN, NB*(N+1), MN+MN*NB, MN+NB*NRHS )
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines ZGEQP3, ZTZRZF, CTZRQF, ZUNMQR,
  //*          and ZUNMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  RWORK   (workspace) DOUBLE PRECISION array, dimension (2*N)

  MemBlock<T> RWORK(2*N);

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, WORKSIZE, &LWORK, &RWORK, &INFO);
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK);
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
  changeStride(c,2*n,&B,2*Bsize,2*n,k);
}

template <class T>
void FM::DenseSolve(int m, int n, int k, T *c, const T *a, const T *b, TermIF *io)
{
  MemBlock<T> A(m*n);
  memcpy(&A,a,m*n*sizeof(T));
  MemBlock<T> B(m*k);
  memcpy(&B,b,m*k*sizeof(T));
  if (m == n)
    realSolveLinEq<T>(m,k,c,&A,&B,io);
  else
    realSolveLeastSq<T>(m,n,k,c,&A,&B,io);
}

template <class T>
void FM::DenseSolveComplex(int m, int n, int k, T *c, const T *a, const T *b, TermIF *io)
{
  MemBlock<T> A(m*n*2);
  memcpy(&A,a,m*n*sizeof(T)*2);
  MemBlock<T> B(m*k*2);
  memcpy(&B,b,m*k*sizeof(T)*2);
  if (m == n)
    complexSolveLinEq<T>(m,k,c,&A,&B,io);
  else
    complexSolveLeastSq<T>(m,n,k,c,&A,&B,io);  
}

template void FM::DenseSolve<double>(int,int,int,double*,const double*, const double*, TermIF *);
template void FM::DenseSolve<float>(int,int,int,float*,const float*, const float*, TermIF*);
template void FM::DenseSolveComplex<double>(int,int,int,double*,const double*, const double*, TermIF *);
template void FM::DenseSolveComplex<float>(int,int,int,float*,const float*, const float*, TermIF*);
