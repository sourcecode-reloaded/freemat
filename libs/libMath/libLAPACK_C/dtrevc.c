/* ../../../dependencies/lapack/src/dtrevc.f -- translated by f2c (version 20061008).
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

static logical c_false = FALSE_;
static integer c__1 = 1;
static doublereal c_b22 = 1.;
static doublereal c_b25 = 0.;
static integer c__2 = 2;
static logical c_true = TRUE_;

/* Subroutine */ int dtrevc_(char *side, char *howmny, logical *select, 
	integer *n, doublereal *t, integer *ldt, doublereal *vl, integer *
	ldvl, doublereal *vr, integer *ldvr, integer *mm, integer *m, 
	doublereal *work, integer *info, ftnlen side_len, ftnlen howmny_len)
{
    /* System generated locals */
    integer t_dim1, t_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1, 
	    i__2, i__3;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k;
    static doublereal x[4]	/* was [2][2] */;
    static integer j1, j2, n2, ii, ki, ip, is;
    static doublereal wi, wr, rec, ulp, beta, emax;
    static logical pair;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    static logical allv;
    static integer ierr;
    static doublereal unfl, ovfl, smin;
    static logical over;
    static doublereal vmax;
    static integer jnxt;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    static doublereal scale;
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, ftnlen);
    static doublereal remax;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    static logical leftv, bothv;
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *);
    static doublereal vcrit;
    static logical somev;
    static doublereal xnorm;
    extern /* Subroutine */ int dlaln2_(logical *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     doublereal *, doublereal *, integer *, doublereal *, doublereal *
	    , doublereal *, integer *, doublereal *, doublereal *, integer *),
	     dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *, ftnlen);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *, ftnlen);
    static doublereal bignum;
    static logical rightv;
    static doublereal smlnum;


/*  -- LAPACK routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     June 30, 1999 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTREVC computes some or all of the right and/or left eigenvectors of */
/*  a real upper quasi-triangular matrix T. */

/*  The right eigenvector x and the left eigenvector y of T corresponding */
/*  to an eigenvalue w are defined by: */

/*               T*x = w*x,     y'*T = w*y' */

/*  where y' denotes the conjugate transpose of the vector y. */

/*  If all eigenvectors are requested, the routine may either return the */
/*  matrices X and/or Y of right or left eigenvectors of T, or the */
/*  products Q*X and/or Q*Y, where Q is an input orthogonal */
/*  matrix. If T was obtained from the real-Schur factorization of an */
/*  original matrix A = Q*T*Q', then Q*X and Q*Y are the matrices of */
/*  right or left eigenvectors of A. */

/*  T must be in Schur canonical form (as returned by DHSEQR), that is, */
/*  block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each */
/*  2-by-2 diagonal block has its diagonal elements equal and its */
/*  off-diagonal elements of opposite sign.  Corresponding to each 2-by-2 */
/*  diagonal block is a complex conjugate pair of eigenvalues and */
/*  eigenvectors; only one eigenvector of the pair is computed, namely */
/*  the one corresponding to the eigenvalue with positive imaginary part. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'R':  compute right eigenvectors only; */
/*          = 'L':  compute left eigenvectors only; */
/*          = 'B':  compute both right and left eigenvectors. */

/*  HOWMNY  (input) CHARACTER*1 */
/*          = 'A':  compute all right and/or left eigenvectors; */
/*          = 'B':  compute all right and/or left eigenvectors, */
/*                  and backtransform them using the input matrices */
/*                  supplied in VR and/or VL; */
/*          = 'S':  compute selected right and/or left eigenvectors, */
/*                  specified by the logical array SELECT. */

