/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "SingularValueDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

template <typename T>
static void realSVD(int nrows, int ncols, T* u, T* vt, T* s, T* a, bool compact, bool vectors) {
  // Here are the comments from the LAPACK routine SGESVD
  //      SUBROUTINE SGESVD( JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT,
  //     $                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     October 31, 1999
  //*
  //*     .. Scalar Arguments ..
  //      CHARACTER          JOBU, JOBVT
  //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), S( * ), U( LDU, * ),
  //     $                   VT( LDVT, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGESVD computes the singular value decomposition (SVD) of a real
  //*  M-by-N matrix A, optionally computing the left and/or right singular
  //*  vectors. The SVD is written
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
  //*  Note that the routine returns V**T, not V.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  JOBU    (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix U:
  //*          = 'A':  all M columns of U are returned in array U:
  //*          = 'S':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are returned in the array U;
  //*          = 'O':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no columns of U (no left singular vectors) are
  //*                  computed.
  //*
  //*  JOBVT   (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix
  //*          V**T:
  //*          = 'A':  all N rows of V**T are returned in the array VT;
  //*          = 'S':  the first min(m,n) rows of V**T (the right singular
  //*                  vectors) are returned in the array VT;
  //*          = 'O':  the first min(m,n) rows of V**T (the right singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no rows of V**T (no right singular vectors) are
  //*                  computed.
  //*
  //*          JOBVT and JOBU cannot both be 'O'.
  //*
  char JOBU;
  if (!vectors)
    JOBU = 'N';
  else {
    if (!compact)
      JOBU = 'A';
    else
      JOBU = 'S';
  }
  char JOBVT = JOBU;
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
  //*          if JOBU = 'O',  A is overwritten with the first min(m,n)
  //*                          columns of U (the left singular vectors,
  //*                          stored columnwise);
  //*          if JOBVT = 'O', A is overwritten with the first min(m,n)
  //*                          rows of V**T (the right singular vectors,
  //*                          stored rowwise);
  //*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
  //*                          are destroyed.
  //*
  T *A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  S       (output) REAL array, dimension (min(M,N))
  //*          The singular values of A, sorted so that S(i) >= S(i+1).
  //*
  T *S = s;
  //*  U       (output) REAL array, dimension (LDU,UCOL)
  //*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
  //*          If JOBU = 'A', U contains the M-by-M orthogonal matrix U;
  //*          if JOBU = 'S', U contains the first min(m,n) columns of U
  //*          (the left singular vectors, stored columnwise);
  //*          if JOBU = 'N' or 'O', U is not referenced.
  //*
  T *U = u;
  //*  LDU     (input) INTEGER
  //*          The leading dimension of the array U.  LDU >= 1; if
  //*          JOBU = 'S' or 'A', LDU >= M.
  //*
  int LDU = nrows;
  //*  VT      (output) REAL array, dimension (LDVT,N)
  //*          If JOBVT = 'A', VT contains the N-by-N orthogonal matrix
  //*          V**T;
  //*          if JOBVT = 'S', VT contains the first min(m,n) rows of
  //*          V**T (the right singular vectors, stored rowwise);
  //*          if JOBVT = 'N' or 'O', VT is not referenced.
  //*
  T *VT = vt;
  //*  LDVT    (input) INTEGER
  //*          The leading dimension of the array VT.  LDVT >= 1; if
  //*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).
  //*
  int LDVT;
  if (!compact)
    LDVT = ncols;
  else
    LDVT = (ncols < nrows) ? ncols : nrows;
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
  //*          if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged
  //*          superdiagonal elements of an upper bidiagonal matrix B
  //*          whose diagonal is in S (not necessarily sorted). B
  //*          satisfies A = U * B * VT, so it has the same singular values
  //*          as A, and singular vectors related by U and VT.
  T WORKSIZE;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 1.
  //*          LWORK >= MAX(3*MIN(M,N)+MAX(M,N),5*MIN(M,N)).
  //*          For good performance, LWORK should generally be larger.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit.
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
  //*          > 0:  if SBDSQR did not converge, INFO specifies how many
  //*                superdiagonals of an intermediate bidiagonal form B
  //*                did not converge to zero. See the description of WORK
  //*                above for details.
  int INFO;
  LWORK = -1;
  Tgesvd(&JOBU,&JOBVT,&M,&N,A,&LDA,S,U,&LDU,VT,&LDVT,&WORKSIZE,&LWORK,&INFO);
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tgesvd(&JOBU,&JOBVT,&M,&N,A,&LDA,S,U,&LDU,VT,&LDVT,&WORK,&LWORK,&INFO);
  if (INFO > 0)
    WarningMessage(QString("svd did not converge"));
  if (INFO < 0)
    WarningMessage(QString("svd had illegal value for parameter %1").arg(-INFO));
}

