//This is a generated file. Do not edit!
#include <QString>
#include <QLibrary> 
#include "blas_wrap.h"
#include "blas_dyn_link.h"
#include <iostream>
#include <vector>

BlasWrapper wrapper;

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */ 
typedef void (* p_caxpy )(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_ccopy )(int *n, complex *x, int *incx, complex *y, int *incy);
typedef complex (* p_cdotc )(complex *ret_val, int *n, complex *x, int *incx, complex *y, int *incy);
typedef complex (* p_cdotu )(complex *ret_val, int *n, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_cgbmv )(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
typedef void (* p_cgemm )(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_cgemv )(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
typedef void (* p_cgerc )(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
typedef void (* p_cgeru )(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
typedef void (* p_chbmv )(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_chemm )(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_chemv )(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_cher )(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len);
typedef void (* p_cher2 )(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len);
typedef void (* p_cher2k )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_cherk )(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_chpmv )(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_chpr )(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len);
typedef void (* p_chpr2 )(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len);
typedef void (* p_crotg )(complex *a, complex *b, float *c, complex *s);
typedef void (* p_cscal )(int *n, complex *alpha, complex *x, int *incx);
typedef void (* p_csscal )(int *n, float *alpha, complex *x, int *incx);
typedef void (* p_cswap )(int *n, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_csymm )(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_csyr2k )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_csyrk )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ctbmv )(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctbsv )(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctpmv )(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctpsv )(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ctrmv )(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ctrsv )(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef double (* p_dasum )(int *n, double *x, int *incx);
typedef void (* p_daxpy )(int *n, double *alpha, double *x, int *incx, double *y, int *incy);
typedef double (* p_dcabs1 )(doublecomplex *z);
typedef void (* p_dcopy )(int *n, double *x, int *incx, double *y, int *incy);
typedef double (* p_ddot )(int *n, double *x, int *incx, double *y, int *incy);
typedef void (* p_dgbmv )(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
typedef void (* p_dgemm )(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_dgemv )(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
typedef void (* p_dger )(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda);
typedef double (* p_dnrm2 )(int *n, double *x, int *incx);
typedef void (* p_drot )(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s);
typedef void (* p_drotg )(double *a, double *b, double *c, double *s);
typedef void (* p_dsbmv )(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dscal )(int *n, double *alpha, double *x, int *incx);
typedef void (* p_dspmv )(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dspr )(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len);
typedef void (* p_dspr2 )(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len);
typedef void (* p_dswap )(int *n, double *x, int *incx, double *y, int *incy);
typedef void (* p_dsymm )(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_dsymv )(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dsyr )(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len);
typedef void (* p_dsyr2 )(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len);
typedef void (* p_dsyr2k )(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_dsyrk )(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_dtbmv )(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtbsv )(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtpmv )(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtpsv )(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_dtrmv )(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_dtrsv )(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef double (* p_dzasum )(int *n, doublecomplex *x, int *incx);
typedef double (* p_dznrm2 )(int *n, doublecomplex *x, int *incx);
typedef int (* p_icamax )(int *n, complex *x, int *incx);
typedef int (* p_idamax )(int *n, double *x, int *incx);
typedef int (* p_isamax )(int *n, float *x, int *incx);
typedef int (* p_izamax )(int *n, doublecomplex *x, int *incx);
typedef double (* p_sasum )(int *n, float *x, int *incx);
typedef void (* p_saxpy )(int *n, float *alpha, float *x, int *incx, float *y, int *incy);
typedef double (* p_scasum )(int *n, complex *x, int *incx);
typedef double (* p_scnrm2 )(int *n, complex *x, int *incx);
typedef void (* p_scopy )(int *n, float *x, int *incx, float *y, int *incy);
typedef double (* p_sdot )(int *n, float *x, int *incx, float *y, int *incy);
typedef void (* p_sgbmv )(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
typedef void (* p_sgemm )(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_sgemv )(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
typedef void (* p_sger )(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda);
typedef double (* p_snrm2 )(int *n, float *x, int *incx);
typedef void (* p_srot )(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s);
typedef void (* p_srotg )(float *a, float *b, float *c, float *s);
typedef void (* p_ssbmv )(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_sscal )(int *n, float *alpha, float *x, int *incx);
typedef void (* p_sspmv )(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_sspr )(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len);
typedef void (* p_sspr2 )(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len);
typedef void (* p_sswap )(int *n, float *x, int *incx, float *y, int *incy);
typedef void (* p_ssymm )(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_ssymv )(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_ssyr )(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len);
typedef void (* p_ssyr2 )(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len);
typedef void (* p_ssyr2k )(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ssyrk )(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_stbmv )(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stbsv )(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stpmv )(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stpsv )(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_strmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_strmv )(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_strsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_strsv )(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_zaxpy )(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zcopy )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef doublecomplex (* p_zdotc )(doublecomplex *ret_val, int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef doublecomplex (* p_zdotu )(doublecomplex *ret_val, int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zdscal )(int *n, double *alpha, doublecomplex *x, int *incx);
typedef void (* p_zgbmv )(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
typedef void (* p_zgemm )(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_zgemv )(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
typedef void (* p_zgerc )(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
typedef void (* p_zgeru )(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
typedef void (* p_zhbmv )(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zhemm )(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_zhemv )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zher )(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len);
typedef void (* p_zher2 )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len);
typedef void (* p_zher2k )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zherk )(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zhpmv )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zhpr )(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len);
typedef void (* p_zhpr2 )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len);
typedef void (* p_zrotg )(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s);
typedef void (* p_zscal )(int *n, doublecomplex *alpha, doublecomplex *x, int *incx);
typedef void (* p_zswap )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zsymm )(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_zsyr2k )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zsyrk )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ztbmv )(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztbsv )(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztpmv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztpsv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ztrmv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ztrsv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_lsame )(char *ca, char *cb, int ca_len, int cb_len);


void caxpy_(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[0] )
		wrapper.fn[0] = wrapper.Resolve("caxpy");
	(* reinterpret_cast<p_caxpy>(wrapper.fn[0]))( n, alpha, x, incx, y, incy );

}

void ccopy_(int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[1] )
		wrapper.fn[1] = wrapper.Resolve("ccopy");
	(* reinterpret_cast<p_ccopy>(wrapper.fn[1]))( n, x, incx, y, incy );

}

complex cdotc_(complex *ret_val, int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[2] )
		wrapper.fn[2] = wrapper.Resolve("cdotc");
	return (* reinterpret_cast<p_cdotc>(wrapper.fn[2]))( ret_val, n, x, incx, y, incy );

}

complex cdotu_(complex *ret_val, int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[3] )
		wrapper.fn[3] = wrapper.Resolve("cdotu");
	return (* reinterpret_cast<p_cdotu>(wrapper.fn[3]))( ret_val, n, x, incx, y, incy );

}