/*  SELECT  (input/output) LOGICAL array, dimension (N) */
/*          If HOWMNY = 'S', SELECT specifies the eigenvectors to be */
/*          computed. */
/*          If HOWMNY = 'A' or 'B', SELECT is not referenced. */
/*          To select the real eigenvector corresponding to a real */
/*          eigenvalue w(j), SELECT(j) must be set to .TRUE..  To select */
/*          the complex eigenvector corresponding to a complex conjugate */
/*          pair w(j) and w(j+1), either SELECT(j) or SELECT(j+1) must be */
/*          set to .TRUE.; then on exit SELECT(j) is .TRUE. and */
/*          SELECT(j+1) is .FALSE.. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,N) */
/*          The upper quasi-triangular matrix T in Schur canonical form. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  VL      (input/output) DOUBLE PRECISION array, dimension (LDVL,MM) */
/*          On entry, if SIDE = 'L' or 'B' and HOWMNY = 'B', VL must */
/*          contain an N-by-N matrix Q (usually the orthogonal matrix Q */
/*          of Schur vectors returned by DHSEQR). */
/*          On exit, if SIDE = 'L' or 'B', VL contains: */
/*          if HOWMNY = 'A', the matrix Y of left eigenvectors of T; */
/*                           VL has the same quasi-lower triangular form */
/*                           as T'. If T(i,i) is a real eigenvalue, then */
/*                           the i-th column VL(i) of VL  is its */
/*                           corresponding eigenvector. If T(i:i+1,i:i+1) */
/*                           is a 2-by-2 block whose eigenvalues are */
/*                           complex-conjugate eigenvalues of T, then */
/*                           VL(i)+sqrt(-1)*VL(i+1) is the complex */
/*                           eigenvector corresponding to the eigenvalue */
/*                           with positive real part. */
/*          if HOWMNY = 'B', the matrix Q*Y; */
/*          if HOWMNY = 'S', the left eigenvectors of T specified by */
/*                           SELECT, stored consecutively in the columns */
/*                           of VL, in the same order as their */
/*                           eigenvalues. */
/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part, and the second the imaginary part. */
/*          If SIDE = 'R', VL is not referenced. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL.  LDVL >= max(1,N) if */
/*          SIDE = 'L' or 'B'; LDVL >= 1 otherwise. */

/*  VR      (input/output) DOUBLE PRECISION array, dimension (LDVR,MM) */
/*          On entry, if SIDE = 'R' or 'B' and HOWMNY = 'B', VR must */
/*          contain an N-by-N matrix Q (usually the orthogonal matrix Q */
/*          of Schur vectors returned by DHSEQR). */
/*          On exit, if SIDE = 'R' or 'B', VR contains: */
/*          if HOWMNY = 'A', the matrix X of right eigenvectors of T; */
/*                           VR has the same quasi-upper triangular form */
/*                           as T. If T(i,i) is a real eigenvalue, then */
/*                           the i-th column VR(i) of VR  is its */
/*                           corresponding eigenvector. If T(i:i+1,i:i+1) */
/*                           is a 2-by-2 block whose eigenvalues are */
/*                           complex-conjugate eigenvalues of T, then */
/*                           VR(i)+sqrt(-1)*VR(i+1) is the complex */
/*                           eigenvector corresponding to the eigenvalue */
/*                           with positive real part. */
/*          if HOWMNY = 'B', the matrix Q*X; */
/*          if HOWMNY = 'S', the right eigenvectors of T specified by */
/*                           SELECT, stored consecutively in the columns */
/*                           of VR, in the same order as their */
/*                           eigenvalues. */
/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part and the second the imaginary part. */
/*          If SIDE = 'L', VR is not referenced. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= max(1,N) if */
/*          SIDE = 'R' or 'B'; LDVR >= 1 otherwise. */

/*  MM      (input) INTEGER */
/*          The number of columns in the arrays VL and/or VR. MM >= M. */

/*  M       (output) INTEGER */
/*          The number of columns in the arrays VL and/or VR actually */
/*          used to store the eigenvectors. */
/*          If HOWMNY = 'A' or 'B', M is set to N. */
/*          Each selected real eigenvector occupies one column and each */
/*          selected complex eigenvector occupies two columns. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The algorithm used in this program is basically backward (forward) */
/*  substitution, with scaling to make the the code robust against */
/*  possible overflow. */

