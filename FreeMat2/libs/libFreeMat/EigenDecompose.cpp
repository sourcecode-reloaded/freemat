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

#include "EigenDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "MemPtr.hpp"
#include "Complex.hpp"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

template <typename T>
static void Tsyev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
		  T *W, T *WORK, int *LWORK, int *INFO);

template <>
static void Tsyev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
		  float *W, float *WORK, int *LWORK, int *INFO) {
  return ssyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO);
}

template <>
static void Tsyev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
		  double *W, double *WORK, int *LWORK, int *INFO) {
  return dsyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO);
}

template <typename T>
static void Tgeevx(char * BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, T* A, int* LDA, T* W, T* VL, 
	       int *LDVL, T* VR, int *LDVR, int *ILO,
	       int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
	       T* RCONDV, T *WORK, int *LWORK, T *RWORK,
	       int *INFO);

template <>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, float* A, int* LDA, float* W, float* VL, 
		   int *LDVL, float* VR, int *LDVR, int *ILO,
		   int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
		   float* RCONDV, float *WORK, int *LWORK, float *RWORK,
		   int *INFO) {
  return cgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO );
}

template <>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, double* A, int* LDA, double* W, double* VL, 
		   int *LDVL, double* VR, int *LDVR, int *ILO,
		   int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
		   double* RCONDV, double *WORK, int *LWORK, double *RWORK,
		   int *INFO) {
  return zgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO );
}

template <typename T>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, T* A, int* LDA, T* WR, T* WI,
		   T* VL, int *LDVL, T* VR, int *LDVR, int *ILO,
		   int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
		   T* RCONDV, T *WORK, int *LWORK, int *IWORK,
		   int *INFO);

template <>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, float* A, int* LDA, float* WR, float* WI,
		   float* VL, int *LDVL, float* VR, int *LDVR, int *ILO,
		   int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
		   float* RCONDV, float *WORK, int *LWORK, int *IWORK,
		   int *INFO) {
  return sgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO);
}

template <>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, double* A, int* LDA, double* WR, double* WI,
		   double* VL, int *LDVL, double* VR, int *LDVR, int *ILO,
		   int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
		   double* RCONDV, double *WORK, int *LWORK, int *IWORK,
		   int *INFO) {
  return dgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO);
}

template <typename T>
static void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
		  T *B, int *LDB, T *ALPHAR, T *ALPHAI,
		  T *BETA, T *VL, int *LDVL, T *VR, 
		  int *LDVR, T *WORK, int *LWORK, int *INFO );

template <>
static void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
		  float *B, int *LDB, float *ALPHAR, float *ALPHAI,
		  float *BETA, float *VL, int *LDVL, float *VR, 
		  int *LDVR, float *WORK, int *LWORK, int *INFO ) {
  return sggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
}

template <>
static void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
		  double *B, int *LDB, double *ALPHAR, double *ALPHAI,
		  double *BETA, double *VL, int *LDVL, double *VR, 
		  int *LDVR, double *WORK, int *LWORK, int *INFO ) {
  return dggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
}

template <typename T>
static void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	      T *B, int *LDB, T *ALPHA, T *BETA, 
	      T *VL, int *LDVL, T *VR, int *LDVR, 
	      T *WORK, int *LWORK, T *RWORK, int *INFO );

template <>
static void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
		  float *B, int *LDB, float *ALPHA, float *BETA, 
		  float *VL, int *LDVL, float *VR, int *LDVR, 
		  float *WORK, int *LWORK, float *RWORK, int *INFO ) {
  return cggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO);
}

template <typename T>
static void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
		  double *B, int *LDB, double *ALPHA, double *BETA, 
		  double *VL, int *LDVL, double *VR, int *LDVR, 
		  double *WORK, int *LWORK, double *RWORK, int *INFO ) {
  return zggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO);  
}

template <typename T>
static void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
		  int *LWORK, int *INFO );

template <>
static void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
		  int *LDA, float *B, int *LDB, float *W, float *WORK,
		  int *LWORK, int *INFO ) {
  return ssygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO);
}

template <>
static void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
		  int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, int *INFO ) {
  return dsygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO);
}

