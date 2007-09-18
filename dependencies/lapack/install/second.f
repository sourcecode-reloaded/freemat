      REAL             FUNCTION SECOND( )
*
*  -- LAPACK auxiliary routine (version 2.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     September 30, 1994
*
*  Purpose
*  =======
*
*  SECOND returns the user time for a process in seconds.
*  This version gets the time from the system timer clock().
*
* =====================================================================
*
      USE DFPORT
*     ..
*     .. Executable Statements ..
*
      SECOND = SECNDS( 0.0E0 )
      RETURN
*
*     End of SECOND
*
      END
