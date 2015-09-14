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


namespace FM {

// #include "Math.hpp"
// #include "Algorithms.hpp"
// #include "SparseCCS.hpp"

// #if HAVE_UMFPACK
// extern "C" {
// #include "umfpack.h"
// }
// #endif

#define MAX(a,b) ((a) > (b) ? (a) : (b))

static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, Complex<float>* A, int* LDA, Complex<float>* W, Complex<float>* VL, 
		   int *LDVL, Complex<float>* VR, int *LDVR, int *ILO,
		   int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
		   float* RCONDV, Complex<float> *WORK, int *LWORK, float *RWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return cgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, Complex<double>* A, int* LDA, Complex<double>* W, Complex<double>* VL, 
		   int *LDVL, Complex<double>* VR, int *LDVR, int *ILO,
		   int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
		   double* RCONDV, Complex<double> *WORK, int *LWORK, double *RWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return zgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
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

static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
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



/*********************
 * Symmetric case    *
 *********************/

void Tsyev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	   float *W, float *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len) {
  return ssyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

void Tsyev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	   double *W, double *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len) {
  return dsyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

void Theev(char *JOBZ, char *UPLO, int *N, FM::Complex<float> *A, int *LDA, 
	   float *W, FM::Complex<float> *WORK, int *LWORK, float *RWORK, int *INFO,
	   ftnlen l1, ftnlen l2) {
  return cheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

void Theev(char *JOBZ, char *UPLO, int *N, FM::Complex<double> *A, int *LDA, 
	   double *W, FM::Complex<double> *WORK, int *LWORK, double *RWORK,
	   int *INFO, ftnlen l1, ftnlen l2) {
  return zheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <typename T>
static void eigenDecomposeSymmetric(int n, T *v, T *d, T *a,
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
static void eigenDecomposeSymmetric(int n, Complex<T> *v, T *d,
				    Complex<T> *a, bool eigenvectors) {
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
  Complex<T> WORKSZE;
  LWORK = -1;
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,&WORKSZE,&LWORK,&RWORK,&INFO,1,1);
  LWORK = (int) WORKSZE.r;
  MemBlock<Complex<T> > WORK(LWORK);
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1,1);
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(Complex<T>));
}

/**
 * Eigen decomposition, symmetric matrix, compact decomposition case
 */

template <class dataclass>
static void TEigenDecomposeCompactSymmetric(Object &a, Object &ret, dataclass _data) {
  auto _eigen = _data->realType();
  ret = _eigen->makeMatrix(a.cols(),1);
  eigenDecomposeSymmetric(a.irows(),_data->null(),_eigen->rw(ret),_data->rw(a),false);
}

Object EigenDecomposeCompactSymmetric(const Object &a, ThreadContext *ctxt) {
  if (a.rows() != a.cols()) throw Exception("A symmetric matrix must be square");
  Object ret(ctxt);
  Object acopy(a);
  switch (a.typeCode()) {
  case TypeDouble:
    TEigenDecomposeCompactSymmetric(acopy,ret,ctxt->_double);
    break;
  case TypeSingle:
    TEigenDecomposeCompactSymmetric(acopy,ret,ctxt->_single);
    break;
  case TypeZDouble:
    TEigenDecomposeCompactSymmetric(acopy,ret,ctxt->_zdouble);
    break;
  case TypeZSingle:
    TEigenDecomposeCompactSymmetric(acopy,ret,ctxt->_zsingle);
    break;
  default:
    throw Exception("Unhandled type " + a.type()->name() + " in eigen decomposition");
  }
  return ret;
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */

template <class dataclass>
static void TEigenDecomposeFullSymmetric(Object &a, Object &V, Object &D, dataclass _data) {
  auto _eigen = _data->realType();
  V = _data->makeMatrix(a.rows(),a.cols());
  Object eigenvals(_eigen->makeMatrix(a.rows(),1));
  eigenDecomposeSymmetric(a.irows(),_data->rw(V),_eigen->rw(eigenvals),_data->rw(a),true);
  D = _eigen->asDiagonalMatrix(eigenvals);
}

void EigenDecomposeFullSymmetric(const Object &a, Object &V, Object &D, ThreadContext *ctxt) {
  if (a.rows() != a.cols()) throw Exception("A symmetric matrix must be square");
  Object acopy(a);
  switch (a.typeCode()) {
  case TypeDouble:
    TEigenDecomposeFullSymmetric(acopy,V,D,ctxt->_double);
    break;
  case TypeSingle:
    TEigenDecomposeFullSymmetric(acopy,V,D,ctxt->_single);
    break;
  case TypeZDouble:
    TEigenDecomposeFullSymmetric(acopy,V,D,ctxt->_zdouble);
    break;
  case TypeZSingle:
    TEigenDecomposeFullSymmetric(acopy,V,D,ctxt->_zsingle);
    break;
  default: 
    throw Exception("Unhandled type " + a.type()->name() + " in eigen decomposition");
 }
}

template <typename T>
static void eigenDecompose(int n, T *v, Complex<T> *d, T *a, 
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
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORKSZE, &LWORK, &IWORK, &INFO,1,1,1,1 );

  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORK, &LWORK, &IWORK, &INFO,1,1,1,1 );
  for (int i=0;i<N;i++) 
    d[i] = Complex<T>(WR[i],WI[i]);
}


