/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "Utils.hpp"
#include "IEEEFP.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include <math.h>
#include "Types.hpp"
#include <algorithm>
#include "SparseMatrix.hpp"
#include "Math.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include <QtCore>


//!
//@Module PCODE Convert a Script or Function to P-Code
//@@Section FREEMAT
//@@Usage
//Writes out a script or function as a P-code function.
//The general syntax for its use is:
//@[
//   pcode fun1 fun2 ...
//@]
//The compiled functions are written to the current
//directory.
//!
ArrayVector PCodeFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  throw Exception("P-code functions currently unsupported");
}




//!
//@Module XNRM2 BLAS Norm Calculation
//@@Section ARRAY
//@@Usage
//Calculates the 2-norm of a vector.  The syntax for its use
//is
//@[
//   y = xnrm2(A)
//@]
//where @|A| is the n-dimensional array to analyze.  This form
//uses the underlying BLAS implementation to compute the 2-norm.
//!
ArrayVector XNrm2Function(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("xnrm2 requires at least one argument");
  Array input(arg[0]);
  Class argType(input.dataClass());
  if (input.isReferenceType())
    throw Exception("xnrm2 does not apply to reference types");
  if ((argType < FM_FLOAT) || (argType == FM_STRING)) {
    input.promoteType(FM_DOUBLE);
    argType = input.dataClass();
  }
  switch (argType) {
  default: throw Exception("Unsupported type for xnrm2 operation");
  case FM_FLOAT: {
    float *ptr = (float*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::floatConstructor(snrm2_(&len,ptr,&one)));
  }
  case FM_DOUBLE:  {
    double *ptr = (double*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::doubleConstructor(dnrm2_(&len,ptr,&one)));
  }
  case FM_COMPLEX:  {
    float *ptr = (float*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::floatConstructor(scnrm2_(&len,ptr,&one)));
  }
  case FM_DCOMPLEX: {
    double *ptr = (double*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::doubleConstructor(dznrm2_(&len,ptr,&one)));
  }
  }
  return ArrayVector();
}