void cgbmv_(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[4] )
		wrapper.fn[4] = wrapper.Resolve("cgbmv");
	(* reinterpret_cast<p_cgbmv>(wrapper.fn[4]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void cgemm_(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[5] )
		wrapper.fn[5] = wrapper.Resolve("cgemm");
	(* reinterpret_cast<p_cgemm>(wrapper.fn[5]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void cgemv_(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[6] )
		wrapper.fn[6] = wrapper.Resolve("cgemv");
	(* reinterpret_cast<p_cgemv>(wrapper.fn[6]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void cgerc_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda)
{
	if( !wrapper.fn[7] )
		wrapper.fn[7] = wrapper.Resolve("cgerc");
	(* reinterpret_cast<p_cgerc>(wrapper.fn[7]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void cgeru_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda)
{
	if( !wrapper.fn[8] )
		wrapper.fn[8] = wrapper.Resolve("cgeru");
	(* reinterpret_cast<p_cgeru>(wrapper.fn[8]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void chbmv_(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[9] )
		wrapper.fn[9] = wrapper.Resolve("chbmv");
	(* reinterpret_cast<p_chbmv>(wrapper.fn[9]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void chemm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[10] )
		wrapper.fn[10] = wrapper.Resolve("chemm");
	(* reinterpret_cast<p_chemm>(wrapper.fn[10]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void chemv_(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[11] )
		wrapper.fn[11] = wrapper.Resolve("chemv");
	(* reinterpret_cast<p_chemv>(wrapper.fn[11]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void cher_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[12] )
		wrapper.fn[12] = wrapper.Resolve("cher");
	(* reinterpret_cast<p_cher>(wrapper.fn[12]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void cher2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[13] )
		wrapper.fn[13] = wrapper.Resolve("cher2");
	(* reinterpret_cast<p_cher2>(wrapper.fn[13]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void cher2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[14] )
		wrapper.fn[14] = wrapper.Resolve("cher2k");
	(* reinterpret_cast<p_cher2k>(wrapper.fn[14]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void cherk_(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[15] )
		wrapper.fn[15] = wrapper.Resolve("cherk");
	(* reinterpret_cast<p_cherk>(wrapper.fn[15]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void chpmv_(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[16] )
		wrapper.fn[16] = wrapper.Resolve("chpmv");
	(* reinterpret_cast<p_chpmv>(wrapper.fn[16]))( uplo, n, alpha, a, x, incx, beta, y, incy, uplo_len );

}

void chpr_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len)
{
	if( !wrapper.fn[17] )
		wrapper.fn[17] = wrapper.Resolve("chpr");
	(* reinterpret_cast<p_chpr>(wrapper.fn[17]))( uplo, n, alpha, x, incx, a, uplo_len );

}

void chpr2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len)
{
	if( !wrapper.fn[18] )
		wrapper.fn[18] = wrapper.Resolve("chpr2");
	(* reinterpret_cast<p_chpr2>(wrapper.fn[18]))( uplo, n, alpha, x, incx, y, incy, a, uplo_len );

}

void crotg_(complex *a, complex *b, float *c, complex *s)
{
	if( !wrapper.fn[19] )
		wrapper.fn[19] = wrapper.Resolve("crotg");
	(* reinterpret_cast<p_crotg>(wrapper.fn[19]))( a, b, c, s );

}

void cscal_(int *n, complex *alpha, complex *x, int *incx)
{
	if( !wrapper.fn[20] )
		wrapper.fn[20] = wrapper.Resolve("cscal");
	(* reinterpret_cast<p_cscal>(wrapper.fn[20]))( n, alpha, x, incx );

}

void csscal_(int *n, float *alpha, complex *x, int *incx)
{
	if( !wrapper.fn[21] )
		wrapper.fn[21] = wrapper.Resolve("csscal");
	(* reinterpret_cast<p_csscal>(wrapper.fn[21]))( n, alpha, x, incx );

}

void cswap_(int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[22] )
		wrapper.fn[22] = wrapper.Resolve("cswap");
	(* reinterpret_cast<p_cswap>(wrapper.fn[22]))( n, x, incx, y, incy );

}

void csymm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[23] )
		wrapper.fn[23] = wrapper.Resolve("csymm");
	(* reinterpret_cast<p_csymm>(wrapper.fn[23]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void csyr2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[24] )
		wrapper.fn[24] = wrapper.Resolve("csyr2k");
	(* reinterpret_cast<p_csyr2k>(wrapper.fn[24]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void csyrk_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[25] )
		wrapper.fn[25] = wrapper.Resolve("csyrk");
	(* reinterpret_cast<p_csyrk>(wrapper.fn[25]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void ctbmv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[26] )
		wrapper.fn[26] = wrapper.Resolve("ctbmv");
	(* reinterpret_cast<p_ctbmv>(wrapper.fn[26]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctbsv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[27] )
		wrapper.fn[27] = wrapper.Resolve("ctbsv");
	(* reinterpret_cast<p_ctbsv>(wrapper.fn[27]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctpmv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[28] )
		wrapper.fn[28] = wrapper.Resolve("ctpmv");
	(* reinterpret_cast<p_ctpmv>(wrapper.fn[28]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ctpsv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[29] )
		wrapper.fn[29] = wrapper.Resolve("ctpsv");
	(* reinterpret_cast<p_ctpsv>(wrapper.fn[29]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ctrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[30] )
		wrapper.fn[30] = wrapper.Resolve("ctrmm");
	(* reinterpret_cast<p_ctrmm>(wrapper.fn[30]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ctrmv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[31] )
		wrapper.fn[31] = wrapper.Resolve("ctrmv");
	(* reinterpret_cast<p_ctrmv>(wrapper.fn[31]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[32] )
		wrapper.fn[32] = wrapper.Resolve("ctrsm");
	(* reinterpret_cast<p_ctrsm>(wrapper.fn[32]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ctrsv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[33] )
		wrapper.fn[33] = wrapper.Resolve("ctrsv");
	(* reinterpret_cast<p_ctrsv>(wrapper.fn[33]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

double dasum_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[34] )
		wrapper.fn[34] = wrapper.Resolve("dasum");
	return (* reinterpret_cast<p_dasum>(wrapper.fn[34]))( n, x, incx );

}

void daxpy_(int *n, double *alpha, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[35] )
		wrapper.fn[35] = wrapper.Resolve("daxpy");
	(* reinterpret_cast<p_daxpy>(wrapper.fn[35]))( n, alpha, x, incx, y, incy );

}

double dcabs1_(doublecomplex *z)
{
	if( !wrapper.fn[36] )
		wrapper.fn[36] = wrapper.Resolve("dcabs1");
	return (* reinterpret_cast<p_dcabs1>(wrapper.fn[36]))( z );

}

void dcopy_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[37] )
		wrapper.fn[37] = wrapper.Resolve("dcopy");
	(* reinterpret_cast<p_dcopy>(wrapper.fn[37]))( n, x, incx, y, incy );

}

double ddot_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[38] )
		wrapper.fn[38] = wrapper.Resolve("ddot");
	return (* reinterpret_cast<p_ddot>(wrapper.fn[38]))( n, x, incx, y, incy );

}

void dgbmv_(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len)
{
	if( !wrapper.fn[39] )
		wrapper.fn[39] = wrapper.Resolve("dgbmv");
	(* reinterpret_cast<p_dgbmv>(wrapper.fn[39]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void dgemm_(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[40] )
		wrapper.fn[40] = wrapper.Resolve("dgemm");
	(* reinterpret_cast<p_dgemm>(wrapper.fn[40]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void dgemv_(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len)
{
	if( !wrapper.fn[41] )
		wrapper.fn[41] = wrapper.Resolve("dgemv");
	(* reinterpret_cast<p_dgemv>(wrapper.fn[41]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda)
{
	if( !wrapper.fn[42] )
		wrapper.fn[42] = wrapper.Resolve("dger");
	(* reinterpret_cast<p_dger>(wrapper.fn[42]))( m, n, alpha, x, incx, y, incy, a, lda );

}

double dnrm2_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[43] )
		wrapper.fn[43] = wrapper.Resolve("dnrm2");
	return (* reinterpret_cast<p_dnrm2>(wrapper.fn[43]))( n, x, incx );

}

void drot_(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s)
{
	if( !wrapper.fn[44] )
		wrapper.fn[44] = wrapper.Resolve("drot");
	(* reinterpret_cast<p_drot>(wrapper.fn[44]))( n, x, incx, y, incy, c, s );

}

void drotg_(double *a, double *b, double *c, double *s)
{
	if( !wrapper.fn[45] )
		wrapper.fn[45] = wrapper.Resolve("drotg");
	(* reinterpret_cast<p_drotg>(wrapper.fn[45]))( a, b, c, s );

}

void dsbmv_(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[46] )
		wrapper.fn[46] = wrapper.Resolve("dsbmv");
	(* reinterpret_cast<p_dsbmv>(wrapper.fn[46]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void dscal_(int *n, double *alpha, double *x, int *incx)
{
	if( !wrapper.fn[47] )
		wrapper.fn[47] = wrapper.Resolve("dscal");
	(* reinterpret_cast<p_dscal>(wrapper.fn[47]))( n, alpha, x, incx );

}

void dspmv_(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[48] )
		wrapper.fn[48] = wrapper.Resolve("dspmv");
	(* reinterpret_cast<p_dspmv>(wrapper.fn[48]))( uplo, n, alpha, ap, x, incx, beta, y, incy, uplo_len );

}

void dspr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len)
{
	if( !wrapper.fn[49] )
		wrapper.fn[49] = wrapper.Resolve("dspr");
	(* reinterpret_cast<p_dspr>(wrapper.fn[49]))( uplo, n, alpha, x, incx, ap, uplo_len );

}

void dspr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len)
{
	if( !wrapper.fn[50] )
		wrapper.fn[50] = wrapper.Resolve("dspr2");
	(* reinterpret_cast<p_dspr2>(wrapper.fn[50]))( uplo, n, alpha, x, incx, y, incy, ap, uplo_len );

}

void dswap_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[51] )
		wrapper.fn[51] = wrapper.Resolve("dswap");
	(* reinterpret_cast<p_dswap>(wrapper.fn[51]))( n, x, incx, y, incy );

}

void dsymm_(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[52] )
		wrapper.fn[52] = wrapper.Resolve("dsymm");
	(* reinterpret_cast<p_dsymm>(wrapper.fn[52]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[53] )
		wrapper.fn[53] = wrapper.Resolve("dsymv");
	(* reinterpret_cast<p_dsymv>(wrapper.fn[53]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void dsyr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[54] )
		wrapper.fn[54] = wrapper.Resolve("dsyr");
	(* reinterpret_cast<p_dsyr>(wrapper.fn[54]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void dsyr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[55] )
		wrapper.fn[55] = wrapper.Resolve("dsyr2");
	(* reinterpret_cast<p_dsyr2>(wrapper.fn[55]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void dsyr2k_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[56] )
		wrapper.fn[56] = wrapper.Resolve("dsyr2k");
	(* reinterpret_cast<p_dsyr2k>(wrapper.fn[56]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void dsyrk_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[57] )
		wrapper.fn[57] = wrapper.Resolve("dsyrk");
	(* reinterpret_cast<p_dsyrk>(wrapper.fn[57]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void dtbmv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[58] )
		wrapper.fn[58] = wrapper.Resolve("dtbmv");
	(* reinterpret_cast<p_dtbmv>(wrapper.fn[58]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtbsv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[59] )
		wrapper.fn[59] = wrapper.Resolve("dtbsv");
	(* reinterpret_cast<p_dtbsv>(wrapper.fn[59]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtpmv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[60] )
		wrapper.fn[60] = wrapper.Resolve("dtpmv");
	(* reinterpret_cast<p_dtpmv>(wrapper.fn[60]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void dtpsv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[61] )
		wrapper.fn[61] = wrapper.Resolve("dtpsv");
	(* reinterpret_cast<p_dtpsv>(wrapper.fn[61]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void dtrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[62] )
		wrapper.fn[62] = wrapper.Resolve("dtrmm");
	(* reinterpret_cast<p_dtrmm>(wrapper.fn[62]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void dtrmv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[63] )
		wrapper.fn[63] = wrapper.Resolve("dtrmv");
	(* reinterpret_cast<p_dtrmv>(wrapper.fn[63]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[64] )
		wrapper.fn[64] = wrapper.Resolve("dtrsm");
	(* reinterpret_cast<p_dtrsm>(wrapper.fn[64]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void dtrsv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[65] )
		wrapper.fn[65] = wrapper.Resolve("dtrsv");
	(* reinterpret_cast<p_dtrsv>(wrapper.fn[65]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

double dzasum_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[66] )
		wrapper.fn[66] = wrapper.Resolve("dzasum");
	return (* reinterpret_cast<p_dzasum>(wrapper.fn[66]))( n, x, incx );

}

double dznrm2_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[67] )
		wrapper.fn[67] = wrapper.Resolve("dznrm2");
	return (* reinterpret_cast<p_dznrm2>(wrapper.fn[67]))( n, x, incx );

}

int icamax_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[68] )
		wrapper.fn[68] = wrapper.Resolve("icamax");
	return (* reinterpret_cast<p_icamax>(wrapper.fn[68]))( n, x, incx );

}

int idamax_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[69] )
		wrapper.fn[69] = wrapper.Resolve("idamax");
	return (* reinterpret_cast<p_idamax>(wrapper.fn[69]))( n, x, incx );

}

int isamax_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[70] )
		wrapper.fn[70] = wrapper.Resolve("isamax");
	return (* reinterpret_cast<p_isamax>(wrapper.fn[70]))( n, x, incx );

}

int izamax_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[71] )
		wrapper.fn[71] = wrapper.Resolve("izamax");
	return (* reinterpret_cast<p_izamax>(wrapper.fn[71]))( n, x, incx );

}

double sasum_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[72] )
		wrapper.fn[72] = wrapper.Resolve("sasum");
	return (* reinterpret_cast<p_sasum>(wrapper.fn[72]))( n, x, incx );

}

void saxpy_(int *n, float *alpha, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[73] )
		wrapper.fn[73] = wrapper.Resolve("saxpy");
	(* reinterpret_cast<p_saxpy>(wrapper.fn[73]))( n, alpha, x, incx, y, incy );

}

double scasum_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[74] )
		wrapper.fn[74] = wrapper.Resolve("scasum");
	return (* reinterpret_cast<p_scasum>(wrapper.fn[74]))( n, x, incx );

}

double scnrm2_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[75] )
		wrapper.fn[75] = wrapper.Resolve("scnrm2");
	return (* reinterpret_cast<p_scnrm2>(wrapper.fn[75]))( n, x, incx );

}

void scopy_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[76] )
		wrapper.fn[76] = wrapper.Resolve("scopy");
	(* reinterpret_cast<p_scopy>(wrapper.fn[76]))( n, x, incx, y, incy );

}

double sdot_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[77] )
		wrapper.fn[77] = wrapper.Resolve("sdot");
	return (* reinterpret_cast<p_sdot>(wrapper.fn[77]))( n, x, incx, y, incy );

}

void sgbmv_(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len)
{
	if( !wrapper.fn[78] )
		wrapper.fn[78] = wrapper.Resolve("sgbmv");
	(* reinterpret_cast<p_sgbmv>(wrapper.fn[78]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void sgemm_(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[79] )
		wrapper.fn[79] = wrapper.Resolve("sgemm");
	(* reinterpret_cast<p_sgemm>(wrapper.fn[79]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void sgemv_(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len)
{
	if( !wrapper.fn[80] )
		wrapper.fn[80] = wrapper.Resolve("sgemv");
	(* reinterpret_cast<p_sgemv>(wrapper.fn[80]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void sger_(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda)
{
	if( !wrapper.fn[81] )
		wrapper.fn[81] = wrapper.Resolve("sger");
	(* reinterpret_cast<p_sger>(wrapper.fn[81]))( m, n, alpha, x, incx, y, incy, a, lda );

}

double snrm2_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[82] )
		wrapper.fn[82] = wrapper.Resolve("snrm2");
	return (* reinterpret_cast<p_snrm2>(wrapper.fn[82]))( n, x, incx );

}

void srot_(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s)
{
	if( !wrapper.fn[83] )
		wrapper.fn[83] = wrapper.Resolve("srot");
	(* reinterpret_cast<p_srot>(wrapper.fn[83]))( n, x, incx, y, incy, c, s );

}

void srotg_(float *a, float *b, float *c, float *s)
{
	if( !wrapper.fn[84] )
		wrapper.fn[84] = wrapper.Resolve("srotg");
	(* reinterpret_cast<p_srotg>(wrapper.fn[84]))( a, b, c, s );

}

void ssbmv_(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[85] )
		wrapper.fn[85] = wrapper.Resolve("ssbmv");
	(* reinterpret_cast<p_ssbmv>(wrapper.fn[85]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void sscal_(int *n, float *alpha, float *x, int *incx)
{
	if( !wrapper.fn[86] )
		wrapper.fn[86] = wrapper.Resolve("sscal");
	(* reinterpret_cast<p_sscal>(wrapper.fn[86]))( n, alpha, x, incx );

}

void sspmv_(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[87] )
		wrapper.fn[87] = wrapper.Resolve("sspmv");
	(* reinterpret_cast<p_sspmv>(wrapper.fn[87]))( uplo, n, alpha, ap, x, incx, beta, y, incy, uplo_len );

}

void sspr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len)
{
	if( !wrapper.fn[88] )
		wrapper.fn[88] = wrapper.Resolve("sspr");
	(* reinterpret_cast<p_sspr>(wrapper.fn[88]))( uplo, n, alpha, x, incx, ap, uplo_len );

}

void sspr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len)
{
	if( !wrapper.fn[89] )
		wrapper.fn[89] = wrapper.Resolve("sspr2");
	(* reinterpret_cast<p_sspr2>(wrapper.fn[89]))( uplo, n, alpha, x, incx, y, incy, ap, uplo_len );

}

void sswap_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[90] )
		wrapper.fn[90] = wrapper.Resolve("sswap");
	(* reinterpret_cast<p_sswap>(wrapper.fn[90]))( n, x, incx, y, incy );

}

void ssymm_(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[91] )
		wrapper.fn[91] = wrapper.Resolve("ssymm");
	(* reinterpret_cast<p_ssymm>(wrapper.fn[91]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void ssymv_(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[92] )
		wrapper.fn[92] = wrapper.Resolve("ssymv");
	(* reinterpret_cast<p_ssymv>(wrapper.fn[92]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void ssyr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[93] )
		wrapper.fn[93] = wrapper.Resolve("ssyr");
	(* reinterpret_cast<p_ssyr>(wrapper.fn[93]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void ssyr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[94] )
		wrapper.fn[94] = wrapper.Resolve("ssyr2");
	(* reinterpret_cast<p_ssyr2>(wrapper.fn[94]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void ssyr2k_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[95] )
		wrapper.fn[95] = wrapper.Resolve("ssyr2k");
	(* reinterpret_cast<p_ssyr2k>(wrapper.fn[95]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void ssyrk_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[96] )
		wrapper.fn[96] = wrapper.Resolve("ssyrk");
	(* reinterpret_cast<p_ssyrk>(wrapper.fn[96]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void stbmv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[97] )
		wrapper.fn[97] = wrapper.Resolve("stbmv");
	(* reinterpret_cast<p_stbmv>(wrapper.fn[97]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void stbsv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[98] )
		wrapper.fn[98] = wrapper.Resolve("stbsv");
	(* reinterpret_cast<p_stbsv>(wrapper.fn[98]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void stpmv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[99] )
		wrapper.fn[99] = wrapper.Resolve("stpmv");
	(* reinterpret_cast<p_stpmv>(wrapper.fn[99]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void stpsv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[100] )
		wrapper.fn[100] = wrapper.Resolve("stpsv");
	(* reinterpret_cast<p_stpsv>(wrapper.fn[100]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void strmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[101] )
		wrapper.fn[101] = wrapper.Resolve("strmm");
	(* reinterpret_cast<p_strmm>(wrapper.fn[101]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void strmv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[102] )
		wrapper.fn[102] = wrapper.Resolve("strmv");
	(* reinterpret_cast<p_strmv>(wrapper.fn[102]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void strsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[103] )
		wrapper.fn[103] = wrapper.Resolve("strsm");
	(* reinterpret_cast<p_strsm>(wrapper.fn[103]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void strsv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[104] )
		wrapper.fn[104] = wrapper.Resolve("strsv");
	(* reinterpret_cast<p_strsv>(wrapper.fn[104]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void zaxpy_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[105] )
		wrapper.fn[105] = wrapper.Resolve("zaxpy");
	(* reinterpret_cast<p_zaxpy>(wrapper.fn[105]))( n, alpha, x, incx, y, incy );

}

void zcopy_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[106] )
		wrapper.fn[106] = wrapper.Resolve("zcopy");
	(* reinterpret_cast<p_zcopy>(wrapper.fn[106]))( n, x, incx, y, incy );

}

doublecomplex zdotc_(doublecomplex *ret_val, int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[107] )
		wrapper.fn[107] = wrapper.Resolve("zdotc");
	return (* reinterpret_cast<p_zdotc>(wrapper.fn[107]))( ret_val, n, x, incx, y, incy );

}

doublecomplex zdotu_(doublecomplex *ret_val, int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[108] )
		wrapper.fn[108] = wrapper.Resolve("zdotu");
	return (* reinterpret_cast<p_zdotu>(wrapper.fn[108]))( ret_val, n, x, incx, y, incy );

}

void zdscal_(int *n, double *alpha, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[109] )
		wrapper.fn[109] = wrapper.Resolve("zdscal");
	(* reinterpret_cast<p_zdscal>(wrapper.fn[109]))( n, alpha, x, incx );

}

void zgbmv_(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[110] )
		wrapper.fn[110] = wrapper.Resolve("zgbmv");
	(* reinterpret_cast<p_zgbmv>(wrapper.fn[110]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void zgemm_(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[111] )
		wrapper.fn[111] = wrapper.Resolve("zgemm");
	(* reinterpret_cast<p_zgemm>(wrapper.fn[111]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void zgemv_(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[112] )
		wrapper.fn[112] = wrapper.Resolve("zgemv");
	(* reinterpret_cast<p_zgemv>(wrapper.fn[112]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void zgerc_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda)
{
	if( !wrapper.fn[113] )
		wrapper.fn[113] = wrapper.Resolve("zgerc");
	(* reinterpret_cast<p_zgerc>(wrapper.fn[113]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zgeru_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda)
{
	if( !wrapper.fn[114] )
		wrapper.fn[114] = wrapper.Resolve("zgeru");
	(* reinterpret_cast<p_zgeru>(wrapper.fn[114]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zhbmv_(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[115] )
		wrapper.fn[115] = wrapper.Resolve("zhbmv");
	(* reinterpret_cast<p_zhbmv>(wrapper.fn[115]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void zhemm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[116] )
		wrapper.fn[116] = wrapper.Resolve("zhemm");
	(* reinterpret_cast<p_zhemm>(wrapper.fn[116]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void zhemv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[117] )
		wrapper.fn[117] = wrapper.Resolve("zhemv");
	(* reinterpret_cast<p_zhemv>(wrapper.fn[117]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void zher_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[118] )
		wrapper.fn[118] = wrapper.Resolve("zher");
	(* reinterpret_cast<p_zher>(wrapper.fn[118]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void zher2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[119] )
		wrapper.fn[119] = wrapper.Resolve("zher2");
	(* reinterpret_cast<p_zher2>(wrapper.fn[119]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void zher2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[120] )
		wrapper.fn[120] = wrapper.Resolve("zher2k");
	(* reinterpret_cast<p_zher2k>(wrapper.fn[120]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void zherk_(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[121] )
		wrapper.fn[121] = wrapper.Resolve("zherk");
	(* reinterpret_cast<p_zherk>(wrapper.fn[121]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void zhpmv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[122] )
		wrapper.fn[122] = wrapper.Resolve("zhpmv");
	(* reinterpret_cast<p_zhpmv>(wrapper.fn[122]))( uplo, n, alpha, a, x, incx, beta, y, incy, uplo_len );

}

void zhpr_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len)
{
	if( !wrapper.fn[123] )
		wrapper.fn[123] = wrapper.Resolve("zhpr");
	(* reinterpret_cast<p_zhpr>(wrapper.fn[123]))( uplo, n, alpha, x, incx, a, uplo_len );

}

void zhpr2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len)
{
	if( !wrapper.fn[124] )
		wrapper.fn[124] = wrapper.Resolve("zhpr2");
	(* reinterpret_cast<p_zhpr2>(wrapper.fn[124]))( uplo, n, alpha, x, incx, y, incy, a, uplo_len );

}

void zrotg_(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s)
{
	if( !wrapper.fn[125] )
		wrapper.fn[125] = wrapper.Resolve("zrotg");
	(* reinterpret_cast<p_zrotg>(wrapper.fn[125]))( a, b, c, s );

}

void zscal_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[126] )
		wrapper.fn[126] = wrapper.Resolve("zscal");
	(* reinterpret_cast<p_zscal>(wrapper.fn[126]))( n, alpha, x, incx );

}

void zswap_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[127] )
		wrapper.fn[127] = wrapper.Resolve("zswap");
	(* reinterpret_cast<p_zswap>(wrapper.fn[127]))( n, x, incx, y, incy );

}

void zsymm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[128] )
		wrapper.fn[128] = wrapper.Resolve("zsymm");
	(* reinterpret_cast<p_zsymm>(wrapper.fn[128]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void zsyr2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[129] )
		wrapper.fn[129] = wrapper.Resolve("zsyr2k");
	(* reinterpret_cast<p_zsyr2k>(wrapper.fn[129]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void zsyrk_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[130] )
		wrapper.fn[130] = wrapper.Resolve("zsyrk");
	(* reinterpret_cast<p_zsyrk>(wrapper.fn[130]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void ztbmv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[131] )
		wrapper.fn[131] = wrapper.Resolve("ztbmv");
	(* reinterpret_cast<p_ztbmv>(wrapper.fn[131]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztbsv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[132] )
		wrapper.fn[132] = wrapper.Resolve("ztbsv");
	(* reinterpret_cast<p_ztbsv>(wrapper.fn[132]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztpmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[133] )
		wrapper.fn[133] = wrapper.Resolve("ztpmv");
	(* reinterpret_cast<p_ztpmv>(wrapper.fn[133]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ztpsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[134] )
		wrapper.fn[134] = wrapper.Resolve("ztpsv");
	(* reinterpret_cast<p_ztpsv>(wrapper.fn[134]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ztrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[135] )
		wrapper.fn[135] = wrapper.Resolve("ztrmm");
	(* reinterpret_cast<p_ztrmm>(wrapper.fn[135]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ztrmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[136] )
		wrapper.fn[136] = wrapper.Resolve("ztrmv");
	(* reinterpret_cast<p_ztrmv>(wrapper.fn[136]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[137] )
		wrapper.fn[137] = wrapper.Resolve("ztrsm");
	(* reinterpret_cast<p_ztrsm>(wrapper.fn[137]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ztrsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[138] )
		wrapper.fn[138] = wrapper.Resolve("ztrsv");
	(* reinterpret_cast<p_ztrsv>(wrapper.fn[138]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void lsame_(char *ca, char *cb, int ca_len, int cb_len)
{
	if( !wrapper.fn[139] )
		wrapper.fn[139] = wrapper.Resolve("lsame");
	(* reinterpret_cast<p_lsame>(wrapper.fn[139]))( ca, cb, ca_len, cb_len );

}



#ifdef __cplusplus 
}
#endif /* __cplusplus */