template <typename T>
static void eigenDecompose(int n, Complex<T> *v, Complex<T> *d, Complex<T> *a,
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
  Complex<T> *Ain = a;
  int LDA = n;
  Complex<T> *W = d;
  Complex<T> *VL = nullptr;
  int LDVL = 1;
  Complex<T> *VR = v;
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
  FM::Complex<T> WORKSZE;
  LWORK = -1;
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORKSZE, &LWORK, &RWORK, &INFO,1,1,1,1 );
  LWORK = (int) WORKSZE.r;
  MemBlock<Complex<T> > WORK(2*LWORK);
  Tgeevx( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	  VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	  &RCONDE, &RCONDV, &WORK, &LWORK, &RWORK, &INFO,1,1,1,1 );
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <class T>
void TEigenDecomposeCompact(Object &a, Object &ret, bool balanceFlag, T _data) {
  auto _complex = _data->complexType();
  ret = _complex->makeMatrix(a.rows(),1);
  eigenDecompose(a.irows(),_data->null(),_complex->rw(ret),_data->rw(a),false,balanceFlag);
}

Object EigenDecomposeCompact(const Object &a, bool balanceFlag, ThreadContext *ctxt) {
  if (a.rows() != a.cols()) throw Exception("Cannot handle non-square matrices in eigen-decomposition");
  Object ret(ctxt);
  Object acopy(a);
  switch (a.typeCode()) {
  case TypeDouble:
    TEigenDecomposeCompact(acopy,ret,balanceFlag,ctxt->_double);
    break;
  case TypeSingle:
    TEigenDecomposeCompact(acopy,ret,balanceFlag,ctxt->_single);
    break;
  case TypeZDouble:
    TEigenDecomposeCompact(acopy,ret,balanceFlag,ctxt->_zdouble);
    break;
  case TypeZSingle:
    TEigenDecomposeCompact(acopy,ret,balanceFlag,ctxt->_zsingle);
    break;
  default:
    throw Exception("Unsupported type " + a.type()->name() + " for eigen decomposition");
  }
  return ret;
}

template <class T>
void TEigenDecomposeFull(Object &a, Object &V, Object &D, bool balanceFlag, T _data) {
  auto _complex = _data->complexType();
  Object eigenvals(_complex->makeMatrix(a.rows(),1));
  eigenDecompose(a.irows(),_data->rw(V),_complex->rw(eigenvals),_data->rw(a),true,balanceFlag);
  D = _complex->asDiagonalMatrix(eigenvals);
}

void EigenDecomposeFull(const Object &A, Object &V, Object &D, bool balanceFlag, ThreadContext *ctxt) {
  if (A.rows() != A.cols()) throw Exception("Matrix must be square");
  Object acopy(A);
  switch (A.typeCode()) {
  case TypeDouble:
    TEigenDecomposeFull(acopy,V,D,balanceFlag,ctxt->_double);
    break;
  case TypeSingle:
    TEigenDecomposeFull(acopy,V,D,balanceFlag,ctxt->_single);
    break;
  case TypeZDouble:
    TEigenDecomposeFull(acopy,V,D,balanceFlag,ctxt->_zdouble);
    break;
  case TypeZSingle:
    TEigenDecomposeFull(acopy,V,D,balanceFlag,ctxt->_zsingle);
    break;
  default:
    throw Exception("Unhandled type " + A.type()->name() + " in eigen decomposition");
  }    
}

}
