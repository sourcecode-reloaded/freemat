//This is a generated file. Do not edit!
#include "blas_wrap.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QLibrary> 

BlasWrapper wrapper;

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */ 
typedef void (* p_caxpy )(int n, complex *alpha, complex *x, int incx, complex *y, int incy);
typedef void (* p_ccopy )(int n, complex *x, int incx, complex *y, int incy);
typedef complex (* p_cdotc )(int n, complex *x, int incx, complex *y, int incy);
typedef complex (* p_cdotu )(int n, complex *x, int incx, complex *y, int incy);
typedef void (* p_cgbmv )(char transa, int m, int n, int nsub, int nsuper, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy);
typedef void (* p_cgemm )(char transa, char transb, int m, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc);
typedef void (* p_cgemv )(char transa, int m, int n, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy);
typedef void (* p_cgerc )(int m, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda);
typedef void (* p_cgeru )(int m, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda);
typedef void (* p_chbmv )(char uplo, int n, int ndiag, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy);
typedef void (* p_chemm )(char side, char uplo, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc);
typedef void (* p_chemv )(char uplo, int n, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy);
typedef void (* p_cher )(char uplo, int n, float alpha, complex *x, int incx, complex *a, int lda);
typedef void (* p_cher2 )(char uplo, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda);
typedef void (* p_cher2k )(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, float beta, complex *c, int ldc);
typedef void (* p_cherk )(char uplo, char transa, int n, int k, float alpha, complex *a, int lda, float beta, complex *c, int ldc);
typedef void (* p_chpmv )(char uplo, int n, complex *alpha, complex *a, complex *x, int incx, complex *beta, complex *y, int incy);
typedef void (* p_chpr )(char uplo, int n, float alpha, complex *x, int incx, complex *a);
typedef void (* p_chpr2 )(char uplo, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a);
typedef void (* p_classq )(int n, complex *x, int incx, float *scale, float *sumsq);
typedef void (* p_crotg )(complex *a, complex *b, float *c, complex *s);
typedef void (* p_cscal )(int n, complex *alpha, complex *y, int incy);
typedef void (* p_csrot )(int n, complex *x, int incx, complex *y, int incy, float c, float s);
typedef void (* p_csscal )(int n, float alpha, complex *y, int incy);
typedef void (* p_cstemr )(char jobz, char range, int n, float *d, float *e, float vl, float vu, int il, int iu, int *m, float *w, complex *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info);
typedef void (* p_cswap )(int n, complex *x, int incx, complex *y, int incy);
typedef void (* p_csymm )(char side, char uplo, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc);
typedef void (* p_csyr2k )(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc);
typedef void (* p_csyrk )(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *beta, complex *c, int ldc);
typedef void (* p_ctbmv )(char uplo, char transa, char diag, int n, int ndiag, complex *a, int lda, complex *y, int incy);
typedef void (* p_ctbsv )(char uplo, char transa, char diag, int n, int ndiag, complex *a, int lda, complex *y, int incy);
typedef void (* p_ctpmv )(char uplo, char transa, char diag, int n, complex *a, complex *y, int incy);
typedef void (* p_ctpsv )(char uplo, char transa, char diag, int n, complex *a, complex *y, int incy);
typedef void (* p_ctrmm )(char side, char uplo, char transa, char diag, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb);
typedef void (* p_ctrmv )(char uplo, char transa, char diag, int n, complex *a, int lda, complex *y, int incy);
typedef void (* p_ctrsm )(char side, char uplo, char transa, char diag, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb);
typedef void (* p_ctrsv )(char uplo, char transa, char diag, int n, complex *a, int lda, complex *y, int incy);
typedef double (* p_dasum )(int n, double *x, int incx);
typedef void (* p_daxpy )(int n, double alpha, double *x, int incx, double *y, int incy);
typedef void (* p_dcopy )(int n, double *x, int incx, double *y, int incy);
typedef double (* p_ddot )(int n, double *x, int incx, double *y, int incy);
typedef void (* p_dgbmv )(char transa, int m, int n, int nsub, int nsuper, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy);
typedef void (* p_dgemm )(char transa, char transb, int m, int n, int k, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc);
typedef void (* p_dgemv )(char transa, int m, int n, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy);
typedef void (* p_dger )(int m, int n, double alpha, double *x, int incx, double *y, int incy, double *a, int lda);
typedef double (* p_dnrm2 )(int n, double *x, int incx);
typedef void (* p_drot )(int n, double *x, int incx, double *y, int incy, double c, double s);
typedef void (* p_drotg )(double *a, double *b, double *c, double *s);
typedef void (* p_drotm )(int n, double *x, int incx, double *y, int incy, double *param);
typedef void (* p_drotmg )(double d1, double d2, double b1, double b2, double *param);
typedef void (* p_dsbmv )(char uplo, int n, int ndiag, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy);
typedef void (* p_dscal )(int n, double alpha, double *y, int incy);
typedef double (* p_dsdot )(int n, float *x, int incx, float *y, int incy);
typedef void (* p_dspmv )(char uplo, int n, double alpha, double *a, double *x, int incx, double beta, double *y, int incy);
typedef void (* p_dspr )(char uplo, int n, double alpha, double *x, int incx, double *a);
typedef void (* p_dspr2 )(char uplo, int n, double alpha, double *x, int incx, double *y, int incy, double *a);
typedef void (* p_dswap )(int n, double *x, int incx, double *y, int incy);
typedef void (* p_dsymm )(char side, char uplo, int m, int n, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc);
typedef void (* p_dsymv )(char uplo, int n, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy);
typedef void (* p_dsyr )(char uplo, int n, double alpha, double *x, int incx, double *a, int lda);
typedef void (* p_dsyr2 )(char uplo, int n, double alpha, double *x, int incx, double *y, int incy, double *a, int lda);
typedef void (* p_dsyr2k )(char uplo, char transa, int n, int k, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc);
typedef void (* p_dsyrk )(char uplo, char transa, int n, int k, double alpha, double *a, int lda, double beta, double *c, int ldc);
typedef void (* p_dtbmv )(char uplo, char transa, char diag, int n, int ndiag, double *a, int lda, double *y, int incy);
typedef void (* p_dtbsv )(char uplo, char transa, char diag, int n, int ndiag, double *a, int lda, double *y, int incy);
typedef void (* p_dtpmv )(char uplo, char transa, char diag, int n, double *a, double *y, int incy);
typedef void (* p_dtpsv )(char uplo, char transa, char diag, int n, double *a, double *y, int incy);
typedef void (* p_dtrmm )(char side, char uplo, char transa, char diag, int m, int n, double alpha, double *a, int lda, double *b, int ldb);
typedef void (* p_dtrmv )(char uplo, char transa, char diag, int n, double *a, int lda, double *y, int incy);
typedef void (* p_dtrsm )(char side, char uplo, char transa, char diag, int m, int n, double alpha, double *a, int lda, double *b, int ldb);
typedef void (* p_dtrsv )(char uplo, char transa, char diag, int n, double *a, int lda, double *y, int incy);
typedef double (* p_dzasum )(int n, doublecomplex *x, int incx);
typedef double (* p_dznrm2 )(int n, doublecomplex *x, int incx);
typedef int (* p_icamax )(int n, complex *x, int incx);
typedef int (* p_idamax )(int n, double *x, int incx);
typedef int (* p_isamax )(int n, float *x, int incx);
typedef int (* p_izamax )(int n, doublecomplex *x, int incx);
typedef int (* p_lsame )(char ca, char cb);
typedef float (* p_sasum )(int n, float *x, int incx);
typedef void (* p_saxpy )(int n, float alpha, float *x, int incx, float *y, int incy);
typedef float (* p_scasum )(int n, complex *x, int incx);
typedef float (* p_scnrm2 )(int n, complex *x, int incx);
typedef void (* p_scopy )(int n, float *x, int incx, float *y, int incy);
typedef float (* p_sdot )(int n, float *x, int incx, float *y, int incy);
typedef float (* p_sdsdot )(int n, float sb, float *sx, int incx, float *sy, int incy);
typedef void (* p_sgbmv )(char transa, int m, int n, int nsub, int nsuper, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy);
typedef void (* p_sgemm )(char transa, char transb, int m, int n, int k, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc);
typedef void (* p_sgemv )(char transa, int m, int n, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy);
typedef void (* p_sger )(int m, int n, float alpha, float *x, int incx, float *y, int incy, float *a, int lda);
typedef float (* p_snrm2 )(int n, float *x, int incx);
typedef void (* p_srot )(int n, float *x, int incx, float *y, int incy, float c, float s);
typedef void (* p_srotg )(float *a, float *b, float *c, float *s);
typedef void (* p_srotm )(int n, float *x, int incx, float *y, int incy, float *param);
typedef void (* p_srotmg )(float d1, float d2, float b1, float b2, float *param);
typedef void (* p_ssbmv )(char uplo, int n, int ndiag, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy);
typedef void (* p_sscal )(int n, float alpha, float *x, int incx);
typedef void (* p_sspmv )(char uplo, int n, float alpha, float *a, float *x, int incx, float beta, float *y, int incy);
typedef void (* p_sspr )(char uplo, int n, float alpha, float *x, int incx, float *a);
typedef void (* p_sspr2 )(char uplo, int n, float alpha, float *x, int incx, float *y, int incy, float *a);
typedef void (* p_sstemr )(char jobz, char range, int n, float *d, float *e, float vl, float vu, int il, int iu, int *m, float *w, float *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info);
typedef void (* p_sswap )(int n, float *x, int incx, float *y, int incy);
typedef void (* p_ssymm )(char side, char uplo, int m, int n, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc);
typedef void (* p_ssymv )(char uplo, int n, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy);
typedef void (* p_ssyr )(char uplo, int n, float alpha, float *x, int incx, float *a, int lda);
typedef void (* p_ssyr2 )(char uplo, int n, float alpha, float *x, int incx, float *y, int incy, float *a, int lda);
typedef void (* p_ssyr2k )(char uplo, char transa, int n, int k, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc);
typedef void (* p_ssyrk )(char uplo, char transa, int n, int k, float alpha, float *a, int lda, float beta, float *c, int ldc);
typedef void (* p_stbmv )(char uplo, char transa, char diag, int n, int ndiag, float *a, int lda, float *y, int incy);
typedef void (* p_stbsv )(char uplo, char transa, char diag, int n, int ndiag, float *a, int lda, float *y, int incy);
typedef void (* p_stpmv )(char uplo, char transa, char diag, int n, float *a, float *y, int incy);
typedef void (* p_stpsv )(char uplo, char transa, char diag, int n, float *a, float *y, int incy);
typedef void (* p_strmm )(char side, char uplo, char transa, char diag, int m, int n, float alpha, float *a, int lda, float *b, int ldb);
typedef void (* p_strmv )(char uplo, char transa, char diag, int n, float *a, int lda, float *y, int incy);
typedef void (* p_strsm )(char side, char uplo, char transa, char diag, int m, int n, float alpha, float *a, int lda, float *b, int ldb);
typedef void (* p_strsv )(char uplo, char transa, char diag, int n, float *a, int lda, float *y, int incy);
typedef void (* p_xerbla )(const char *srname, int *info);
typedef void (* p_zaxpy )(int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy);
typedef void (* p_zcopy )(int n, doublecomplex *x, int incx, doublecomplex *y, int incy);
typedef doublecomplex (* p_zdotc )(int n, doublecomplex *x, int incx, doublecomplex *y, int incy);
typedef doublecomplex (* p_zdotu )(int n, doublecomplex *x, int incx, doublecomplex *y, int incy);
typedef void (* p_zdrot )(int n, doublecomplex *cx, int incx, doublecomplex *cy, int incy, double c, double s);
typedef void (* p_zdscal )(int n, double alpha, doublecomplex *y, int incy);
typedef void (* p_zgbmv )(char transa, int m, int n, int nsub, int nsuper, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy);
typedef void (* p_zgemm )(char transa, char transb, int m, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc);
typedef void (* p_zgemv )(char transa, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy);
typedef void (* p_zgerc )(int m, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda);
typedef void (* p_zgeru )(int m, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda);
typedef void (* p_zhbmv )(char uplo, int n, int ndiag, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy);
typedef void (* p_zhemm )(char side, char uplo, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc);
typedef void (* p_zhemv )(char uplo, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy);
typedef void (* p_zher )(char uplo, int n, double alpha, doublecomplex *x, int incx, doublecomplex *a, int lda);
typedef void (* p_zher2 )(char uplo, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda);
typedef void (* p_zher2k )(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, double beta, doublecomplex *c, int ldc);
typedef void (* p_zherk )(char uplo, char transa, int n, int k, double alpha, doublecomplex *a, int lda, double beta, doublecomplex *c, int ldc);
typedef void (* p_zhpmv )(char uplo, int n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy);
typedef void (* p_zhpr )(char uplo, int n, double alpha, doublecomplex *x, int incx, doublecomplex *a);
typedef void (* p_zhpr2 )(char uplo, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a);
typedef void (* p_zlassq )(int n, doublecomplex *x, int incx, double *scale, double *sumsq);
typedef void (* p_zrotg )(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s);
typedef void (* p_zscal )(int n, doublecomplex *alpha, doublecomplex *y, int incy);
typedef void (* p_zstemr )(char jobz, char range, int n, double *d, double *e, double vl, double vu, int il, int iu, int *m, double *w, doublecomplex *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info);
typedef void (* p_zswap )(int n, doublecomplex *x, int incx, doublecomplex *y, int incy);
typedef void (* p_zsymm )(char side, char uplo, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc);
typedef void (* p_zsyr2k )(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc);
typedef void (* p_zsyrk )(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *beta, doublecomplex *c, int ldc);
typedef void (* p_ztbmv )(char uplo, char transa, char diag, int n, int ndiag, doublecomplex *a, int lda, doublecomplex *y, int incy);
typedef void (* p_ztbsv )(char uplo, char transa, char diag, int n, int ndiag, doublecomplex *a, int lda, doublecomplex *y, int incy);
typedef void (* p_ztpmv )(char uplo, char transa, char diag, int n, doublecomplex *a, doublecomplex *y, int incy);
typedef void (* p_ztpsv )(char uplo, char transa, char diag, int n, doublecomplex *a, doublecomplex *y, int incy);
typedef void (* p_ztrmm )(char side, char uplo, char transa, char diag, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb);
typedef void (* p_ztrmv )(char uplo, char transa, char diag, int n, doublecomplex *a, int lda, doublecomplex *y, int incy);
typedef void (* p_ztrsm )(char side, char uplo, char transa, char diag, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb);
typedef void (* p_ztrsv )(char uplo, char transa, char diag, int n, doublecomplex *a, int lda, doublecomplex *y, int incy);