/*  Each eigenvector is normalized so that the element of largest */
/*  magnitude has magnitude 1; here the magnitude of a complex number */
/*  (x,y) is taken to be |x| + |y|. */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    bothv = lsame_(side, "B", (ftnlen)1, (ftnlen)1);
    rightv = lsame_(side, "R", (ftnlen)1, (ftnlen)1) || bothv;
    leftv = lsame_(side, "L", (ftnlen)1, (ftnlen)1) || bothv;

    allv = lsame_(howmny, "A", (ftnlen)1, (ftnlen)1);
    over = lsame_(howmny, "B", (ftnlen)1, (ftnlen)1);
    somev = lsame_(howmny, "S", (ftnlen)1, (ftnlen)1);

    *info = 0;
    if (! rightv && ! leftv) {
	*info = -1;
    } else if (! allv && ! over && ! somev) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldt < max(1,*n)) {
	*info = -6;
    } else if (*ldvl < 1 || leftv && *ldvl < *n) {
	*info = -8;
    } else if (*ldvr < 1 || rightv && *ldvr < *n) {
	*info = -10;
    } else {

/*        Set M to the number of columns required to store the selected */
/*        eigenvectors, standardize the array SELECT if necessary, and */
/*        test MM. */

	if (somev) {
	    *m = 0;
	    pair = FALSE_;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (pair) {
		    pair = FALSE_;
		    select[j] = FALSE_;
		} else {
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] == 0.) {
			    if (select[j]) {
				++(*m);
			    }
			} else {
			    pair = TRUE_;
			    if (select[j] || select[j + 1]) {
				select[j] = TRUE_;
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*mm < *m) {
	    *info = -11;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTREVC", &i__1, (ftnlen)6);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }

/*     Set the constants to control overflow. */

    unfl = dlamch_("Safe minimum", (ftnlen)12);
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = dlamch_("Precision", (ftnlen)9);
    smlnum = unfl * (*n / ulp);
    bignum = (1. - ulp) / smlnum;

/*     Compute 1-norm of each column of strictly upper triangular */
/*     part of T to control overflow in triangular solver. */

    work[1] = 0.;
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	work[j] = 0.;
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[j] += (d__1 = t[i__ + j * t_dim1], abs(d__1));
/* L20: */
	}
/* L30: */
    }

/*     Index IP is used to specify the real or complex eigenvalue: */
/*       IP = 0, real eigenvalue, */
/*            1, first of conjugate complex pair: (wr,wi) */
/*           -1, second of conjugate complex pair: (wr,wi) */

    n2 = *n << 1;

    if (rightv) {

/*        Compute right eigenvectors. */

	ip = 0;
	is = *m;
	for (ki = *n; ki >= 1; --ki) {

	    if (ip == 1) {
		goto L130;
	    }
	    if (ki == 1) {
		goto L40;
	    }
	    if (t[ki + (ki - 1) * t_dim1] == 0.) {
		goto L40;
	    }
	    ip = -1;

L40:
	    if (somev) {
		if (ip == 0) {
		    if (! select[ki]) {
			goto L130;
		    }
		} else {
		    if (! select[ki - 1]) {
			goto L130;
		    }
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki - 1) * t_dim1], abs(d__1))) * 
			sqrt((d__2 = t[ki - 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = max(d__1,smlnum);

	    if (ip == 0) {

/*              Real right eigenvector */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__1 = ki - 1;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -t[k + ki * t_dim1];
/* L50: */
		}

/*              Solve the upper quasi-triangular system: */
/*                 (T(1:KI-1,1:KI-1) - WR)*X = SCALE*WORK. */

		jnxt = ki - 1;
		for (j = ki - 1; j >= 1; --j) {
		    if (j > jnxt) {
			goto L60;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, 
				&ierr);

/*                    Scale X(1,1) to avoid overflow when updating */
/*                    the right-hand side. */

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j + *n] = x[0];

/*                    Update right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b22, &t[j - 
				1 + (j - 1) * t_dim1], ldt, &c_b22, &c_b22, &
				work[j - 1 + *n], n, &wr, &c_b25, x, &c__2, &
				scale, &xnorm, &ierr);

/*                    Scale X(1,1) and X(2,1) to avoid overflow when */
/*                    updating the right-hand side. */

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				x[0] /= xnorm;
				x[1] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];

/*                    Update right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1, 
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
		    }
L60:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    ii = idamax_(&ki, &vr[is * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + is * vr_dim1], abs(d__1));
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + is * vr_dim1] = 0.;
/* L70: */
		    }
		} else {
		    if (ki > 1) {
			i__1 = ki - 1;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki + *n], &vr[ki * 
				vr_dim1 + 1], &c__1, (ftnlen)1);
		    }

		    ii = idamax_(n, &vr[ki * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + ki * vr_dim1], abs(d__1));
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}

	    } else {

/*              Complex right eigenvector. */

/*              Initial solve */
/*                [ (T(KI-1,KI-1) T(KI-1,KI) ) - (WR + I* WI)]*X = 0. */
/*                [ (T(KI,KI-1)   T(KI,KI)   )               ] */

		if ((d__1 = t[ki - 1 + ki * t_dim1], abs(d__1)) >= (d__2 = t[
			ki + (ki - 1) * t_dim1], abs(d__2))) {
		    work[ki - 1 + *n] = 1.;
		    work[ki + n2] = wi / t[ki - 1 + ki * t_dim1];
		} else {
		    work[ki - 1 + *n] = -wi / t[ki + (ki - 1) * t_dim1];
		    work[ki + n2] = 1.;
		}
		work[ki + *n] = 0.;
		work[ki - 1 + n2] = 0.;

/*              Form right-hand side */

		i__1 = ki - 2;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -work[ki - 1 + *n] * t[k + (ki - 1) * 
			    t_dim1];
		    work[k + n2] = -work[ki + n2] * t[k + ki * t_dim1];
/* L80: */
		}

