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

#include "Array.hpp"
#include "LAPACK.hpp"
#include "IEEEFP.hpp"
#include <math.h>
#include <float.h>

//!
//@Module INF Infinity Constant
//@@Section CONSTANTS
//@@Usage
//Returns a value that represents positive infinity 
//for both 32 and 64-bit floating point values.
//@[
//   y = inf
//@]
//The returned type is a 64-bit float, but demotion to
//64 bits preserves the infinity.
//@@Function Internals
//The infinity constant has
//several interesting properties.  In particular:
//\[
//\begin{array}{ll}
//   \infty \times 0 & = \mathrm{NaN} \\                                             \infty \times a & = \infty \, \mathrm{for all} \, a > 0 \\   \infty \times a & = -\infty \, \mathrm{for all} \, a < 0 \\   \infty / \infty & = \mathrm{NaN} \\   \infty / 0 & = \infty 
//\end{array}
//\]
//Note that infinities are not preserved under type conversion to integer types (see the examples below).
//@@Example
//The following examples demonstrate the various properties of the infinity constant.
//@<
//inf*0
//inf*2
//inf*-2
//inf/inf
//inf/0
//inf/nan
//@>
//Note that infinities are preserved under type conversion to floating point types (i.e., @|float|, @|double|, @|complex| and @|dcomplex| types), but not integer  types.
//@<
//uint32(inf)
//complex(inf)
//@>
//@@Signature
//function inf InfFunction
//inputs none
//outputs y
//!
ArrayVector InfFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(Inf()));
}

//!
//@Module NAN Not-a-Number Constant
//@@Section CONSTANTS
//@@Usage
//Returns a value that represents ``not-a-number'' for both 32 and 64-bit 
//floating point values.  This constant is meant to represent the result of
//arithmetic operations whose output cannot be meaningfully defined (like 
//zero divided by zero).
//@[
//   y = nan
//@]
//The returned type is a 64-bit float, but demotion to 32 bits preserves the not-a-number.  The not-a-number constant has one simple property.  In particular, any arithmetic operation with a @|NaN| results in a @|NaN|. These calculations run significantly slower than calculations involving finite quantities!  Make sure that you use @|NaN|s in extreme circumstances only.  Note that @|NaN| is not preserved under type conversion to integer types (see the examples below).
//@@Example
//The following examples demonstrate a few calculations with the not-a-number constant.
//@<
//nan*0
//nan-nan
//@>
//Note that @|NaN|s are preserved under type conversion to floating point types (i.e., @|float|, @|double|, @|complex| and @|dcomplex| types), but not integer  types.
//@<
//uint32(nan)
//complex(nan)
//@>
//@@Signature
//function nan NaNFunction
//inputs none
//outputs y
//@@Signature
//function NaN NaNFunction
//inputs none
//outputs y
//!
ArrayVector NaNFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(NaN()));
}
  
//!
//@Module I-J Square Root of Negative One
//@@Section CONSTANTS
//@@Usage
//Returns a @|complex| value that represents the square root of -1.  There are two
//functions that return the same value:
//@[
//   y = i
//@]
//and 
//@[
//   y = j.
//@]
//This allows either @|i| or @|j| to be used as loop indices.  The returned value is a 32-bit complex value.
//@@Example
//The following examples demonstrate a few calculations with @|i|.
//@<
//i
//i^2
//@>
//The same calculations with @|j|:
//@<
//j
//j^2
//@>
//Here is an example of how @|i| can be used as a loop index and then recovered as the square root of -1.
//@<
//accum = 0; for i=1:100; accum = accum + i; end; accum
//i
//clear i
//i
//@>
//@@Signature
//function i IFunction
//inputs none
//outputs y
//@@Signature
//function j IFunction
//inputs none
//outputs y
//!
ArrayVector IFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(0),double(1)));
}

//!
//@Module PI Constant Pi
//@@Section CONSTANTS
//@@Usage
//Returns a @|double| (64-bit floating point number) value that represents pi (ratio between the circumference and diameter of a circle...).  Typical usage 
//@[
//   y = pi
//@]
//This value is approximately 3.141592653589793.
//@@Example
//The following example demonstrates the use of the @|pi| function.
//@<
//pi
//cos(pi)
//@>
//@@Signature
//function pi PiFunction
//inputs none
//outputs y
//!
ArrayVector PiFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(4.0*atan(1.0))));
}  