void caxpy(int n, complex *alpha, complex *x, int incx, complex *y, int incy)
{
	if( !wrapper.fn[0] )
		wrapper.fn[0] = wrapper.Resolve("caxpy");
	(* reinterpret_cast<p_caxpy>(wrapper.fn[0]))( n, alpha, x, incx, y, incy );

}

void ccopy(int n, complex *x, int incx, complex *y, int incy)
{
	if( !wrapper.fn[1] )
		wrapper.fn[1] = wrapper.Resolve("ccopy");
	(* reinterpret_cast<p_ccopy>(wrapper.fn[1]))( n, x, incx, y, incy );

}

complex cdotc(int n, complex *x, int incx, complex *y, int incy)
{
	if( !wrapper.fn[2] )
		wrapper.fn[2] = wrapper.Resolve("cdotc");
	return (* reinterpret_cast<p_cdotc>(wrapper.fn[2]))( n, x, incx, y, incy );

}

complex cdotu(int n, complex *x, int incx, complex *y, int incy)
{
	if( !wrapper.fn[3] )
		wrapper.fn[3] = wrapper.Resolve("cdotu");
	return (* reinterpret_cast<p_cdotu>(wrapper.fn[3]))( n, x, incx, y, incy );

}

void cgbmv(char transa, int m, int n, int nsub, int nsuper, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy)
{
	if( !wrapper.fn[4] )
		wrapper.fn[4] = wrapper.Resolve("cgbmv");
	(* reinterpret_cast<p_cgbmv>(wrapper.fn[4]))( transa, m, n, nsub, nsuper, alpha, a, lda, x, incx, beta, y, incy );

}