/*              Solve upper quasi-triangular system: */
/*              (T(1:KI-2,1:KI-2) - (WR+i*WI))*X = SCALE*(WORK+i*WORK2) */

		jnxt = ki - 2;
		for (j = ki - 2; j >= 1; --j) {
		    if (j > jnxt) {
			goto L90;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &wi, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale X(1,1) and X(1,2) to avoid overflow when */
/*                    updating the right-hand side. */

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				x[2] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];

/*                    Update the right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 1;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__2, &smin, &c_b22, &t[j - 
				1 + (j - 1) * t_dim1], ldt, &c_b22, &c_b22, &
				work[j - 1 + *n], n, &wr, &wi, x, &c__2, &
				scale, &xnorm, &ierr);

/*                    Scale X to avoid overflow when updating */
/*                    the right-hand side. */

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				rec = 1. / xnorm;
				x[0] *= rec;
				x[2] *= rec;
				x[1] *= rec;
				x[3] *= rec;
				scale *= rec;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];
			work[j - 1 + n2] = x[2];
			work[j + n2] = x[3];

/*                    Update the right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1, 
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1, 
				&work[n2 + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[3];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);
		    }
L90:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[(is - 1) * vr_dim1 
			    + 1], &c__1);
		    dcopy_(&ki, &work[n2 + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    emax = 0.;
		    i__1 = ki;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (is - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + is * vr_dim1], 
				abs(d__2));
			emax = max(d__3,d__4);
/* L100: */
		    }

		    remax = 1. / emax;
		    dscal_(&ki, &remax, &vr[(is - 1) * vr_dim1 + 1], &c__1);
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + (is - 1) * vr_dim1] = 0.;
			vr[k + is * vr_dim1] = 0.;
/* L110: */
		    }

		} else {

		    if (ki > 2) {
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki - 1 + *n], &vr[(
				ki - 1) * vr_dim1 + 1], &c__1, (ftnlen)1);
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[n2 + 1], &c__1, &work[ki + n2], &vr[ki * 
				vr_dim1 + 1], &c__1, (ftnlen)1);
		    } else {
			dscal_(n, &work[ki - 1 + *n], &vr[(ki - 1) * vr_dim1 
				+ 1], &c__1);
			dscal_(n, &work[ki + n2], &vr[ki * vr_dim1 + 1], &
				c__1);
		    }

		    emax = 0.;
		    i__1 = *n;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (ki - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + ki * vr_dim1], 
				abs(d__2));
			emax = max(d__3,d__4);