template <typename T>
static void Theev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
		  T *W, T *WORK, int *LWORK, T *RWORK, int *INFO);

template <>
static void Theev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
		  float *W, float *WORK, int *LWORK, float *RWORK, int *INFO) {
  return cheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO);
}

template <>
static void Theev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
		  double *W, double *WORK, int *LWORK, double *RWORK, int *INFO) {
  return zheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO);
}

template <typename T>
static void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
	   int *LWORK, T *RWORK, int *INFO );

template <>
static void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
		  int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, float *RWORK, int *INFO ) {
  return chegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO);
}

template <>
static void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
		  int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, double *RWORK, int *INFO ) {
  return zhegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO);
}

template <typename T>
static void realEigenDecomposeSymmetric(int n, T *v, T *d, T *a,
					bool eigenvectors) {
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *Ain = a;
  int LDA = n;
  int INFO;
  T WORKSZE;
  int LWORK;
  LWORK = -1;
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORK, &LWORK, &INFO);
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(T));
}

template <typename T>
static void realEigenDecompose(int n, T *v, T *d, T *a, 
			       bool eigenvectors, bool balance) {
  char BALANC;
  if (balance)
    BALANC = 'B';
  else
    BALANC = 'N';
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  char SENSE = 'N';
  int N = n;
  T *Ain = a;
  int LDA = n;
  MemBlock<T> WR(n);
  MemBlock<T> WI(n);
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  int ILO;
  int IHI;
  MemBlock<T> SCALE(n);
  T ABNRM;
  MemBlock<T> RCONDE(n);
  MemBlock<T> RCONDV(n);
  int maxN = (N < 6) ? 6 : N;
  int LWORK = maxN*maxN*2;
  MemBlock<int> IWORK(2*n-2);
  int INFO;
  MemBlock<T> WORK(LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &IWORK, &INFO );
  for (int i=0;i<N;i++) {
    d[2*i] = WR[i];
    d[2*i+1] = WI[i];
  }
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
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  T WORKSZE;
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORKSZE, &LWORK, &INFO );
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &INFO );
  int i;
  for (i=0;i<n;i++) {
    d[2*i] = ALPHAR[i]/BETA[i];
    d[2*i+1] = ALPHAI[i]/BETA[i];
  }
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
	 &LWORK, &INFO );
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tsygv( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORK, 
	 &LWORK, &INFO );
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(T));
  return true;
}

template <typename T>
static void complexEigenDecomposeSymmetric(int n, T *v, T *d, T *a, 
					   bool eigenvectors) {
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  int LDA = n;
  MemBlock<T> RWORK(MAX(1,3*N-2));
  int LWORK;
  int INFO;
  T WORKSZE[2];
  LWORK = -1;
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO);
  if (eigenvectors)
    memcpy(v,a,2*n*n*sizeof(T));
}

template <typename T>
static void complexEigenDecompose(int n, T *v, T *d, T *a,
				  bool eigenvectors, bool balance) {
  char BALANC;
  if (balance)
    BALANC = 'B';
  else
    BALANC = 'N';
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  char SENSE = 'N';
  int N = n;
  T *Ain = a;
  int LDA = n;
  T *W = d;
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  int ILO;
  int IHI;
  MemBlock<T> SCALE(n);
  T ABNRM;
  MemBlock<T> RCONDE(n);
  MemBlock<T> RCONDV(n);
  int LWORK;
  MemBlock<T> RWORK(2*n);
  int INFO;
  T WORKSZE[2];
  LWORK = -1;
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, WORKSZE, &LWORK, &RWORK, &INFO );
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &RWORK, &INFO );
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
  T *VL = NULL;
  int LDVL = n;
  T *VR = v;
  int LDVR = n;
  MemBlock<T> RWORK(8*n);
  T WORKSIZE[2];
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORKSIZE[0], &LWORK, &RWORK, &INFO );
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK*2);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &RWORK, &INFO );
  int i;
  for (i=0;i<n;i++) 
    complex_divide<T>(ALPHA[2*i],ALPHA[2*i+1],
		      BETA[2*i],BETA[2*i+1],
		      d[2*i],d[2*i+1]);
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
	&LWORK, &RWORK, &INFO );    
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,2*n*n*sizeof(T));
  return true;

}

