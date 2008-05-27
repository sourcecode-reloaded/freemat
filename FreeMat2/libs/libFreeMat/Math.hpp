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

#ifndef __Math_hpp_
#define __Math_hpp_

#include "Array.hpp"
#include <cmath>

/**
 * Add the two argument arrays together: $$C_n = A_n + B_n$$.
 */
Array Add(const Array& A, const Array& B);
/**
 * Subtract the second array from the first: $$C_n = A_n - B_n$$.
 */
Array Subtract(const Array& A, const Array& B);
/**
 * Element-wise multiply of two arrays: $$C_n = A_n B_n$$.
 */
Array DotMultiply(const Array& A, const Array& B);
/**
 * Element-wise divide of two arrays: $$C_n = \frac{A_n}{B_n}$$.
 */
Array DotRightDivide(const Array& A, const Array& B);
/**
 * Element-wise divide of two arrays: $$C_n = \frac{B_n}{A_n}$$.
 */
Array DotLeftDivide(const Array& A, const Array& B);
/**
 * Element-wise compare (lt) of two arrays: $$C_n = A_n < B_n$$.
 */ 
Array LessThan(const Array& A, const Array& B);
/**
 * Element-wise compare (le) of two arrays: $$C_n = A_n \leq B_n$$.
 */   
Array LessEquals(const Array& A, const Array& B);
/**
 * Element-wise compare (gt) of two arrays: $$C_n = A_n > B_n$$.
 */ 
Array GreaterThan(const Array& A, const Array& B);
/**
 * Element-wise compare (ge) of two arrays: $$C_n = A_n \geq B_n$$.
 */ 
Array GreaterEquals(const Array& A, const Array& B);
/**
 * Element-wise compare (eq) of two arrays: $$C_n = A_n == B_n$$.
 */ 
Array Equals(const Array& A, const Array& B);
/**
 * Element-wise compare (ne) of two arrays: $$C_n = A_n \neq B_n$$.
 */ 
Array NotEquals(const Array& A, const Array& B);
/**
 * Element-wise or of two arrays: $$C_n = A_n \or B_n$$.
 */
Array Or(const Array& A, const Array& B);
/**
 * Element-wise and of two arrays: $$C_n = A_n \and B_n$$.
 */
Array And(const Array& A, const Array& B);
/**
 * Element-wise not of argument: $$C_n = \not A_n$$.
 */
Array Not(const Array& A);
/**
 * Element-wise plus of argument: $$C_n = + A_n$$.
 */
Array Plus(const Array& A);
/**
 * Element-wise negate of argument: $$C_n = - A_n$$.
 */
Array Negate(const Array& A);
/**
 * Element-wise power: $$C_n = A_n ^ {B_n}$$.
 */
Array DotPower(const Array& A, const Array& B);
/**
 * Matrix to matrix power.  The calculation performed
 * depends on the sizes of the arguments.
 *   - both are scalars, $$C = A^B$$.
 *   - $$A$$ is a square matrix, $$B$$ is a scalar,
 *     $$C = E V^{B} E^{-1}$$, where $$V$$ is the diagonal
 *     matrix of eigenvalues of $$A$$, and $$E$$ is the
 *     associated matrix of eigenvectors.  
 *   - $$A$$ is a scalar, $$B$$ is a matrix,
 *     $$C = E A^{V} E^{-1}$$, where $$V$$ is the diagonal
 *     matrix of eigenvalues of $$B$$, and $$E$$ is the 
 *     associated matrix of eigenvectors.  The quantity
 *     $$A^{V}$$ is evaluated along the diagonal.
 * Throws an exception if 
 *   - both arguments are matrices
 *   - either of the arguments is more than 2-dimensional
 *   - any of the arguments are rectangular.
 */
Array Power(const Array& A, const Array& B);
/**
 * Matrix multiply of the arguments.  For $$m \times n$$ matrix $$A$$,
 * and $$n \times k$$ matrix $$B$$, the output matrix $$C$$ is of 
 * size $$m \times k$$, defined by  $$C_{i,j} = \sum_{p=1}^{n} A_{i,p} B_{p,j}$$.
 * Throws an exception if the sizes are not conformant.
 */
Array Multiply(const Array& A, const Array& B);
/**
 * The right divide operation is related to the left divide operation
 * via: B/A = (A'\B')'.
 */
Array RightDivide(const Array& A, const Array& B);
/**
 * The left divide operation is equivalent to solving the system of equations
 * $$A C = B$$ for the matrix $$C$$, where $$A$$ is of size $$m \times n$$,
 * $$B$$ is of size $$m \times k$$, and the output $$C$$ is of size $$n \times k$$.
 * Uses the linear equation solver from LAPACK to solve these equations.
 * They are effectively solved independently for each column of $$B$$.
 */
Array LeftDivide(const Array& A, const Array& B);
/**
 * For scalars $$A$$ and $$B$$, the output is the row vector
 * $$[A,A+1,\ldots,A+n]$$, where $$n$$ is the largest integer
 * such that $$A+n < B$$.
 */
Array UnitColon(const Array& A, const Array& B);
/**
 * For scalars $$A$$, $$B$$ and $$C$$, the output is the row vector
 * $$[A,A+B,\ldots,A+nB]$$, where $$n$$ is the largest integer
 * such that $$A+nB < C$$.
 */
Array DoubleColon(const Array& A, const Array& B, const Array& C);

void TypeCheck(Array &A, Array &B, bool isDivOrMatrix);

Array InvertMatrix(const Array& A);

template <class T>
inline T complex_abs(T real, T imag) {
  T swap;
  if(real < 0)
    real = -real;
  if(imag < 0)
    imag = -imag;
  if(imag > real){
    swap = real;
    real = imag;
    imag = swap;
  }
  if((real+imag) == real)
    return(real);
  double temp = double(imag)/double(real);
  temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
  return(T(temp));
}

template <typename T>
inline T complex_phase(const T &ar, const T &ai) {
  return atan2(ai,ar);
}

template <typename T>
inline void complex_multiply(const T &ar, const T &ai, 
			     const T &br, const T &bi, 
			     T &cr, T &ci) {
  cr = ar * br - ai * bi;
  ci = ar * bi + ai * br;  
}

template <typename T>
inline bool complex_lt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if (mag_a < mag_b) return true;
  if (mag_b < mag_a) return false;
  return (complex_phase(ar,ai) < complex_phase(br,bi));
}

template <typename T>
inline bool complex_gt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if (mag_a > mag_b) return true;
  if (mag_b > mag_a) return false;
  return (complex_phase(ar,ai) > complex_phase(br,bi));
}

template <typename T>
inline bool complex_eq(const T & ar, const T & ai, 
		       const T & br, const T & bi) {
  return ((ar == br) && (ai == bi));
}

template <typename T>
inline bool complex_ne(const T & ar, const T & ai,
		       const T & br, const T & bi) {
  return ((ar != br) || (ai != bi));
}

template <typename T>
inline bool complex_le(const T & ar, const T & ai, 
		       const T & br, const T & bi) {
  return complex_eq(ar,ai,br,bi) || complex_lt(ar,ai,br,bi);
}

template <typename T>
inline bool complex_ge(const T & ar, const T & ai, 
		const T & br, const T & bi) {
  return complex_eq(ar,ai,br,bi) || complex_gt(ar,ai,br,bi);
}

#endif
