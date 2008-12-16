/* ../../../dependencies/lapack/src/chpev.f -- translated by f2c (version 20061008).
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

/* Subroutine */ int chpev_(char *jobz, char *uplo, integer *n, complex *ap, 
	real *w, complex *z__, integer *ldz, complex *work, real *rwork, 
	integer *info, ftnlen jobz_len, ftnlen uplo_len)
{
    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    real r__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real eps;
    static integer inde;
    static real anrm;
    static integer imax;
    static real rmin, rmax, sigma;
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    static integer iinfo;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static logical wantz;
    static integer iscale;
    extern doublereal clanhp_(char *, char *, integer *, complex *, real *, 
	    ftnlen, ftnlen), slamch_(char *, ftnlen);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *, ftnlen);
    static real bignum;
    static integer indtau;
    extern /* Subroutine */ int chptrd_(char *, integer *, complex *, real *, 
	    real *, complex *, integer *, ftnlen);
    static integer indrwk, indwrk;
    extern /* Subroutine */ int csteqr_(char *, integer *, real *, real *, 
	    complex *, integer *, real *, integer *, ftnlen), cupgtr_(char *, 
	    integer *, complex *, complex *, complex *, integer *, complex *, 
	    integer *, ftnlen), ssterf_(integer *, real *, real *, integer *);
    static real smlnum;


/*  -- LAPACK driver routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     March 31, 1993 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CHPEV computes all the eigenvalues and, optionally, eigenvectors of a */
/*  complex Hermitian matrix in packed storage. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) COMPLEX array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the Hermitian matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, AP is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the diagonal */
/*          and first superdiagonal of the tridiagonal matrix T overwrite */
/*          the corresponding elements of A, and if UPLO = 'L', the */
/*          diagonal and first subdiagonal of T overwrite the */
/*          corresponding elements of A. */

/*  W       (output) REAL array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) COMPLEX array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) COMPLEX array, dimension (max(1, 2*N-1)) */

/*  RWORK   (workspace) REAL array, dimension (max(1, 3*N-2)) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --ap;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --rwork;

    /* Function Body */
    wantz = lsame_(jobz, "V", (ftnlen)1, (ftnlen)1);

    *info = 0;
    if (! (wantz || lsame_(jobz, "N", (ftnlen)1, (ftnlen)1))) {
	*info = -1;
    } else if (! (lsame_(uplo, "L", (ftnlen)1, (ftnlen)1) || lsame_(uplo, 
	    "U", (ftnlen)1, (ftnlen)1))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -7;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CHPEV ", &i__1, (ftnlen)6);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = ap[1].r;
	rwork[1] = 1.f;
	if (wantz) {
	    i__1 = z_dim1 + 1;
	    z__[i__1].r = 1.f, z__[i__1].i = 0.f;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = slamch_("Safe minimum", (ftnlen)12);
    eps = slamch_("Precision", (ftnlen)9);
    smlnum = safmin / eps;
    bignum = 1.f / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = clanhp_("M", uplo, n, &ap[1], &rwork[1], (ftnlen)1, (ftnlen)1);
    iscale = 0;
    if (anrm > 0.f && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	i__1 = *n * (*n + 1) / 2;
	csscal_(&i__1, &sigma, &ap[1], &c__1);
    }

/*     Call CHPTRD to reduce Hermitian packed matrix to tridiagonal form. */

    inde = 1;
    indtau = 1;
    chptrd_(uplo, n, &ap[1], &w[1], &rwork[inde], &work[indtau], &iinfo, (
	    ftnlen)1);

/*     For eigenvalues only, call SSTERF.  For eigenvectors, first call */
/*     CUPGTR to generate the orthogonal matrix, then call CSTEQR. */

    if (! wantz) {
	ssterf_(n, &w[1], &rwork[inde], info);
    } else {
	indwrk = indtau + *n;
	cupgtr_(uplo, n, &ap[1], &work[indtau], &z__[z_offset], ldz, &work[
		indwrk], &iinfo, (ftnlen)1);
	indrwk = inde + *n;
	csteqr_(jobz, n, &w[1], &rwork[inde], &z__[z_offset], ldz, &rwork[
		indrwk], info, (ftnlen)1);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	r__1 = 1.f / sigma;
	sscal_(&imax, &r__1, &w[1], &c__1);
    }

    return 0;

/*     End of CHPEV */

} /* chpev_ */