/**
 * Eigen decomposition, symmetric matrix, compact decomposition case
 */
template <typename T>
static void EigenDecomposeCompactSymmetric(Array &D, Array A) {
  index_t N = A.rows();
  NTuple Vdims(N,1);
  if (A.allReal()) {
    D = Array(GetDataClass<T>(),Vdims);
    realEigenDecomposeSymmetric(int(N), (T*) NULL, D.real<T>().data(), 
				A.real<T>().data(),false);
  } else {
    D = Array(GetDataClass<T>(),Vdims);
    complexEigenDecomposeSymmetric(int(N), (T*) NULL, D.real<T>().data(),
				   A.fortran<T>().data(),false);
  }
}

void EigenDecomposeCompactSymmetric(Array A, Array& D) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Create one square matrix to store the eigenvectors
  switch (A.dataClass()) {
  default: 
    throw Exception("Unhandled type for symmetric eigendecomposition");
  case Float:
    EigenDecomposeCompactSymmetric<float>(D,A);
    break;
  case Double:
    EigenDecomposeCompactSymmetric<double>(D,A);
    break;
  }
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */
template <typename T>
static inline void EigenDecomposeFullSymmetric(Array &V, Array &D, Array A) {
  index_t N = A.rows();
  NTuple Vdims(N,N);
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(N,1));
    V = Array(GetDataClass<T>(),Vdims);
    realEigenDecomposeSymmetric(int(N), V.real<T>().data(), 
				eigenvals.data(), 
				A.real<T>().data(),
				true);
    D = DiagonalArray(eigenvals);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    V = Array(GetDataClass<T>(),Vdims);
    complexEigenDecomposeSymmetric(int(N), V.real<T>().data(),
				   eigenvals.data(),
				   A.fortran<T>().data(),
				   true);
    D = DiagonalArray(eigenvals);
  }
}

void EigenDecomposeFullSymmetric(Array A, Array& V, Array& D) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    EigenDecomposeFullSymmetric<float>(V,D,A);
    break;
  case Double:
    EigenDecomposeFullSymmetric<double>(V,D,A);
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <typename T>
static void HandleEigenVectorsRealMatrix(BasicArray<T> &eigenvals,
					 BasicArray<T> &Vp, 
					 index_t N, Array &D, Array &V) {
  // Make a pass through the eigenvals, and look for complex eigenvalues.
  bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
  if (!complexEigenvalues) {
    D = DiagonalArray(SplitReal<T>(eigenvals));
    V = Array(Vp);
  } else {
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(GetDataClass<T>(),NTuple(N,N));
    index_t i = 1;
    while (i <= N) {
      if ((i < N) && (eigenvals[2*i-1] != 0)) {
	for (index_t j=1;j<=N;j++) {
	  V.set(NTuple(j,i),Array(Vp.get(NTuple(j,i)),Vp.get(NTuple(j,i+1))));
	  V.set(NTuple(j,i+1),Array(Vp.get(NTuple(j,i)),-Vp.get(NTuple(j,i+1))));
	}
	i += 2;
      } else {
	for (index_t j=1;j<=N;j++) 
	  V.set(NTuple(j,i),Array(Vp.get(NTuple(j,i))));
	i++;
      }
    }
  }
}

template <typename T>
static void EigenDecomposeFullGeneral(Array A, Array &V, Array &D,
				      bool balanceFlag) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    // A temporary vector to store the eigenvalues
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    BasicArray<T> Vp(NTuple(N,N));
    realEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
		       A.real<T>().data(), true, balanceFlag);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V);
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    complexEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			  A.fortran<T>().data(), true, balanceFlag);
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
}

void EigenDecomposeFullGeneral(Array A, Array& V, Array& D, bool balanceFlag) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float: 
    EigenDecomposeFullGeneral<float>(A,V,D,balanceFlag);
    return;
  case Double:
    EigenDecomposeFullGeneral<double>(A,V,D,balanceFlag);
    return;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T>
