/* ---------------------------------------------------------------------
 *
 * -- Automatically Tuned Linear Algebra Software (ATLAS)
 *    (C) Copyright 2000 All Rights Reserved
 *
 * -- ATLAS routine -- Version 3.2 -- December 25, 2000
 *
 * -- Suggestions,  comments,  bugs reports should be sent to the follo-
 *    wing e-mail address: atlas@cs.utk.edu
 *
 * Author         : Antoine P. Petitet
 * Contributor(s) : R. Clint Whaley
 * University of Tennessee - Innovative Computing Laboratory
 * Knoxville TN, 37996-1301, USA.
 *
 * ---------------------------------------------------------------------
 *
 * -- Copyright notice and Licensing terms:
 *
 *  Redistribution  and  use in  source and binary forms, with or without
 *  modification, are  permitted provided  that the following  conditions
 *  are met:
 *
 * 1. Redistributions  of  source  code  must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce  the above copyright
 *    notice,  this list of conditions, and the  following disclaimer in
 *    the documentation and/or other materials provided with the distri-
 *    bution.
 * 3. The name of the University,  the ATLAS group,  or the names of its
 *    contributors  may not be used to endorse or promote products deri-
 *    ved from this software without specific written permission.
 *
 * -- Disclaimer:
 *
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
 * RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
 * CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------
 */
/*
 * Include files
 */
#include "atlas_misc.h"
#include "atlas_mv.h"
#include "atlas_r1.h"
#include "atlas_level1.h"
#include "atlas_kernel2.h"
#include "atlas_reflvl2.h"
#include "atlas_lvl2.h"

void Mjoin( PATL, trmvUC )
(
   const enum ATLAS_DIAG      DIAG,
   const int                  N,         /* N > 0 assumed */
   const TYPE                 * A,
   const int                  LDA,
   TYPE                       * X
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, trmvUC ) performs the following matrix-vector operation
 *
 *    x := conjg( A  ) * x,
 *
 * where x is an n-element vector and  A is an n by n unit or  non-unit,
 * upper triangular matrix.
 *
 * This is a blocked version of the algorithm.  For a more detailed des-
 * cription of  the arguments of this function, see the reference imple-
 * mentation in the  ATLAS/src/blas/reference directory.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
#ifdef TREAL
#define    one                ATL_rone
#define    lda2               LDA
#else
   TYPE                       one[2] = { ATL_rone, ATL_rzero };
   const int                  lda2   = ( LDA SHIFT );
#endif
   TYPE                       * A0, * x0;
#ifdef ATL_AXPYMV
   int                        incA,        incX, mb, mb1, n, nb;
#else
   int                        incA0, incA, incX, m, mb, nb, nb1;
#endif
   void                       (*trmv0)( const int, const TYPE *, const int,
                              TYPE * );
#define    gemv0              Mjoin( PATL, gemvNc_a1_x1_b1_y1  )
/* ..
 * .. Executable Statements ..
 *
 */
   ATL_GetPartMVN( A, LDA, &mb, &nb );

   if( DIAG == AtlasNonUnit ) trmv0 = Mjoin( PATL, trmvUCN );
   else                       trmv0 = Mjoin( PATL, trmvUCU );

#ifdef ATL_AXPYMV
   mb1  = N - ( ( N - 1 ) / mb ) * mb;
   incA = mb * lda2 + ( incX = (mb SHIFT) );

   for( n  = N - mb, A0 = (TYPE *)(A) + mb * lda2, x0 = X + incX; n > 0;
        n -= mb, A0 += incA, A += incA, X += incX, x0 += incX )
   {
      trmv0( mb, A, LDA, X );
      gemv0( mb, n, one, A0, LDA, x0, 1, one, X, 1 );
   }
   trmv0( mb1, A, LDA, X );
#else
   nb1  = N - ( ( N - 1 ) / nb ) * nb;
   incA = ( incA0 = nb * lda2 ) + ( incX = (nb SHIFT) );  x0 = X;

   trmv0( nb1, A, LDA, X );
   X += (nb1 SHIFT); A0 = (TYPE *)(A += nb1 * lda2); A += (nb1 SHIFT);
   for( m = nb1; m < N; m += nb, A0 += incA0, A += incA, X += incX )
   {
      gemv0( m, nb, one, A0, LDA, X, 1, one, x0, 1 );
      trmv0( nb, A, LDA, X );
   }
#endif
/*
 * End of Mjoin( PATL, trmvUC )
 */
}
