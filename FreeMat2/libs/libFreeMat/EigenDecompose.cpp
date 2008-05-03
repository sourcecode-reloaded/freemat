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
void Tsyev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
	   T *W, T *WORK, int *LWORK, int *INFO);

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	   float *W, float *WORK, int *LWORK, int *INFO) {
  return ssyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO);
}

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	   double *W, double *WORK, int *LWORK, int *INFO) {
  return dsyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO);
}


template <typename T>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, T* A, int* LDA, T* WR, T* WI,
	    T* VL, int *LDVL, T* VR, int *LDVR, int *ILO,
	    int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
	    T* RCONDV, T *WORK, int *LWORK, int *IWORK,
	    int *INFO);

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
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
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
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
void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	   T *B, int *LDB, T *ALPHAR, T *ALPHAI,
	   T *BETA, T *VL, int *LDVL, T *VR, 
	   int *LDVR, T *WORK, int *LWORK, int *INFO );

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	   float *B, int *LDB, float *ALPHAR, float *ALPHAI,
	   float *BETA, float *VL, int *LDVL, float *VR, 
	   int *LDVR, float *WORK, int *LWORK, int *INFO ) {
  return sggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
}

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	   double *B, int *LDB, double *ALPHAR, double *ALPHAI,
	   double *BETA, double *VL, int *LDVL, double *VR, 
	   int *LDVR, double *WORK, int *LWORK, int *INFO ) {
  return dggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
}

template <typename T>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
	   int *LDA, T *B, int *LDB, T *W, T *WORK,
	   int *LWORK, int *INFO );

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	   int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, int *INFO ) {
  return ssygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO);
}

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	   int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, int *INFO ) {
  return dsygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO);
}

template <typename T>
void Theev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
	   T *W, T *WORK, int *LWORK, T *RWORK, int *INFO);

template <>
void Theev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	   float *W, float *WORK, int *LWORK, float *RWORK, int *INFO) {
  return cheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO);
}

template <>
void Theev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	   double *W, double *WORK, int *LWORK, double *RWORK, int *INFO) {
  return zheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO);
}

template <typename T>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
	   int *LDA, T *B, int *LDB, T *W, T *WORK,
	   int *LWORK, T *RWORK, int *INFO );

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	   int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, float *RWORK, int *INFO ) {
  return chegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO);
}

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	   int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, double *RWORK, int *INFO ) {
  return zhegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO);
}

template <typename T>
void realEigenDecomposeSymmetric(int n, T *v, T *d, T *a,
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
void realEigenDecompose(int n, T *v, T *d, T *a, bool eigenvectors, bool balance) {
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
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORK, &LWORK, &IWORK, &INFO );
  for (int i=0;i<N;i++) {
    d[2*i] = WR[i];
    d[2*i+1] = WI[i];
  }
}

template <typename T>
void realGenEigenDecompose(int n, T *v, T *d, T *a,
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
bool realGenEigenDecomposeSymmetric(int n, T *v, T *d,
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
void complexEigenDecomposeSymmetric(int n, T *v, T *d, T *a, bool eigenvectors) {
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
void complexEigenDecompose(int n, T *v, T *d, T *a,
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
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, WORKSZE, &LWORK, &RWORK, &INFO );
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORK, &LWORK, &RWORK, &INFO );
}

template <typename T>
void complexGenEigenDecompose(int n, T *v, T *d, T *a, T *b, bool eigenvectors) {
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
bool complexGenEigenDecomposeSymmetric(int n, T *v, T *d, T *a, T *b, 
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