template <typename T>
static void complexSVD(int nrows, int ncols, T *u, T *vt, T *s, T *a, bool compact, bool vectors) {
  // Here are the comments from the LAPACK routine CGESVD
  //
  //      SUBROUTINE CGESVD( JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     October 31, 1999
  //*
  //*     .. Scalar Arguments ..
  //      CHARACTER          JOBU, JOBVT
  //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               RWORK( * ), S( * )
  //      COMPLEX            A( LDA, * ), U( LDU, * ), VT( LDVT, * ),
  //     $                   WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CGESVD computes the singular value decomposition (SVD) of a complex
  //*  M-by-N matrix A, optionally computing the left and/or right singular
  //*  vectors. The SVD is written
  //*
  //*       A = U * SIGMA * conjugate-transpose(V)
  //*
  //*  where SIGMA is an M-by-N matrix which is zero except for its
  //*  min(m,n) diagonal elements, U is an M-by-M unitary matrix, and
  //*  V is an N-by-N unitary matrix.  The diagonal elements of SIGMA
  //*  are the singular values of A; they are real and non-negative, and
  //*  are returned in descending order.  The first min(m,n) columns of
  //*  U and V are the left and right singular vectors of A.
  //*
  //*  Note that the routine returns V**H, not V.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  JOBU    (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix U:
  //*          = 'A':  all M columns of U are returned in array U:
  //*          = 'S':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are returned in the array U;
  //*          = 'O':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no columns of U (no left singular vectors) are
  //*                  computed.
  //*
  char JOBU;
  if (!vectors)
    JOBU = 'N';
  else {
    if (!compact)
      JOBU = 'A';
    else
      JOBU = 'S';
  }
  //*  JOBVT   (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix
  //*          V**H:
  //*          = 'A':  all N rows of V**H are returned in the array VT;
  //*          = 'S':  the first min(m,n) rows of V**H (the right singular
  //*                  vectors) are returned in the array VT;
  //*          = 'O':  the first min(m,n) rows of V**H (the right singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no rows of V**H (no right singular vectors) are
  //*                  computed.
  //*
  //*          JOBVT and JOBU cannot both be 'O'.
  //*
  char JOBVT = JOBU;
  //*  M       (input) INTEGER
  //*          The number of rows of the input matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the input matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit,
  //*          if JOBU = 'O',  A is overwritten with the first min(m,n)
  //*                          columns of U (the left singular vectors,
  //*                          stored columnwise);
  //*          if JOBVT = 'O', A is overwritten with the first min(m,n)
  //*                          rows of V**H (the right singular vectors,
  //*                          stored rowwise);
  //*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
  //*                          are destroyed.
  //*
  T *A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;    
  //*  S       (output) REAL array, dimension (min(M,N))
  //*          The singular values of A, sorted so that S(i) >= S(i+1).
  //*
  T *S = s;
  //*  U       (output) COMPLEX array, dimension (LDU,UCOL)
  //*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
  //*          If JOBU = 'A', U contains the M-by-M unitary matrix U;
  //*          if JOBU = 'S', U contains the first min(m,n) columns of U
  //*          (the left singular vectors, stored columnwise);
  //*          if JOBU = 'N' or 'O', U is not referenced.
  //*
  T *U = u;
  //*  LDU     (input) INTEGER
  //*          The leading dimension of the array U.  LDU >= 1; if
  //*          JOBU = 'S' or 'A', LDU >= M.
  //*
  int LDU = nrows;    
  //*  VT      (output) COMPLEX array, dimension (LDVT,N)
  //*          If JOBVT = 'A', VT contains the N-by-N unitary matrix
  //*          V**H;
  //*          if JOBVT = 'S', VT contains the first min(m,n) rows of
  //*          V**H (the right singular vectors, stored rowwise);
  //*          if JOBVT = 'N' or 'O', VT is not referenced.
  //*
  T *VT = vt;
  //*  LDVT    (input) INTEGER
  //*          The leading dimension of the array VT.  LDVT >= 1; if
  //*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).
  //*
  int LDVT;
  if (!compact)
    LDVT = ncols;
  else
    LDVT = (ncols < nrows) ? ncols : nrows;
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  T WORKSIZE[2];
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 1.
  //*          LWORK >=  2*MIN(M,N)+MAX(M,N).
  //*          For good performance, LWORK should generally be larger.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  RWORK   (workspace) REAL array, dimension (5*min(M,N))
  //*          On exit, if INFO > 0, RWORK(1:MIN(M,N)-1) contains the
  //*          unconverged superdiagonal elements of an upper bidiagonal
  //*          matrix B whose diagonal is in S (not necessarily sorted).
  //*          B satisfies A = U * B * VT, so it has the same singular
  //*          values as A, and singular vectors related by U and VT.
  //*
  int minMN;
  minMN = (M < N) ? M : N;
  MemBlock<T> RWORK(5*minMN);
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit.
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
  //*          > 0:  if CBDSQR did not converge, INFO specifies how many
  //*                superdiagonals of an intermediate bidiagonal form B
  //*                did not converge to zero. See the description of RWORK
  //*                above for details.
  //*
  int INFO;
  LWORK = -1;
  Tgesvd( &JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	  WORKSZE, &LWORK, &RWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(LWORK*2);
  Tgesvd( &JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	   &WORK, &LWORK, &RWORK, &INFO);
  if (INFO > 0)
    WarningMessage(QString("svd did not converge"));
  if (INFO < 0)
    WarningMessage(QString("svd had illegal value for parameter %1").arg(-INFO));
}