static void EigenDecomposeCompactGeneral(Array A, Array& D, 
					 bool balanceFlag) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    realEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
		       A.real<T>().data(),
		       false, balanceFlag);
    bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
    if (!complexEigenvalues) {
      D = Array(SplitReal<T>(eigenvals));
    } else {
      D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    }
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    complexEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
			  A.fortran<T>().data(),
			  false, balanceFlag);
    D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
  }
}

void EigenDecomposeCompactGeneral(Array A, Array& D, bool balanceFlag) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    EigenDecomposeCompactGeneral<float>(A,D,balanceFlag);
    return;
  case Double:
    EigenDecomposeCompactGeneral<double>(A,D,balanceFlag);
    return;
  }
}

template <typename T>
static bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, 
						      Array& D) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(N,1));
    if (!realGenEigenDecomposeSymmetric(int(N), (T*) NULL, eigenvals.data(), 
					A.real<T>().data(),
					B.real<T>().data(),false)) {
      return false;
    }
    D = Array(eigenvals);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    if (!complexGenEigenDecomposeSymmetric(int(N), (T*) NULL, eigenvals.data(), 
					   A.fortran<T>().data(),
					   B.fortran<T>().data(),
					   false)) {
      return false;
    }
    D = Array(eigenvals);
  }
  return true;
}

bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, 
						      Array& D) {
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeCompactSymmetric<float>(A,B,D);
  case Double:
    return GeneralizedEigenDecomposeCompactSymmetric<double>(A,B,D);
  }
  return false;
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */

template <typename T>
static bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, 
						   Array &V, Array &D) {
  index_t N = A.rows();
  if (A.allReal()) {
      // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(N,1));
    BasicArray<T> Vp(NTuple(N,N));
    if (!realGenEigenDecomposeSymmetric(int(N), Vp.data(), eigenvals.data(), 
					A.real<T>().data(),
					B.real<T>().data(),true))
      return false;
    D = DiagonalArray(eigenvals);
    V = Array(Vp);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    if (!complexGenEigenDecomposeSymmetric(int(N), Vp.data(), eigenvals.data(), 
					   A.fortran<T>().data(),
					   B.fortran<T>().data(),true))
      return false;
    D = DiagonalArray(eigenvals);
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
  return true;
}

bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array& V, Array& D) {
  switch (A.dataClass()) {
  default:
    throw Exception("Unsupported type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeFullSymmetric<float>(A,B,V,D);
  case Double:
    return GeneralizedEigenDecomposeFullSymmetric<double>(A,B,V,D);
  }
  return false;
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <typename T>
static void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, 
						 Array &V, Array &D) {
  index_t N = A.rows();
  if (A.allReal()) {
    // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(2*N,1));
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    BasicArray<T> Vp(NTuple(N*N,1));
    realGenEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			  A.real<T>().data(),B.real<T>().data(),true);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V);
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    complexGenEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			     A.fortran<T>().data(),
			     B.fortran<T>().data(),true);
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
}

void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array& V, Array& D) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeFullGeneral<float>(A,B,V,D);
  case Double:
    return GeneralizedEigenDecomposeFullGeneral<double>(A,B,V,D);
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T>
static void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D) {
  index_t N = A.rows();
  if (A.allReal()) {
    // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(2*N,1));
    realGenEigenDecompose(int(N), (T*) NULL, eigenvals.data(),
			  A.real<T>().data(),
			  B.real<T>().data(),false);
    // Make a pass through the eigenvals, and look for complex eigenvalues.
    bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
    if (!complexEigenvalues) {
      D = Array(SplitReal<T>(eigenvals));
    } else {
      // Copy the eigenvalues into a complex vector
      D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    }
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    complexGenEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
			     A.fortran<T>().data(), 
			     B.fortran<T>().data(),
			     false);
    D = Array(SplitReal(eigenvals),SplitImag(eigenvals));
  }
}

void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: 
    throw Exception("Unhandled type for eigendecomposition");
  case Float: 
    GeneralizedEigenDecomposeCompactGeneral<float>(A,B,D);
    break;
  case Double:
    GeneralizedEigenDecomposeCompactGeneral<double>(A,B,D);
    break;
  }
}



