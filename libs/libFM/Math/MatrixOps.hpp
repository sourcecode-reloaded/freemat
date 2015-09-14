#ifndef __MatrixOps_hpp__
#define __MatrixOps_hpp__

#include "Object.hpp"

namespace FM
{
  Object MatrixMultiply(const Object &a, const Object &b, ThreadContext *ctxt);
  Object MatrixInvert(const Object &a, ThreadContext *ctxt);
  Object MatrixLeftDivide(const Object &a, const Object &b, ThreadContext *ctxt);
  Object MatrixRightDivide(const Object &a, const Object &b, ThreadContext *ctxt);
  Object MatrixPower(const Object &a, const Object &b, ThreadContext *ctxt);

  // Promote the types of a and b so that they can be operated on by a matrix
  // operation.  This is:
  //  a    b   v 
  //
  //  s    s   s  0
  //  s    d   s  2
  //  s    c   c  1
  //  s    z   c  3
  //
  //  d    s   s  2
  //  d    d   d  0
  //  d    c   c  1
  //  d    z   z  4
  //
  //  c    s   c  1
  //  c    d   c  1
  //  c    c   c  0
  //  c    z   c  1
  //
  //  z    s   c  3
  //  z    d   z  4
  //  z    c   c  1
  //  z    z   z  0
  //
  //  Rule 0:
  //  x + x --> x
  //
  //  Rule 1:
  //  c + * --> c
  //  * + c --> c
  //
  //  Rule 2:
  //  s + d --> s
  //  d + s --> s
  //
  //  Rule 3:
  //  s + z --> c
  //  z + s --> c
  //
  //  Rule 4:
  //  d + z --> z
  //  z + d --> z
  void MatrixPromoteTypes(Object &a, Object &b, ThreadContext *ctxt);
}


#endif
