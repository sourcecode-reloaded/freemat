#ifndef __BLAS_hpp__
#define __BLAS_hpp__

#include "Complex.hpp"

namespace FM
{
  void BLAS_dgemm(int Arows, int Acols, int Ccols, const double *A, const double *b, double *C);
  void BLAS_zgemm(int Arows, int Acols, int Ccols, const Complex<double> *A, const Complex<double> *b, Complex<double> *C);
  void BLAS_sgemm(int Arows, int Acols, int Ccols, const float *A, const float *b, float *C);
  void BLAS_cgemm(int Arows, int Acols, int Ccols, const Complex<float> *A, const Complex<float> *b, Complex<float> *C);

  template <class T>
  static inline void BLAS_gemm(int Arows, int Acols, int Ccols, const T*A, const T*B, T*C);
  
  template <>
  void BLAS_gemm(int Arows, int Acols, int Ccols, const double *A, const double *B, double *C) {
    return BLAS_dgemm(Arows,Acols,Ccols,A,B,C);
  }
  
  template <>
  void BLAS_gemm(int Arows, int Acols, int Ccols, const float *A, const float *B, float *C) {
    return BLAS_sgemm(Arows,Acols,Ccols,A,B,C);
  }
  
  template <>
  void BLAS_gemm(int Arows, int Acols, int Ccols, const Complex<double> *A, const Complex<double> *B, Complex<double> *C) {
    return BLAS_zgemm(Arows,Acols,Ccols,A,B,C);
  }
  
  template <>
  void BLAS_gemm(int Arows, int Acols, int Ccols, const Complex<float> *A, const Complex<float> *B, Complex<float> *C) {
    return BLAS_cgemm(Arows,Acols,Ccols,A,B,C);
  }
}

#endif
