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
#include "FloatType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"


using namespace FM;

// #include "Math.hpp"
// #include "Algorithms.hpp"
// #include "SparseCCS.hpp"

// #if HAVE_UMFPACK
// extern "C" {
// #include "umfpack.h"
// }
// #endif

#define MAX(a,b) ((a) > (b) ? (a) : (b))

template <typename T>
void Tsyev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
	   T *W, T *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len);

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	   float *W, float *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len) {
  return ssyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	   double *W, double *WORK, int *LWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len) {
  return dsyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

template <typename T>
static void Tgeevx(char * BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, T* A, int* LDA, T* W, T* VL, 
		   int *LDVL, T* VR, int *LDVR, int *ILO,
		   int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
		   T* RCONDV, T *WORK, int *LWORK, T *RWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, float* A, int* LDA, float* W, float* VL, 
	    int *LDVL, float* VR, int *LDVR, int *ILO,
	    int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	    float* RCONDV, float *WORK, int *LWORK, float *RWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return cgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, double* A, int* LDA, double* W, double* VL, 
	    int *LDVL, double* VR, int *LDVR, int *ILO,
	    int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	    double* RCONDV, double *WORK, int *LWORK, double *RWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return zgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

template <typename T>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, T* A, int* LDA, T* WR, T* WI,
		   T* VL, int *LDVL, T* VR, int *LDVR, int *ILO,
		   int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
		   T* RCONDV, T *WORK, int *LWORK, int *IWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, float* A, int* LDA, float* WR, float* WI,
	    float* VL, int *LDVL, float* VR, int *LDVR, int *ILO,
	    int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	    float* RCONDV, float *WORK, int *LWORK, int *IWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return sgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO, 
		 balanc_len, jobvl_len, jobvr_len, sense_len);
}

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, double* A, int* LDA, double* WR, double* WI,
	    double* VL, int *LDVL, double* VR, int *LDVR, int *ILO,
	    int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	    double* RCONDV, double *WORK, int *LWORK, int *IWORK,
	    int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return dgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO, 
		 balanc_len, jobvl_len, jobvr_len, sense_len);
}

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
static void Theev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
		  T *W, T *WORK, int *LWORK, T *RWORK, int *INFO, ftnlen l1, ftnlen l2);

template <>
void Theev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
		  float *W, float *WORK, int *LWORK, float *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  return cheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Theev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
		  double *W, double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  return zheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <typename T>
static void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
	   int *LWORK, T *RWORK, int *INFO , ftnlen l1, ftnlen l2);

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
		  int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, float *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return chegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
		  int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, double *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return zhegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
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
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORKSZE, &LWORK, &INFO, 1, 1);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORK, &LWORK, &INFO, 1, 1);
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
  T *VL = nullptr;
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
  T WORKSZE;
  MemBlock<int> IWORK(2*n-2);
  int INFO;

  LWORK = -1;
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORKSZE, &LWORK, &IWORK, &INFO,1,1,1,1 );

  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &IWORK, &INFO,1,1,1,1 );
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
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO,1,1);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1,1);
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
  T *VL = nullptr;
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
	     &RCONDE, &RCONDV, WORKSZE, &LWORK, &RWORK, &INFO,1,1,1,1 );
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &RWORK, &INFO,1,1,1,1 );
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

/**
 * Eigen decomposition, symmetric matrix, compact decomposition case
 */
template <typename T, FM::DataCode codeNum>
static void TEigenDecomposeCompactSymmetric(Object &D, Object A, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  if (!A.isComplex()) {
    D = ft->makeMatrix(N,1);
    realEigenDecomposeSymmetric(int(N), (T*) nullptr, ft->rw(D), 
				ft->rw(A),false);
  } else {
    D = ft->makeMatrix(N,1);
    complexEigenDecomposeSymmetric(int(N), (T*) nullptr, ft->rw(D),
				   reinterpret_cast<T*>(ft->rwComplex(A)),false);
  }
}

void FM::EigenDecomposeCompactSymmetric(Object A, Object& D, ThreadContext *ctxt) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Create one square matrix to store the eigenvectors
  switch (A.typeCode()) {
  default: 
    throw Exception("Unhandled type for symmetric eigendecomposition");
  case TypeSingle:
    TEigenDecomposeCompactSymmetric<float,TypeSingle>(D,A,ctxt->_single);
    break;
  case TypeDouble:
    TEigenDecomposeCompactSymmetric<double,TypeDouble>(D,A,ctxt->_double);
    break;
  }
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */
template <typename T, FM::DataCode codeNum>
static void TEigenDecomposeFullSymmetric(Object &V, Object &D, Object A, FloatType<T,codeNum> *ft) {
  ndx_t N = A.rows();
  if (!A.isComplex()) {
    Object eigenvals = ft->makeMatrix(N,1);
    V = ft->makeMatrix(N,N);
    realEigenDecomposeSymmetric(int(N), ft->rw(V),
				ft->rw(eigenvals), 
				ft->rw(A),
				true);
    D = ft->asDiagonalMatrix(eigenvals);
  } else {
    Object eigenvals = ft->makeMatrix(N,1);
    V = ft->makeMatrixComplex(N,N);
    complexEigenDecomposeSymmetric(int(N), ft->rw(V),
				   ft->rw(eigenvals),
				   ft->rw(A),
				   true);
    D = ft->asDiagonalMatrix(eigenvals);
  }
}

void EigenDecomposeFullSymmetric(const Object &A, Object& V, Object& D, ThreadContext *ctxt) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.typeCode()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case TypeSingle:
    TEigenDecomposeFullSymmetric<float,TypeSingle>(V,D,A,ctxt->_single);
    break;
  case TypeDouble:
    TEigenDecomposeFullSymmetric<double,TypeDouble>(V,D,A,ctxt->_double);
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

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

void EigenDecomposeFullGeneral(const Object &A, Object& V, Object& D, bool balanceFlag, ThreadContext *ctxt) {
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

void EigenDecomposeCompactGeneral(Object A, Object& D, bool balanceFlag, ThreadContext *ctxt) {
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