//!
//@Module E Euler Constant (Base of Natural Logarithm)
//@@Section CONSTANTS
//@@Usage
//Returns a @|double| (64-bit floating point number) value that represents Euler's constant, the base of the natural logarithm.  Typical usage 
//@[
//   y = e
//@]
//This value is approximately 2.718281828459045.
//@@Example
//The following example demonstrates the use of the @|e| function.
//@<
//e
//log(e)
//@>
//@@Signature
//function e EFunction
//inputs none
//outputs y
//!
ArrayVector EFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(exp(1.0)));
}  

//!
//@Module EPS Double Precision Floating Point Relative Machine Precision Epsilon
//@@Section CONSTANTS
//@@Usage
//Returns @|eps|, which quantifies the relative machine precision
//of floating point numbers (a machine specific quantity).  The syntax
//for @|eps| is:
//@[
//   y = eps
//   y = eps('double')
//   y = eps(X)
//@]
//First form returns @|eps| for @|double| precision values. For most
//typical processors, this value is approximately @|2^-52|, or 2.2204e-16.
//Second form return @|eps| for class @|double| or @|single|.
//Third form returns distance to the next value greater than X.
//@@Example
//The following example demonstrates the use of the @|eps| function,
//and one of its numerical consequences.
//@<
//eps
//1.0+eps
//eps(1000.)
//@>
//@{ test_eps1.m
//b = eps(1.);
//c = eps('single');
//d = eps(single(1.));
//test_val = test(a==b && c==d);
//@}
//@@Signature
//function eps EpsFunction
//inputs varargin
//outputs y
//!
ArrayVector EpsFunction(int nargout, const ArrayVector& arg) {
    bool isDouble = true;
    ArrayVector retval;
    
    if( arg.size()> 1 )
	throw Exception("eps takes no more than 1 argument");
    if( arg.size()==1 ){
	Array a( arg[0] );
	if( a.isString() ){
	    QString str = a.asString();
	    if( str == QString( "double" ) ){
		retval << Array( feps( 1. ) ); 
	    }
	    else if( str == QString( "single" ) ){
		retval << Array( fepsf( 1. ) );
	    }
	    else{
		throw Exception("Class must be 'double' or 'single'");
	    }
	}
	else { //numeric argument
	    switch( a.dataClass() ){ 
		case Double: 
		    {
			double x = fabs( a.asDouble() );
			retval << Array( feps( x ) );
			break;
		    }
		case Float: 
		    {
			float x = fabsf( a.asDouble() );
			retval << Array( fepsf( x ) );
			break;
		    }
		default:
		    throw Exception("Class must be 'double' or 'single'");
	    }
	}
    }
    else{
	retval << Array( feps( 1. ) );
    }
  return retval;
}

//!
//@Module FEPS Single Precision Floating Point Relative Machine Precision Epsilon
//@@Section CONSTANTS
//@@Usage
//Returns @|feps|, which quantifies the relative machine precision
//of floating point numbers (a machine specific quantity).  The syntax
//for @|feps| is:
//@[
//   y = feps
//@]
//which returns @|feps| for @|single| precision values. For most
//typical processors, this value is approximately @|2^-24|, or 5.9604e-8.
//@@Example
//The following example demonstrates the use of the @|feps| function,
//and one of its numerical consequences.
//@<
//feps
//1.0f+eps
//@>
//@@Signature
//function feps FepsFunction
//inputs none
//outputs y
//!
ArrayVector FepsFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(float(nextafterf(1.0,2.0)-1.0f)));
}

//!
//@Module TRUE Logical TRUE
//@@Section CONSTANTS
//@@Usage
//Returns a logical 1.  The syntax for its use is
//@[
//   y = true
//@]
//@@Tests
//@$exact|y1=logical(x1)
//@@Signature
//function true TrueFunction
//inputs none
//outputs y
//!
ArrayVector TrueFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(bool(true)));
}

//!
//@Module FALSE Logical False
//@@Section CONSTANTS
//@@Usage
//Returns a logical 0.  The syntax for its use is
//@[
//   y = false
//@]
//@@Signature
//function false FalseFunction
//inputs none
//outputs y
//!
ArrayVector FalseFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(bool(false)));
}
