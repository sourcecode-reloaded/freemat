      DOUBLE PRECISION FUNCTION DSECND( )
*
*  -- LAPACK auxiliary routine (version 2.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     September 30, 1994
*
*  Purpose
*  =======
*
*  DSECND returns the user time for a process in seconds.
*  This version gets the time from the system function CLOCK().
*
* =====================================================================
*
      USE DFPORT
*
*     ..
*     .. Executable Statements ..
*
      DSECND = DBLE( SECNDS( 0.0E0 ) )
      RETURN
*
*     End of DSECND
*
      END