/* L120: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vr[(ki - 1) * vr_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}
	    }

	    --is;
	    if (ip != 0) {
		--is;
	    }
L130:
	    if (ip == 1) {
		ip = 0;
	    }
	    if (ip == -1) {
		ip = 1;
	    }
/* L140: */
	}
    }

    if (leftv) {

/*        Compute left eigenvectors. */

	ip = 0;
	is = 1;
	i__1 = *n;
	for (ki = 1; ki <= i__1; ++ki) {

	    if (ip == -1) {
		goto L250;
	    }
	    if (ki == *n) {
		goto L150;
	    }
	    if (t[ki + 1 + ki * t_dim1] == 0.) {
		goto L150;
	    }
	    ip = 1;

L150:
	    if (somev) {
		if (! select[ki]) {
		    goto L250;
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1))) * 
			sqrt((d__2 = t[ki + 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = max(d__1,smlnum);

	    if (ip == 0) {

/*              Real left eigenvector. */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 1; k <= i__2; ++k) {
		    work[k + *n] = -t[ki + k * t_dim1];
/* L160: */
		}

/*              Solve the quasi-triangular system: */
/*                 (T(KI+1:N,KI+1:N) - WR)'*X = SCALE*WORK */

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 1;
		i__2 = *n;
		for (j = ki + 1; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L170;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side. */

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1], 
				&c__1, &work[ki + 1 + *n], &c__1);

/*                    Solve (T(J,J)-WR)'*X = WORK */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, 
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
/* Computing MAX */
			d__2 = (d__1 = work[j + *n], abs(d__1));
			vmax = max(d__2,vmax);
			vcrit = bignum / vmax;

		    } else {

/*                    2-by-2 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side. */

/* Computing MAX */
			d__1 = work[j], d__2 = work[j + 1];
			beta = max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1], 
				&c__1, &work[ki + 1 + *n], &c__1);

			i__3 = j - ki - 1;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 1 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 1 + *n], &c__1);

/*                    Solve */
/*                      [T(J,J)-WR   T(J,J+1)     ]'* X = SCALE*( WORK1 ) */
/*                      [T(J+1,J)    T(J+1,J+1)-WR]             ( WORK2 ) */

			dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm, 
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
			work[j + 1 + *n] = x[1];

/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2 
				= work[j + 1 + *n], abs(d__2)), d__3 = max(
				d__3,d__4);
			vmax = max(d__3,vmax);
			vcrit = bignum / vmax;

		    }
L170:
		    ;
		}

/*              Copy the vector x or Q*x to VL and normalize. */

		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is * 
			    vl_dim1], &c__1);

		    i__2 = *n - ki + 1;
		    ii = idamax_(&i__2, &vl[ki + is * vl_dim1], &c__1) + ki - 
			    1;
		    remax = 1. / (d__1 = vl[ii + is * vl_dim1], abs(d__1));
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
/* L180: */
		    }

		} else {

		    if (ki < *n) {
			i__2 = *n - ki;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 1) * vl_dim1 
				+ 1], ldvl, &work[ki + 1 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1, (
				ftnlen)1);
		    }

		    ii = idamax_(n, &vl[ki * vl_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vl[ii + ki * vl_dim1], abs(d__1));
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);

		}

	    } else {

/*              Complex left eigenvector. */

/*               Initial solve: */
/*                 ((T(KI,KI)    T(KI,KI+1) )' - (WR - I* WI))*X = 0. */
/*                 ((T(KI+1,KI) T(KI+1,KI+1))                ) */

		if ((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1)) >= (d__2 = 
			t[ki + 1 + ki * t_dim1], abs(d__2))) {
		    work[ki + *n] = wi / t[ki + (ki + 1) * t_dim1];
		    work[ki + 1 + n2] = 1.;
		} else {
		    work[ki + *n] = 1.;
		    work[ki + 1 + n2] = -wi / t[ki + 1 + ki * t_dim1];
		}
		work[ki + 1 + *n] = 0.;
		work[ki + n2] = 0.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 2; k <= i__2; ++k) {
		    work[k + *n] = -work[ki + *n] * t[ki + k * t_dim1];
		    work[k + n2] = -work[ki + 1 + n2] * t[ki + 1 + k * t_dim1]
			    ;
/* L190: */
		}

