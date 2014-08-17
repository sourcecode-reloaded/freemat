#include "BLAS.hpp"

#ifdef HAVE_ACCELERATE
#include <Accelerate.h>
#endif

#ifdef HAVE_CBLAS

void FM::BLAS_dgemm(int Arows, int Acols, int Bcols,
		    const double *A, const double *B,
		    double *C)
{
  cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,Arows,Bcols,Acols,1.0,A,Arows,B,Acols,0.0,C,Arows);
}

void FM::BLAS_zgemm(int Arows, int Acols, int Bcols,
		    const double *A, const double *B,
		    double *C)
{
  double alpha[] = {1,0};
  double beta[] = {0,0};
  cblas_zgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,Arows,Bcols,Acols,alpha,A,Arows,B,Acols,beta,C,Arows);
}

void FM::BLAS_sgemm(int Arows, int Acols, int Bcols,
		    const float *A, const float *B,
		    float *C)
{
  cblas_sgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,Arows,Bcols,Acols,1.0,A,Arows,B,Acols,0.0,C,Arows);
}

void FM::BLAS_cgemm(int Arows, int Acols, int Bcols,
		    const float *A, const float *B,
		    float *C)
{
  float alpha[] = {1,0};
  float beta[] = {0,0};
  cblas_cgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,Arows,Bcols,Acols,alpha,A,Arows,B,Acols,beta,C,Arows);
}

#endif

