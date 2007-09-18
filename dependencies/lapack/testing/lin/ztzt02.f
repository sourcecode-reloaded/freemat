      DOUBLE PRECISION FUNCTION ZTZT02( M, N, AF, LDA, TAU, WORK,
     $                 LWORK )
*
*  -- LAPACK test routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     September 30, 1994
*
*     .. Scalar Arguments ..
      INTEGER            LDA, LWORK, M, N
*     ..
*     .. Array Arguments ..
      COMPLEX*16         AF( LDA, * ), TAU( * ), WORK( LWORK )
*     ..
*
*  Purpose
*  =======
*
*  ZTZT02 returns
*       || I - Q'*Q || / ( M * eps)
*  where the matrix Q is defined by the Householder transformations
*  generated by ZTZRQF.
*
*  Arguments
*  =========
*
*  M       (input) INTEGER
*          The number of rows of the matrix AF.
*
*  N       (input) INTEGER
*          The number of columns of the matrix AF.
*
*  AF      (input) COMPLEX*16 array, dimension (LDA,N)
*          The output of ZTZRQF.
*
*  LDA     (input) INTEGER
*          The leading dimension of the array AF.
*
*  TAU     (input) COMPLEX*16 array, dimension (M)
*          Details of the Householder transformations as returned by
*          ZTZRQF.
*
*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK)
*
*  LWORK   (input) INTEGER
*          length of WORK array. Must be >= N*N+N
*
*  =====================================================================
*
*     .. Parameters ..
      DOUBLE PRECISION   ZERO, ONE
      PARAMETER          ( ZERO = 0.0D0, ONE = 1.0D0 )
*     ..
*     .. Local Scalars ..
      INTEGER            I
*     ..
*     .. Local Arrays ..
      DOUBLE PRECISION   RWORK( 1 )
*     ..
*     .. External Functions ..
      DOUBLE PRECISION   DLAMCH, ZLANGE
      EXTERNAL           DLAMCH, ZLANGE
*     ..
*     .. External Subroutines ..
      EXTERNAL           XERBLA, ZLASET, ZLATZM
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          DBLE, DCMPLX, DCONJG, MAX
*     ..
*     .. Executable Statements ..
*
      ZTZT02 = ZERO
*
      IF( LWORK.LT.N*N+N ) THEN
         CALL XERBLA( 'ZTZT02', 7 )
         RETURN
      END IF
*
*     Quick return if possible
*
      IF( M.LE.0 .OR. N.LE.0 )
     $   RETURN
*
*     Q := I
*
      CALL ZLASET( 'Full', N, N, DCMPLX( ZERO ), DCMPLX( ONE ), WORK,
     $             N )
*
*     Q := P(1) * ... * P(m) * Q
*
      DO 10 I = M, 1, -1
         CALL ZLATZM( 'Left', N-M+1, N, AF( I, M+1 ), LDA, TAU( I ),
     $                WORK( I ), WORK( M+1 ), N, WORK( N*N+1 ) )
   10 CONTINUE
*
*     Q := P(m)' * ... * P(1)' * Q
*
      DO 20 I = 1, M
         CALL ZLATZM( 'Left', N-M+1, N, AF( I, M+1 ), LDA,
     $                DCONJG( TAU( I ) ), WORK( I ), WORK( M+1 ), N,
     $                WORK( N*N+1 ) )
   20 CONTINUE
*
*     Q := Q - I
*
      DO 30 I = 1, N
         WORK( ( I-1 )*N+I ) = WORK( ( I-1 )*N+I ) - ONE
   30 CONTINUE
*
      ZTZT02 = ZLANGE( 'One-norm', N, N, WORK, N, RWORK ) /
     $         ( DLAMCH( 'Epsilon' )*DBLE( MAX( M, N ) ) )
      RETURN
*
*     End of ZTZT02
*
      END
