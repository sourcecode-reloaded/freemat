#ifndef __BLAS_hpp__
#define __BLAS_hpp__

namespace FM
{
  void BLAS_dgemm(int Arows, int Acols, int Ccols, const double *A, const double *b, double *C);
  void BLAS_zgemm(int Arows, int Acols, int Ccols, const double *A, const double *b, double *C);
  void BLAS_sgemm(int Arows, int Acols, int Ccols, const float *A, const float *b, float *C);
  void BLAS_cgemm(int Arows, int Acols, int Ccols, const float *A, const float *b, float *C);
}

#endif