/*              Solve complex quasi-triangular system: */
/*              ( T(KI+2,N:KI+2,N) - (WR-i*WI) )*X = WORK1+i*WORK2 */

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 2;
		i__2 = *n;
		for (j = ki + 2; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L200;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

/*                    Scale if necessary to avoid overflow when */
/*                    forming the right-hand side elements. */

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1], 
				&c__1, &work[ki + 2 + *n], &c__1);
			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1], 
				&c__1, &work[ki + 2 + n2], &c__1);

/*                    Solve (T(J,J)-(WR-i*WI))*(X11+i*X12)= WK+I*WK2 */

			d__1 = -wi;
			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2 
				= work[j + n2], abs(d__2)), d__3 = max(d__3,
				d__4);
			vmax = max(d__3,vmax);
			vcrit = bignum / vmax;

		    } else {

/*                    2-by-2 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side elements. */

/* Computing MAX */
			d__1 = work[j], d__2 = work[j + 1];
			beta = max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1], 
				&c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1], 
				&c__1, &work[ki + 2 + n2], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + n2] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + n2], &c__1);

/*                    Solve 2-by-2 complex linear equation */
/*                      ([T(j,j)   T(j,j+1)  ]'-(wr-i*wi)*I)*X = SCALE*B */
/*                      ([T(j+1,j) T(j+1,j+1)]             ) */

			d__1 = -wi;
			dlaln2_(&c_true, &c__2, &c__2, &smin, &c_b22, &t[j + 
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
			work[j + 1 + *n] = x[1];
			work[j + 1 + n2] = x[3];
/* Computing MAX */
			d__1 = abs(x[0]), d__2 = abs(x[2]), d__1 = max(d__1,
				d__2), d__2 = abs(x[1]), d__1 = max(d__1,d__2)
				, d__2 = abs(x[3]), d__1 = max(d__1,d__2);
			vmax = max(d__1,vmax);
			vcrit = bignum / vmax;

		    }
L200:
		    ;
		}

/*              Copy the vector x or Q*x to VL and normalize. */

/* L210: */
		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is * 
			    vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + n2], &c__1, &vl[ki + (is + 1) * 
			    vl_dim1], &c__1);

		    emax = 0.;
		    i__2 = *n;
		    for (k = ki; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + is * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (is + 1) * vl_dim1], 
				abs(d__2));
			emax = max(d__3,d__4);
/* L220: */
		    }
		    remax = 1. / emax;
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + (is + 1) * vl_dim1], &c__1)
			    ;

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
			vl[k + (is + 1) * vl_dim1] = 0.;
/* L230: */
		    }
		} else {
		    if (ki < *n - 1) {
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 2) * vl_dim1 
				+ 1], ldvl, &work[ki + 2 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1, (
				ftnlen)1);
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 2) * vl_dim1 
				+ 1], ldvl, &work[ki + 2 + n2], &c__1, &work[
				ki + 1 + n2], &vl[(ki + 1) * vl_dim1 + 1], &
				c__1, (ftnlen)1);
		    } else {
			dscal_(n, &work[ki + *n], &vl[ki * vl_dim1 + 1], &
				c__1);
			dscal_(n, &work[ki + 1 + n2], &vl[(ki + 1) * vl_dim1 
				+ 1], &c__1);
		    }

		    emax = 0.;
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + ki * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (ki + 1) * vl_dim1], 
				abs(d__2));
			emax = max(d__3,d__4);
/* L240: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vl[(ki + 1) * vl_dim1 + 1], &c__1);

		}

	    }

	    ++is;
	    if (ip != 0) {
		++is;
	    }
L250:
	    if (ip == -1) {
		ip = 0;
	    }
	    if (ip == 1) {
		ip = -1;
	    }

/* L260: */
	}

    }

    return 0;

/*     End of DTREVC */

} /* dtrevc_ */