void cgemm(char transa, char transb, int m, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc)
{
	if( !wrapper.fn[5] )
		wrapper.fn[5] = wrapper.Resolve("cgemm");
	(* reinterpret_cast<p_cgemm>(wrapper.fn[5]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void cgemv(char transa, int m, int n, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy)
{
	if( !wrapper.fn[6] )
		wrapper.fn[6] = wrapper.Resolve("cgemv");
	(* reinterpret_cast<p_cgemv>(wrapper.fn[6]))( transa, m, n, alpha, a, lda, x, incx, beta, y, incy );

}

void cgerc(int m, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda)
{
	if( !wrapper.fn[7] )
		wrapper.fn[7] = wrapper.Resolve("cgerc");
	(* reinterpret_cast<p_cgerc>(wrapper.fn[7]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void cgeru(int m, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda)
{
	if( !wrapper.fn[8] )
		wrapper.fn[8] = wrapper.Resolve("cgeru");
	(* reinterpret_cast<p_cgeru>(wrapper.fn[8]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void chbmv(char uplo, int n, int ndiag, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy)
{
	if( !wrapper.fn[9] )
		wrapper.fn[9] = wrapper.Resolve("chbmv");
	(* reinterpret_cast<p_chbmv>(wrapper.fn[9]))( uplo, n, ndiag, alpha, a, lda, x, incx, beta, y, incy );

}

void chemm(char side, char uplo, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc)
{
	if( !wrapper.fn[10] )
		wrapper.fn[10] = wrapper.Resolve("chemm");
	(* reinterpret_cast<p_chemm>(wrapper.fn[10]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void chemv(char uplo, int n, complex *alpha, complex *a, int lda, complex *x, int incx, complex *beta, complex *y, int incy)
{
	if( !wrapper.fn[11] )
		wrapper.fn[11] = wrapper.Resolve("chemv");
	(* reinterpret_cast<p_chemv>(wrapper.fn[11]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy );

}

void cher(char uplo, int n, float alpha, complex *x, int incx, complex *a, int lda)
{
	if( !wrapper.fn[12] )
		wrapper.fn[12] = wrapper.Resolve("cher");
	(* reinterpret_cast<p_cher>(wrapper.fn[12]))( uplo, n, alpha, x, incx, a, lda );

}

void cher2(char uplo, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a, int lda)
{
	if( !wrapper.fn[13] )
		wrapper.fn[13] = wrapper.Resolve("cher2");
	(* reinterpret_cast<p_cher2>(wrapper.fn[13]))( uplo, n, alpha, x, incx, y, incy, a, lda );

}

void cher2k(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, float beta, complex *c, int ldc)
{
	if( !wrapper.fn[14] )
		wrapper.fn[14] = wrapper.Resolve("cher2k");
	(* reinterpret_cast<p_cher2k>(wrapper.fn[14]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void cherk(char uplo, char transa, int n, int k, float alpha, complex *a, int lda, float beta, complex *c, int ldc)
{
	if( !wrapper.fn[15] )
		wrapper.fn[15] = wrapper.Resolve("cherk");
	(* reinterpret_cast<p_cherk>(wrapper.fn[15]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void chpmv(char uplo, int n, complex *alpha, complex *a, complex *x, int incx, complex *beta, complex *y, int incy)
{
	if( !wrapper.fn[16] )
		wrapper.fn[16] = wrapper.Resolve("chpmv");
	(* reinterpret_cast<p_chpmv>(wrapper.fn[16]))( uplo, n, alpha, a, x, incx, beta, y, incy );

}

void chpr(char uplo, int n, float alpha, complex *x, int incx, complex *a)
{
	if( !wrapper.fn[17] )
		wrapper.fn[17] = wrapper.Resolve("chpr");
	(* reinterpret_cast<p_chpr>(wrapper.fn[17]))( uplo, n, alpha, x, incx, a );

}

void chpr2(char uplo, int n, complex *alpha, complex *x, int incx, complex *y, int incy, complex *a)
{
	if( !wrapper.fn[18] )
		wrapper.fn[18] = wrapper.Resolve("chpr2");
	(* reinterpret_cast<p_chpr2>(wrapper.fn[18]))( uplo, n, alpha, x, incx, y, incy, a );

}

void classq(int n, complex *x, int incx, float *scale, float *sumsq)
{
	if( !wrapper.fn[19] )
		wrapper.fn[19] = wrapper.Resolve("classq");
	(* reinterpret_cast<p_classq>(wrapper.fn[19]))( n, x, incx, scale, sumsq );

}

void crotg(complex *a, complex *b, float *c, complex *s)
{
	if( !wrapper.fn[20] )
		wrapper.fn[20] = wrapper.Resolve("crotg");
	(* reinterpret_cast<p_crotg>(wrapper.fn[20]))( a, b, c, s );

}

void cscal(int n, complex *alpha, complex *y, int incy)
{
	if( !wrapper.fn[21] )
		wrapper.fn[21] = wrapper.Resolve("cscal");
	(* reinterpret_cast<p_cscal>(wrapper.fn[21]))( n, alpha, y, incy );

}

void csrot(int n, complex *x, int incx, complex *y, int incy, float c, float s)
{
	if( !wrapper.fn[22] )
		wrapper.fn[22] = wrapper.Resolve("csrot");
	(* reinterpret_cast<p_csrot>(wrapper.fn[22]))( n, x, incx, y, incy, c, s );

}

void csscal(int n, float alpha, complex *y, int incy)
{
	if( !wrapper.fn[23] )
		wrapper.fn[23] = wrapper.Resolve("csscal");
	(* reinterpret_cast<p_csscal>(wrapper.fn[23]))( n, alpha, y, incy );

}

void cstemr(char jobz, char range, int n, float *d, float *e, float vl, float vu, int il, int iu, int *m, float *w, complex *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info)
{
	if( !wrapper.fn[24] )
		wrapper.fn[24] = wrapper.Resolve("cstemr");
	(* reinterpret_cast<p_cstemr>(wrapper.fn[24]))( jobz, range, n, d, e, vl, vu, il, iu, m, w, z, ldz, nzc, isuppz, tryrac, info );

}

void cswap(int n, complex *x, int incx, complex *y, int incy)
{
	if( !wrapper.fn[25] )
		wrapper.fn[25] = wrapper.Resolve("cswap");
	(* reinterpret_cast<p_cswap>(wrapper.fn[25]))( n, x, incx, y, incy );

}

void csymm(char side, char uplo, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc)
{
	if( !wrapper.fn[26] )
		wrapper.fn[26] = wrapper.Resolve("csymm");
	(* reinterpret_cast<p_csymm>(wrapper.fn[26]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void csyr2k(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *b, int ldb, complex *beta, complex *c, int ldc)
{
	if( !wrapper.fn[27] )
		wrapper.fn[27] = wrapper.Resolve("csyr2k");
	(* reinterpret_cast<p_csyr2k>(wrapper.fn[27]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void csyrk(char uplo, char transa, int n, int k, complex *alpha, complex *a, int lda, complex *beta, complex *c, int ldc)
{
	if( !wrapper.fn[28] )
		wrapper.fn[28] = wrapper.Resolve("csyrk");
	(* reinterpret_cast<p_csyrk>(wrapper.fn[28]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void ctbmv(char uplo, char transa, char diag, int n, int ndiag, complex *a, int lda, complex *y, int incy)
{
	if( !wrapper.fn[29] )
		wrapper.fn[29] = wrapper.Resolve("ctbmv");
	(* reinterpret_cast<p_ctbmv>(wrapper.fn[29]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void ctbsv(char uplo, char transa, char diag, int n, int ndiag, complex *a, int lda, complex *y, int incy)
{
	if( !wrapper.fn[30] )
		wrapper.fn[30] = wrapper.Resolve("ctbsv");
	(* reinterpret_cast<p_ctbsv>(wrapper.fn[30]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void ctpmv(char uplo, char transa, char diag, int n, complex *a, complex *y, int incy)
{
	if( !wrapper.fn[31] )
		wrapper.fn[31] = wrapper.Resolve("ctpmv");
	(* reinterpret_cast<p_ctpmv>(wrapper.fn[31]))( uplo, transa, diag, n, a, y, incy );

}

void ctpsv(char uplo, char transa, char diag, int n, complex *a, complex *y, int incy)
{
	if( !wrapper.fn[32] )
		wrapper.fn[32] = wrapper.Resolve("ctpsv");
	(* reinterpret_cast<p_ctpsv>(wrapper.fn[32]))( uplo, transa, diag, n, a, y, incy );

}

void ctrmm(char side, char uplo, char transa, char diag, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb)
{
	if( !wrapper.fn[33] )
		wrapper.fn[33] = wrapper.Resolve("ctrmm");
	(* reinterpret_cast<p_ctrmm>(wrapper.fn[33]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void ctrmv(char uplo, char transa, char diag, int n, complex *a, int lda, complex *y, int incy)
{
	if( !wrapper.fn[34] )
		wrapper.fn[34] = wrapper.Resolve("ctrmv");
	(* reinterpret_cast<p_ctrmv>(wrapper.fn[34]))( uplo, transa, diag, n, a, lda, y, incy );

}

void ctrsm(char side, char uplo, char transa, char diag, int m, int n, complex *alpha, complex *a, int lda, complex *b, int ldb)
{
	if( !wrapper.fn[35] )
		wrapper.fn[35] = wrapper.Resolve("ctrsm");
	(* reinterpret_cast<p_ctrsm>(wrapper.fn[35]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void ctrsv(char uplo, char transa, char diag, int n, complex *a, int lda, complex *y, int incy)
{
	if( !wrapper.fn[36] )
		wrapper.fn[36] = wrapper.Resolve("ctrsv");
	(* reinterpret_cast<p_ctrsv>(wrapper.fn[36]))( uplo, transa, diag, n, a, lda, y, incy );

}

double dasum(int n, double *x, int incx)
{
	if( !wrapper.fn[37] )
		wrapper.fn[37] = wrapper.Resolve("dasum");
	return (* reinterpret_cast<p_dasum>(wrapper.fn[37]))( n, x, incx );

}

void daxpy(int n, double alpha, double *x, int incx, double *y, int incy)
{
	if( !wrapper.fn[38] )
		wrapper.fn[38] = wrapper.Resolve("daxpy");
	(* reinterpret_cast<p_daxpy>(wrapper.fn[38]))( n, alpha, x, incx, y, incy );

}

void dcopy(int n, double *x, int incx, double *y, int incy)
{
	if( !wrapper.fn[39] )
		wrapper.fn[39] = wrapper.Resolve("dcopy");
	(* reinterpret_cast<p_dcopy>(wrapper.fn[39]))( n, x, incx, y, incy );

}

double ddot(int n, double *x, int incx, double *y, int incy)
{
	if( !wrapper.fn[40] )
		wrapper.fn[40] = wrapper.Resolve("ddot");
	return (* reinterpret_cast<p_ddot>(wrapper.fn[40]))( n, x, incx, y, incy );

}

void dgbmv(char transa, int m, int n, int nsub, int nsuper, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy)
{
	if( !wrapper.fn[41] )
		wrapper.fn[41] = wrapper.Resolve("dgbmv");
	(* reinterpret_cast<p_dgbmv>(wrapper.fn[41]))( transa, m, n, nsub, nsuper, alpha, a, lda, x, incx, beta, y, incy );

}

void dgemm(char transa, char transb, int m, int n, int k, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc)
{
	if( !wrapper.fn[42] )
		wrapper.fn[42] = wrapper.Resolve("dgemm");
	(* reinterpret_cast<p_dgemm>(wrapper.fn[42]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void dgemv(char transa, int m, int n, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy)
{
	if( !wrapper.fn[43] )
		wrapper.fn[43] = wrapper.Resolve("dgemv");
	(* reinterpret_cast<p_dgemv>(wrapper.fn[43]))( transa, m, n, alpha, a, lda, x, incx, beta, y, incy );

}

void dger(int m, int n, double alpha, double *x, int incx, double *y, int incy, double *a, int lda)
{
	if( !wrapper.fn[44] )
		wrapper.fn[44] = wrapper.Resolve("dger");
	(* reinterpret_cast<p_dger>(wrapper.fn[44]))( m, n, alpha, x, incx, y, incy, a, lda );

}

double dnrm2(int n, double *x, int incx)
{
	if( !wrapper.fn[45] )
		wrapper.fn[45] = wrapper.Resolve("dnrm2");
	return (* reinterpret_cast<p_dnrm2>(wrapper.fn[45]))( n, x, incx );

}

void drot(int n, double *x, int incx, double *y, int incy, double c, double s)
{
	if( !wrapper.fn[46] )
		wrapper.fn[46] = wrapper.Resolve("drot");
	(* reinterpret_cast<p_drot>(wrapper.fn[46]))( n, x, incx, y, incy, c, s );

}

void drotg(double *a, double *b, double *c, double *s)
{
	if( !wrapper.fn[47] )
		wrapper.fn[47] = wrapper.Resolve("drotg");
	(* reinterpret_cast<p_drotg>(wrapper.fn[47]))( a, b, c, s );

}

void drotm(int n, double *x, int incx, double *y, int incy, double *param)
{
	if( !wrapper.fn[48] )
		wrapper.fn[48] = wrapper.Resolve("drotm");
	(* reinterpret_cast<p_drotm>(wrapper.fn[48]))( n, x, incx, y, incy, param );

}

void drotmg(double d1, double d2, double b1, double b2, double *param)
{
	if( !wrapper.fn[49] )
		wrapper.fn[49] = wrapper.Resolve("drotmg");
	(* reinterpret_cast<p_drotmg>(wrapper.fn[49]))( d1, d2, b1, b2, param );

}

void dsbmv(char uplo, int n, int ndiag, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy)
{
	if( !wrapper.fn[50] )
		wrapper.fn[50] = wrapper.Resolve("dsbmv");
	(* reinterpret_cast<p_dsbmv>(wrapper.fn[50]))( uplo, n, ndiag, alpha, a, lda, x, incx, beta, y, incy );

}

void dscal(int n, double alpha, double *y, int incy)
{
	if( !wrapper.fn[51] )
		wrapper.fn[51] = wrapper.Resolve("dscal");
	(* reinterpret_cast<p_dscal>(wrapper.fn[51]))( n, alpha, y, incy );

}

double dsdot(int n, float *x, int incx, float *y, int incy)
{
	if( !wrapper.fn[52] )
		wrapper.fn[52] = wrapper.Resolve("dsdot");
	return (* reinterpret_cast<p_dsdot>(wrapper.fn[52]))( n, x, incx, y, incy );

}

void dspmv(char uplo, int n, double alpha, double *a, double *x, int incx, double beta, double *y, int incy)
{
	if( !wrapper.fn[53] )
		wrapper.fn[53] = wrapper.Resolve("dspmv");
	(* reinterpret_cast<p_dspmv>(wrapper.fn[53]))( uplo, n, alpha, a, x, incx, beta, y, incy );

}

void dspr(char uplo, int n, double alpha, double *x, int incx, double *a)
{
	if( !wrapper.fn[54] )
		wrapper.fn[54] = wrapper.Resolve("dspr");
	(* reinterpret_cast<p_dspr>(wrapper.fn[54]))( uplo, n, alpha, x, incx, a );

}

void dspr2(char uplo, int n, double alpha, double *x, int incx, double *y, int incy, double *a)
{
	if( !wrapper.fn[55] )
		wrapper.fn[55] = wrapper.Resolve("dspr2");
	(* reinterpret_cast<p_dspr2>(wrapper.fn[55]))( uplo, n, alpha, x, incx, y, incy, a );

}

void dswap(int n, double *x, int incx, double *y, int incy)
{
	if( !wrapper.fn[56] )
		wrapper.fn[56] = wrapper.Resolve("dswap");
	(* reinterpret_cast<p_dswap>(wrapper.fn[56]))( n, x, incx, y, incy );

}

void dsymm(char side, char uplo, int m, int n, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc)
{
	if( !wrapper.fn[57] )
		wrapper.fn[57] = wrapper.Resolve("dsymm");
	(* reinterpret_cast<p_dsymm>(wrapper.fn[57]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void dsymv(char uplo, int n, double alpha, double *a, int lda, double *x, int incx, double beta, double *y, int incy)
{
	if( !wrapper.fn[58] )
		wrapper.fn[58] = wrapper.Resolve("dsymv");
	(* reinterpret_cast<p_dsymv>(wrapper.fn[58]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy );

}

void dsyr(char uplo, int n, double alpha, double *x, int incx, double *a, int lda)
{
	if( !wrapper.fn[59] )
		wrapper.fn[59] = wrapper.Resolve("dsyr");
	(* reinterpret_cast<p_dsyr>(wrapper.fn[59]))( uplo, n, alpha, x, incx, a, lda );

}

void dsyr2(char uplo, int n, double alpha, double *x, int incx, double *y, int incy, double *a, int lda)
{
	if( !wrapper.fn[60] )
		wrapper.fn[60] = wrapper.Resolve("dsyr2");
	(* reinterpret_cast<p_dsyr2>(wrapper.fn[60]))( uplo, n, alpha, x, incx, y, incy, a, lda );

}

void dsyr2k(char uplo, char transa, int n, int k, double alpha, double *a, int lda, double *b, int ldb, double beta, double *c, int ldc)
{
	if( !wrapper.fn[61] )
		wrapper.fn[61] = wrapper.Resolve("dsyr2k");
	(* reinterpret_cast<p_dsyr2k>(wrapper.fn[61]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void dsyrk(char uplo, char transa, int n, int k, double alpha, double *a, int lda, double beta, double *c, int ldc)
{
	if( !wrapper.fn[62] )
		wrapper.fn[62] = wrapper.Resolve("dsyrk");
	(* reinterpret_cast<p_dsyrk>(wrapper.fn[62]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void dtbmv(char uplo, char transa, char diag, int n, int ndiag, double *a, int lda, double *y, int incy)
{
	if( !wrapper.fn[63] )
		wrapper.fn[63] = wrapper.Resolve("dtbmv");
	(* reinterpret_cast<p_dtbmv>(wrapper.fn[63]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void dtbsv(char uplo, char transa, char diag, int n, int ndiag, double *a, int lda, double *y, int incy)
{
	if( !wrapper.fn[64] )
		wrapper.fn[64] = wrapper.Resolve("dtbsv");
	(* reinterpret_cast<p_dtbsv>(wrapper.fn[64]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void dtpmv(char uplo, char transa, char diag, int n, double *a, double *y, int incy)
{
	if( !wrapper.fn[65] )
		wrapper.fn[65] = wrapper.Resolve("dtpmv");
	(* reinterpret_cast<p_dtpmv>(wrapper.fn[65]))( uplo, transa, diag, n, a, y, incy );

}

void dtpsv(char uplo, char transa, char diag, int n, double *a, double *y, int incy)
{
	if( !wrapper.fn[66] )
		wrapper.fn[66] = wrapper.Resolve("dtpsv");
	(* reinterpret_cast<p_dtpsv>(wrapper.fn[66]))( uplo, transa, diag, n, a, y, incy );

}

void dtrmm(char side, char uplo, char transa, char diag, int m, int n, double alpha, double *a, int lda, double *b, int ldb)
{
	if( !wrapper.fn[67] )
		wrapper.fn[67] = wrapper.Resolve("dtrmm");
	(* reinterpret_cast<p_dtrmm>(wrapper.fn[67]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void dtrmv(char uplo, char transa, char diag, int n, double *a, int lda, double *y, int incy)
{
	if( !wrapper.fn[68] )
		wrapper.fn[68] = wrapper.Resolve("dtrmv");
	(* reinterpret_cast<p_dtrmv>(wrapper.fn[68]))( uplo, transa, diag, n, a, lda, y, incy );

}

void dtrsm(char side, char uplo, char transa, char diag, int m, int n, double alpha, double *a, int lda, double *b, int ldb)
{
	if( !wrapper.fn[69] )
		wrapper.fn[69] = wrapper.Resolve("dtrsm");
	(* reinterpret_cast<p_dtrsm>(wrapper.fn[69]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void dtrsv(char uplo, char transa, char diag, int n, double *a, int lda, double *y, int incy)
{
	if( !wrapper.fn[70] )
		wrapper.fn[70] = wrapper.Resolve("dtrsv");
	(* reinterpret_cast<p_dtrsv>(wrapper.fn[70]))( uplo, transa, diag, n, a, lda, y, incy );

}

double dzasum(int n, doublecomplex *x, int incx)
{
	if( !wrapper.fn[71] )
		wrapper.fn[71] = wrapper.Resolve("dzasum");
	return (* reinterpret_cast<p_dzasum>(wrapper.fn[71]))( n, x, incx );

}

double dznrm2(int n, doublecomplex *x, int incx)
{
	if( !wrapper.fn[72] )
		wrapper.fn[72] = wrapper.Resolve("dznrm2");
	return (* reinterpret_cast<p_dznrm2>(wrapper.fn[72]))( n, x, incx );

}

int icamax(int n, complex *x, int incx)
{
	if( !wrapper.fn[73] )
		wrapper.fn[73] = wrapper.Resolve("icamax");
	return (* reinterpret_cast<p_icamax>(wrapper.fn[73]))( n, x, incx );

}

int idamax(int n, double *x, int incx)
{
	if( !wrapper.fn[74] )
		wrapper.fn[74] = wrapper.Resolve("idamax");
	return (* reinterpret_cast<p_idamax>(wrapper.fn[74]))( n, x, incx );

}

int isamax(int n, float *x, int incx)
{
	if( !wrapper.fn[75] )
		wrapper.fn[75] = wrapper.Resolve("isamax");
	return (* reinterpret_cast<p_isamax>(wrapper.fn[75]))( n, x, incx );

}

int izamax(int n, doublecomplex *x, int incx)
{
	if( !wrapper.fn[76] )
		wrapper.fn[76] = wrapper.Resolve("izamax");
	return (* reinterpret_cast<p_izamax>(wrapper.fn[76]))( n, x, incx );

}

int lsame(char ca, char cb)
{
	if( !wrapper.fn[77] )
		wrapper.fn[77] = wrapper.Resolve("lsame");
	return (* reinterpret_cast<p_lsame>(wrapper.fn[77]))( ca, cb );

}

float sasum(int n, float *x, int incx)
{
	if( !wrapper.fn[78] )
		wrapper.fn[78] = wrapper.Resolve("sasum");
	return (* reinterpret_cast<p_sasum>(wrapper.fn[78]))( n, x, incx );

}

void saxpy(int n, float alpha, float *x, int incx, float *y, int incy)
{
	if( !wrapper.fn[79] )
		wrapper.fn[79] = wrapper.Resolve("saxpy");
	(* reinterpret_cast<p_saxpy>(wrapper.fn[79]))( n, alpha, x, incx, y, incy );

}

float scasum(int n, complex *x, int incx)
{
	if( !wrapper.fn[80] )
		wrapper.fn[80] = wrapper.Resolve("scasum");
	return (* reinterpret_cast<p_scasum>(wrapper.fn[80]))( n, x, incx );

}

float scnrm2(int n, complex *x, int incx)
{
	if( !wrapper.fn[81] )
		wrapper.fn[81] = wrapper.Resolve("scnrm2");
	return (* reinterpret_cast<p_scnrm2>(wrapper.fn[81]))( n, x, incx );

}

void scopy(int n, float *x, int incx, float *y, int incy)
{
	if( !wrapper.fn[82] )
		wrapper.fn[82] = wrapper.Resolve("scopy");
	(* reinterpret_cast<p_scopy>(wrapper.fn[82]))( n, x, incx, y, incy );

}

float sdot(int n, float *x, int incx, float *y, int incy)
{
	if( !wrapper.fn[83] )
		wrapper.fn[83] = wrapper.Resolve("sdot");
	return (* reinterpret_cast<p_sdot>(wrapper.fn[83]))( n, x, incx, y, incy );

}

float sdsdot(int n, float sb, float *sx, int incx, float *sy, int incy)
{
	if( !wrapper.fn[84] )
		wrapper.fn[84] = wrapper.Resolve("sdsdot");
	return (* reinterpret_cast<p_sdsdot>(wrapper.fn[84]))( n, sb, sx, incx, sy, incy );

}

void sgbmv(char transa, int m, int n, int nsub, int nsuper, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy)
{
	if( !wrapper.fn[85] )
		wrapper.fn[85] = wrapper.Resolve("sgbmv");
	(* reinterpret_cast<p_sgbmv>(wrapper.fn[85]))( transa, m, n, nsub, nsuper, alpha, a, lda, x, incx, beta, y, incy );

}

void sgemm(char transa, char transb, int m, int n, int k, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc)
{
	if( !wrapper.fn[86] )
		wrapper.fn[86] = wrapper.Resolve("sgemm");
	(* reinterpret_cast<p_sgemm>(wrapper.fn[86]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void sgemv(char transa, int m, int n, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy)
{
	if( !wrapper.fn[87] )
		wrapper.fn[87] = wrapper.Resolve("sgemv");
	(* reinterpret_cast<p_sgemv>(wrapper.fn[87]))( transa, m, n, alpha, a, lda, x, incx, beta, y, incy );

}

void sger(int m, int n, float alpha, float *x, int incx, float *y, int incy, float *a, int lda)
{
	if( !wrapper.fn[88] )
		wrapper.fn[88] = wrapper.Resolve("sger");
	(* reinterpret_cast<p_sger>(wrapper.fn[88]))( m, n, alpha, x, incx, y, incy, a, lda );

}

float snrm2(int n, float *x, int incx)
{
	if( !wrapper.fn[89] )
		wrapper.fn[89] = wrapper.Resolve("snrm2");
	return (* reinterpret_cast<p_snrm2>(wrapper.fn[89]))( n, x, incx );

}

void srot(int n, float *x, int incx, float *y, int incy, float c, float s)
{
	if( !wrapper.fn[90] )
		wrapper.fn[90] = wrapper.Resolve("srot");
	(* reinterpret_cast<p_srot>(wrapper.fn[90]))( n, x, incx, y, incy, c, s );

}

void srotg(float *a, float *b, float *c, float *s)
{
	if( !wrapper.fn[91] )
		wrapper.fn[91] = wrapper.Resolve("srotg");
	(* reinterpret_cast<p_srotg>(wrapper.fn[91]))( a, b, c, s );

}

void srotm(int n, float *x, int incx, float *y, int incy, float *param)
{
	if( !wrapper.fn[92] )
		wrapper.fn[92] = wrapper.Resolve("srotm");
	(* reinterpret_cast<p_srotm>(wrapper.fn[92]))( n, x, incx, y, incy, param );

}

void srotmg(float d1, float d2, float b1, float b2, float *param)
{
	if( !wrapper.fn[93] )
		wrapper.fn[93] = wrapper.Resolve("srotmg");
	(* reinterpret_cast<p_srotmg>(wrapper.fn[93]))( d1, d2, b1, b2, param );

}

void ssbmv(char uplo, int n, int ndiag, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy)
{
	if( !wrapper.fn[94] )
		wrapper.fn[94] = wrapper.Resolve("ssbmv");
	(* reinterpret_cast<p_ssbmv>(wrapper.fn[94]))( uplo, n, ndiag, alpha, a, lda, x, incx, beta, y, incy );

}

void sscal(int n, float alpha, float *x, int incx)
{
	if( !wrapper.fn[95] )
		wrapper.fn[95] = wrapper.Resolve("sscal");
	(* reinterpret_cast<p_sscal>(wrapper.fn[95]))( n, alpha, x, incx );

}

void sspmv(char uplo, int n, float alpha, float *a, float *x, int incx, float beta, float *y, int incy)
{
	if( !wrapper.fn[96] )
		wrapper.fn[96] = wrapper.Resolve("sspmv");
	(* reinterpret_cast<p_sspmv>(wrapper.fn[96]))( uplo, n, alpha, a, x, incx, beta, y, incy );

}

void sspr(char uplo, int n, float alpha, float *x, int incx, float *a)
{
	if( !wrapper.fn[97] )
		wrapper.fn[97] = wrapper.Resolve("sspr");
	(* reinterpret_cast<p_sspr>(wrapper.fn[97]))( uplo, n, alpha, x, incx, a );

}

void sspr2(char uplo, int n, float alpha, float *x, int incx, float *y, int incy, float *a)
{
	if( !wrapper.fn[98] )
		wrapper.fn[98] = wrapper.Resolve("sspr2");
	(* reinterpret_cast<p_sspr2>(wrapper.fn[98]))( uplo, n, alpha, x, incx, y, incy, a );

}

void sstemr(char jobz, char range, int n, float *d, float *e, float vl, float vu, int il, int iu, int *m, float *w, float *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info)
{
	if( !wrapper.fn[99] )
		wrapper.fn[99] = wrapper.Resolve("sstemr");
	(* reinterpret_cast<p_sstemr>(wrapper.fn[99]))( jobz, range, n, d, e, vl, vu, il, iu, m, w, z, ldz, nzc, isuppz, tryrac, info );

}

void sswap(int n, float *x, int incx, float *y, int incy)
{
	if( !wrapper.fn[100] )
		wrapper.fn[100] = wrapper.Resolve("sswap");
	(* reinterpret_cast<p_sswap>(wrapper.fn[100]))( n, x, incx, y, incy );

}

void ssymm(char side, char uplo, int m, int n, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc)
{
	if( !wrapper.fn[101] )
		wrapper.fn[101] = wrapper.Resolve("ssymm");
	(* reinterpret_cast<p_ssymm>(wrapper.fn[101]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void ssymv(char uplo, int n, float alpha, float *a, int lda, float *x, int incx, float beta, float *y, int incy)
{
	if( !wrapper.fn[102] )
		wrapper.fn[102] = wrapper.Resolve("ssymv");
	(* reinterpret_cast<p_ssymv>(wrapper.fn[102]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy );

}

void ssyr(char uplo, int n, float alpha, float *x, int incx, float *a, int lda)
{
	if( !wrapper.fn[103] )
		wrapper.fn[103] = wrapper.Resolve("ssyr");
	(* reinterpret_cast<p_ssyr>(wrapper.fn[103]))( uplo, n, alpha, x, incx, a, lda );

}

void ssyr2(char uplo, int n, float alpha, float *x, int incx, float *y, int incy, float *a, int lda)
{
	if( !wrapper.fn[104] )
		wrapper.fn[104] = wrapper.Resolve("ssyr2");
	(* reinterpret_cast<p_ssyr2>(wrapper.fn[104]))( uplo, n, alpha, x, incx, y, incy, a, lda );

}

void ssyr2k(char uplo, char transa, int n, int k, float alpha, float *a, int lda, float *b, int ldb, float beta, float *c, int ldc)
{
	if( !wrapper.fn[105] )
		wrapper.fn[105] = wrapper.Resolve("ssyr2k");
	(* reinterpret_cast<p_ssyr2k>(wrapper.fn[105]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void ssyrk(char uplo, char transa, int n, int k, float alpha, float *a, int lda, float beta, float *c, int ldc)
{
	if( !wrapper.fn[106] )
		wrapper.fn[106] = wrapper.Resolve("ssyrk");
	(* reinterpret_cast<p_ssyrk>(wrapper.fn[106]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void stbmv(char uplo, char transa, char diag, int n, int ndiag, float *a, int lda, float *y, int incy)
{
	if( !wrapper.fn[107] )
		wrapper.fn[107] = wrapper.Resolve("stbmv");
	(* reinterpret_cast<p_stbmv>(wrapper.fn[107]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void stbsv(char uplo, char transa, char diag, int n, int ndiag, float *a, int lda, float *y, int incy)
{
	if( !wrapper.fn[108] )
		wrapper.fn[108] = wrapper.Resolve("stbsv");
	(* reinterpret_cast<p_stbsv>(wrapper.fn[108]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void stpmv(char uplo, char transa, char diag, int n, float *a, float *y, int incy)
{
	if( !wrapper.fn[109] )
		wrapper.fn[109] = wrapper.Resolve("stpmv");
	(* reinterpret_cast<p_stpmv>(wrapper.fn[109]))( uplo, transa, diag, n, a, y, incy );

}

void stpsv(char uplo, char transa, char diag, int n, float *a, float *y, int incy)
{
	if( !wrapper.fn[110] )
		wrapper.fn[110] = wrapper.Resolve("stpsv");
	(* reinterpret_cast<p_stpsv>(wrapper.fn[110]))( uplo, transa, diag, n, a, y, incy );

}

void strmm(char side, char uplo, char transa, char diag, int m, int n, float alpha, float *a, int lda, float *b, int ldb)
{
	if( !wrapper.fn[111] )
		wrapper.fn[111] = wrapper.Resolve("strmm");
	(* reinterpret_cast<p_strmm>(wrapper.fn[111]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void strmv(char uplo, char transa, char diag, int n, float *a, int lda, float *y, int incy)
{
	if( !wrapper.fn[112] )
		wrapper.fn[112] = wrapper.Resolve("strmv");
	(* reinterpret_cast<p_strmv>(wrapper.fn[112]))( uplo, transa, diag, n, a, lda, y, incy );

}

void strsm(char side, char uplo, char transa, char diag, int m, int n, float alpha, float *a, int lda, float *b, int ldb)
{
	if( !wrapper.fn[113] )
		wrapper.fn[113] = wrapper.Resolve("strsm");
	(* reinterpret_cast<p_strsm>(wrapper.fn[113]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void strsv(char uplo, char transa, char diag, int n, float *a, int lda, float *y, int incy)
{
	if( !wrapper.fn[114] )
		wrapper.fn[114] = wrapper.Resolve("strsv");
	(* reinterpret_cast<p_strsv>(wrapper.fn[114]))( uplo, transa, diag, n, a, lda, y, incy );

}

void xerbla(const char *srname, int *info)
{
	if( !wrapper.fn[115] )
		wrapper.fn[115] = wrapper.Resolve("xerbla");
	(* reinterpret_cast<p_xerbla>(wrapper.fn[115]))( srname, info );

}

void zaxpy(int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy)
{
	if( !wrapper.fn[116] )
		wrapper.fn[116] = wrapper.Resolve("zaxpy");
	(* reinterpret_cast<p_zaxpy>(wrapper.fn[116]))( n, alpha, x, incx, y, incy );

}

void zcopy(int n, doublecomplex *x, int incx, doublecomplex *y, int incy)
{
	if( !wrapper.fn[117] )
		wrapper.fn[117] = wrapper.Resolve("zcopy");
	(* reinterpret_cast<p_zcopy>(wrapper.fn[117]))( n, x, incx, y, incy );

}

doublecomplex zdotc(int n, doublecomplex *x, int incx, doublecomplex *y, int incy)
{
	if( !wrapper.fn[118] )
		wrapper.fn[118] = wrapper.Resolve("zdotc");
	return (* reinterpret_cast<p_zdotc>(wrapper.fn[118]))( n, x, incx, y, incy );

}

doublecomplex zdotu(int n, doublecomplex *x, int incx, doublecomplex *y, int incy)
{
	if( !wrapper.fn[119] )
		wrapper.fn[119] = wrapper.Resolve("zdotu");
	return (* reinterpret_cast<p_zdotu>(wrapper.fn[119]))( n, x, incx, y, incy );

}

void zdrot(int n, doublecomplex *cx, int incx, doublecomplex *cy, int incy, double c, double s)
{
	if( !wrapper.fn[120] )
		wrapper.fn[120] = wrapper.Resolve("zdrot");
	(* reinterpret_cast<p_zdrot>(wrapper.fn[120]))( n, cx, incx, cy, incy, c, s );

}

void zdscal(int n, double alpha, doublecomplex *y, int incy)
{
	if( !wrapper.fn[121] )
		wrapper.fn[121] = wrapper.Resolve("zdscal");
	(* reinterpret_cast<p_zdscal>(wrapper.fn[121]))( n, alpha, y, incy );

}

void zgbmv(char transa, int m, int n, int nsub, int nsuper, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy)
{
	if( !wrapper.fn[122] )
		wrapper.fn[122] = wrapper.Resolve("zgbmv");
	(* reinterpret_cast<p_zgbmv>(wrapper.fn[122]))( transa, m, n, nsub, nsuper, alpha, a, lda, x, incx, beta, y, incy );

}

void zgemm(char transa, char transb, int m, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[123] )
		wrapper.fn[123] = wrapper.Resolve("zgemm");
	(* reinterpret_cast<p_zgemm>(wrapper.fn[123]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void zgemv(char transa, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy)
{
	if( !wrapper.fn[124] )
		wrapper.fn[124] = wrapper.Resolve("zgemv");
	(* reinterpret_cast<p_zgemv>(wrapper.fn[124]))( transa, m, n, alpha, a, lda, x, incx, beta, y, incy );

}

void zgerc(int m, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda)
{
	if( !wrapper.fn[125] )
		wrapper.fn[125] = wrapper.Resolve("zgerc");
	(* reinterpret_cast<p_zgerc>(wrapper.fn[125]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zgeru(int m, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda)
{
	if( !wrapper.fn[126] )
		wrapper.fn[126] = wrapper.Resolve("zgeru");
	(* reinterpret_cast<p_zgeru>(wrapper.fn[126]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zhbmv(char uplo, int n, int ndiag, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy)
{
	if( !wrapper.fn[127] )
		wrapper.fn[127] = wrapper.Resolve("zhbmv");
	(* reinterpret_cast<p_zhbmv>(wrapper.fn[127]))( uplo, n, ndiag, alpha, a, lda, x, incx, beta, y, incy );

}

void zhemm(char side, char uplo, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[128] )
		wrapper.fn[128] = wrapper.Resolve("zhemm");
	(* reinterpret_cast<p_zhemm>(wrapper.fn[128]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void zhemv(char uplo, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy)
{
	if( !wrapper.fn[129] )
		wrapper.fn[129] = wrapper.Resolve("zhemv");
	(* reinterpret_cast<p_zhemv>(wrapper.fn[129]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy );

}

void zher(char uplo, int n, double alpha, doublecomplex *x, int incx, doublecomplex *a, int lda)
{
	if( !wrapper.fn[130] )
		wrapper.fn[130] = wrapper.Resolve("zher");
	(* reinterpret_cast<p_zher>(wrapper.fn[130]))( uplo, n, alpha, x, incx, a, lda );

}

void zher2(char uplo, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a, int lda)
{
	if( !wrapper.fn[131] )
		wrapper.fn[131] = wrapper.Resolve("zher2");
	(* reinterpret_cast<p_zher2>(wrapper.fn[131]))( uplo, n, alpha, x, incx, y, incy, a, lda );

}

void zher2k(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, double beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[132] )
		wrapper.fn[132] = wrapper.Resolve("zher2k");
	(* reinterpret_cast<p_zher2k>(wrapper.fn[132]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void zherk(char uplo, char transa, int n, int k, double alpha, doublecomplex *a, int lda, double beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[133] )
		wrapper.fn[133] = wrapper.Resolve("zherk");
	(* reinterpret_cast<p_zherk>(wrapper.fn[133]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void zhpmv(char uplo, int n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int incx, doublecomplex *beta, doublecomplex *y, int incy)
{
	if( !wrapper.fn[134] )
		wrapper.fn[134] = wrapper.Resolve("zhpmv");
	(* reinterpret_cast<p_zhpmv>(wrapper.fn[134]))( uplo, n, alpha, a, x, incx, beta, y, incy );

}

void zhpr(char uplo, int n, double alpha, doublecomplex *x, int incx, doublecomplex *a)
{
	if( !wrapper.fn[135] )
		wrapper.fn[135] = wrapper.Resolve("zhpr");
	(* reinterpret_cast<p_zhpr>(wrapper.fn[135]))( uplo, n, alpha, x, incx, a );

}

void zhpr2(char uplo, int n, doublecomplex *alpha, doublecomplex *x, int incx, doublecomplex *y, int incy, doublecomplex *a)
{
	if( !wrapper.fn[136] )
		wrapper.fn[136] = wrapper.Resolve("zhpr2");
	(* reinterpret_cast<p_zhpr2>(wrapper.fn[136]))( uplo, n, alpha, x, incx, y, incy, a );

}

void zlassq(int n, doublecomplex *x, int incx, double *scale, double *sumsq)
{
	if( !wrapper.fn[137] )
		wrapper.fn[137] = wrapper.Resolve("zlassq");
	(* reinterpret_cast<p_zlassq>(wrapper.fn[137]))( n, x, incx, scale, sumsq );

}

void zrotg(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s)
{
	if( !wrapper.fn[138] )
		wrapper.fn[138] = wrapper.Resolve("zrotg");
	(* reinterpret_cast<p_zrotg>(wrapper.fn[138]))( a, b, c, s );

}

void zscal(int n, doublecomplex *alpha, doublecomplex *y, int incy)
{
	if( !wrapper.fn[139] )
		wrapper.fn[139] = wrapper.Resolve("zscal");
	(* reinterpret_cast<p_zscal>(wrapper.fn[139]))( n, alpha, y, incy );

}

void zstemr(char jobz, char range, int n, double *d, double *e, double vl, double vu, int il, int iu, int *m, double *w, doublecomplex *z, int ldz, int *nzc, int *isuppz, int *tryrac, int *info)
{
	if( !wrapper.fn[140] )
		wrapper.fn[140] = wrapper.Resolve("zstemr");
	(* reinterpret_cast<p_zstemr>(wrapper.fn[140]))( jobz, range, n, d, e, vl, vu, il, iu, m, w, z, ldz, nzc, isuppz, tryrac, info );

}

void zswap(int n, doublecomplex *x, int incx, doublecomplex *y, int incy)
{
	if( !wrapper.fn[141] )
		wrapper.fn[141] = wrapper.Resolve("zswap");
	(* reinterpret_cast<p_zswap>(wrapper.fn[141]))( n, x, incx, y, incy );

}

void zsymm(char side, char uplo, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[142] )
		wrapper.fn[142] = wrapper.Resolve("zsymm");
	(* reinterpret_cast<p_zsymm>(wrapper.fn[142]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc );

}

void zsyr2k(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb, doublecomplex *beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[143] )
		wrapper.fn[143] = wrapper.Resolve("zsyr2k");
	(* reinterpret_cast<p_zsyr2k>(wrapper.fn[143]))( uplo, transa, n, k, alpha, a, lda, b, ldb, beta, c, ldc );

}

void zsyrk(char uplo, char transa, int n, int k, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *beta, doublecomplex *c, int ldc)
{
	if( !wrapper.fn[144] )
		wrapper.fn[144] = wrapper.Resolve("zsyrk");
	(* reinterpret_cast<p_zsyrk>(wrapper.fn[144]))( uplo, transa, n, k, alpha, a, lda, beta, c, ldc );

}

void ztbmv(char uplo, char transa, char diag, int n, int ndiag, doublecomplex *a, int lda, doublecomplex *y, int incy)
{
	if( !wrapper.fn[145] )
		wrapper.fn[145] = wrapper.Resolve("ztbmv");
	(* reinterpret_cast<p_ztbmv>(wrapper.fn[145]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void ztbsv(char uplo, char transa, char diag, int n, int ndiag, doublecomplex *a, int lda, doublecomplex *y, int incy)
{
	if( !wrapper.fn[146] )
		wrapper.fn[146] = wrapper.Resolve("ztbsv");
	(* reinterpret_cast<p_ztbsv>(wrapper.fn[146]))( uplo, transa, diag, n, ndiag, a, lda, y, incy );

}

void ztpmv(char uplo, char transa, char diag, int n, doublecomplex *a, doublecomplex *y, int incy)
{
	if( !wrapper.fn[147] )
		wrapper.fn[147] = wrapper.Resolve("ztpmv");
	(* reinterpret_cast<p_ztpmv>(wrapper.fn[147]))( uplo, transa, diag, n, a, y, incy );

}

void ztpsv(char uplo, char transa, char diag, int n, doublecomplex *a, doublecomplex *y, int incy)
{
	if( !wrapper.fn[148] )
		wrapper.fn[148] = wrapper.Resolve("ztpsv");
	(* reinterpret_cast<p_ztpsv>(wrapper.fn[148]))( uplo, transa, diag, n, a, y, incy );

}

void ztrmm(char side, char uplo, char transa, char diag, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb)
{
	if( !wrapper.fn[149] )
		wrapper.fn[149] = wrapper.Resolve("ztrmm");
	(* reinterpret_cast<p_ztrmm>(wrapper.fn[149]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void ztrmv(char uplo, char transa, char diag, int n, doublecomplex *a, int lda, doublecomplex *y, int incy)
{
	if( !wrapper.fn[150] )
		wrapper.fn[150] = wrapper.Resolve("ztrmv");
	(* reinterpret_cast<p_ztrmv>(wrapper.fn[150]))( uplo, transa, diag, n, a, lda, y, incy );

}

void ztrsm(char side, char uplo, char transa, char diag, int m, int n, doublecomplex *alpha, doublecomplex *a, int lda, doublecomplex *b, int ldb)
{
	if( !wrapper.fn[151] )
		wrapper.fn[151] = wrapper.Resolve("ztrsm");
	(* reinterpret_cast<p_ztrsm>(wrapper.fn[151]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb );

}

void ztrsv(char uplo, char transa, char diag, int n, doublecomplex *a, int lda, doublecomplex *y, int incy)
{
	if( !wrapper.fn[152] )
		wrapper.fn[152] = wrapper.Resolve("ztrsv");
	(* reinterpret_cast<p_ztrsv>(wrapper.fn[152]))( uplo, transa, diag, n, a, lda, y, incy );

}



#ifdef __cplusplus 
}
#endif /* __cplusplus */

void BlasWrapper::DiscoverBlasLibrary( void )
{
    fileName = QString("libacml_dll.dll");
    blasLib->setFileName( fileName );
    if( ! blasLib->load() ){
	std::cout << blasLib->errorString().toStdString() << "\n";
    }
}

void* BlasWrapper::Resolve( const char* function_name )
{
    void *p;
    QString fname( function_name );
    if( capitalized )
	fname = fname.toUpper();
    //TODO: handle prefix, suffix.

    if( !(p = blasLib->resolve( fname.toAscii() ))){
	std::cout << blasLib->errorString().toStdString() 
	    << " while loading function: " << fname.toStdString() << "\n";
    }
    return p;
}


void BlasWrapper::InitFunctions( void )
{
	fn.resize( 153 );
}
