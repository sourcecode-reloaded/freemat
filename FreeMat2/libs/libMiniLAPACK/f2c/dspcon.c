/* ../double/dspcon.f -- translated by f2c (version 20041007).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int dspcon_(char *uplo, integer *n, doublereal *ap, integer *
	ipiv, doublereal *anorm, doublereal *rcond, doublereal *work, integer 
	*iwork, integer *info, ftnlen uplo_len)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, ip, kase;
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    static logical upper;
    extern /* Subroutine */ int dlacon_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *, integer *), xerbla_(char *, integer *, 
	    ftnlen);
    static doublereal ainvnm;
    extern /* Subroutine */ int dsptrs_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    ftnlen);


/*  -- LAPACK routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     March 31, 1993 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPCON estimates the reciprocal of the condition number (in the */
/*  1-norm) of a real symmetric packed matrix A using the factorization */
/*  A = U*D*U**T or A = L*D*L**T computed by DSPTRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as RCOND = 1 / (ANORM * norm(inv(A))). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by DSPTRF, stored as a */
/*          packed triangular matrix. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSPTRF. */

/*  ANORM   (input) DOUBLE PRECISION */
/*          The 1-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(ANORM * AINVNM), where AINVNM is an */
/*          estimate of the 1-norm of inv(A) computed in this routine. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

/*  IWORK    (workspace) INTEGER array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --ipiv;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U", (ftnlen)1, (ftnlen)1);
    if (! upper && ! lsame_(uplo, "L", (ftnlen)1, (ftnlen)1)) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*anorm < 0.) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPCON", &i__1, (ftnlen)6);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm <= 0.) {
	return 0;
    }

/*     Check that the diagonal matrix D is nonsingular. */

    if (upper) {

/*        Upper triangular storage: examine D from bottom to top */

	ip = *n * (*n + 1) / 2;
	for (i__ = *n; i__ >= 1; --i__) {
	    if (ipiv[i__] > 0 && ap[ip] == 0.) {
		return 0;
	    }
	    ip -= i__;
/* L10: */
	}
    } else {

/*        Lower triangular storage: examine D from top to bottom. */

	ip = 1;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ipiv[i__] > 0 && ap[ip] == 0.) {
		return 0;
	    }
	    ip = ip + *n - i__ + 1;
/* L20: */
	}
    }

/*     Estimate the 1-norm of the inverse. */

    kase = 0;
L30:
    dlacon_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase);
    if (kase != 0) {

/*        Multiply by inv(L*D*L') or inv(U*D*U'). */

	dsptrs_(uplo, n, &c__1, &ap[1], &ipiv[1], &work[1], n, info, (ftnlen)
		1);
	goto L30;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

    return 0;

/*     End of DSPCON */

} /* dspcon_ */

