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
#include <math.h>
#include "Math.hpp"
#include "Operators.hpp"
#include "Complex.hpp"
#include "IEEEFP.hpp"


//!
//@Module LOG1P Natural Logarithm of 1+P Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the @|log| function for one plus its argument.  The general
//syntax for its use is
//@[
//  y = log1p(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Signature
//function log1p Log1PFunction
//inputs x
//output y
//!

struct OpLog1P {
  static inline float func(float x) {return log1pf(x);}
  static inline double func(double x) {return log1p(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = log1pf(complex_abs<float>(xr,xi));
    yi = atan2f(xi,xr);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log1p(complex_abs<double>(xr,xi));
    yi = atan2(xi,xr);
  }
};

ArrayVector Log1PFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log1p function takes exactly one argument");
  Array input(arg[0]);
  if (!IsPositive(input)) input.forceComplex();
  return ArrayVector(UnaryOp<OpLog1P>(input));
}

//!
//@Module LOG Natural Logarithm Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the @|log| function for its argument.  The general
//syntax for its use is
//@[
//  y = log(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//Integer types are promoted to the @|double| type prior to
//calculation of the @|log| function.  Output @|y| is of the
//same size as the input @|x|. For strictly positive, real inputs, 
//the output type is the same as the input.
//For negative and complex arguments, the output is complex.
//@@Function Internals
//Mathematically, the @|log| function is defined for all real
//valued arguments @|x| by the integral
//\[
//  \log x \equiv \int_1^{x} \frac{d\,t}{t}.
//\]
//For complex-valued arguments, @|z|, the complex logarithm is
//defined as
//\[
//  \log z \equiv \log |z| + i \arg z,
//\]
//where @|arg| is the complex argument of @|z|.
//@@Example
//The following piece of code plots the real-valued @|log|
//function over the interval @|[1,100]|:
//@<
//x = linspace(1,100);
//plot(x,log(x))
//xlabel('x');
//ylabel('log(x)');
//mprint('logplot');
//@>
//@figure logplot
//@@Signature
//function log LogFunction
//inputs x
//outputs y
//!

struct OpLog {
  static inline float func(float x) {return logf(x);}
  static inline double func(double x) {return log(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = logf(complex_abs<float>(xr,xi));
    yi = atan2f(xi,xr);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log(complex_abs<double>(xr,xi));
    yi = atan2(xi,xr);
  }
};

ArrayVector LogFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log function takes exactly one argument");
  Array input(arg[0]);
  if (!IsPositive(input)) input.forceComplex();
  return ArrayVector(UnaryOp<OpLog>(input));
}

//!
//@Module EXP Exponential Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the @|exp| function for its argument.  The general
//syntax for its use is
//@[
//   y = exp(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//Integer types are promoted to the @|double| type prior to
//calculation of the @|exp| function.  Output @|y| is of the
//same size and type as the input @|x|, (unless @|x| is an
//integer, in which case @|y| is a @|double| type).
//@@Function Internals
//Mathematically, the @|exp| function is defined for all real
//valued arguments @|x| as
//\[
//  \exp x \equiv e^{x},
//\]
//where
//\[
//  e = \sum_{0}^{\infty} \frac{1}{k!}
//\]
//and is approximately @|2.718281828459045| (returned by the function 
//@|e|).  For complex values
//@|z|, the famous Euler formula is used to calculate the 
//exponential
//\[
//  e^{z} = e^{|z|} \left[ \cos \Re z + i \sin \Re z \right]
//\]
//@@Example
//The following piece of code plots the real-valued @|exp|
//function over the interval @|[-1,1]|:
//@<
//x = linspace(-1,1);
//plot(x,exp(x))
//mprint('expplot1');
//@>
//@figure expplot1
//In the second example, we plot the unit circle in the 
//complex plane @|e^{i 2 pi x}| for @|x in [-1,1]|.
//@<
//x = linspace(-1,1);
//plot(exp(-i*x*2*pi))
//mprint('expplot2');
//@>
//@figure expplot2
//@@Signature
//function exp ExpFunction
//inputs x
//outputs y
//!

struct OpExp {
  static inline float func(float x) {return expf(x);}
  static inline double func(double x) {return exp(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expf(xr)*cosf(xi);
    yi = expf(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = exp(xr)*cos(xi);
    yi = exp(xr)*sin(xi);
  }
};

ArrayVector ExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Exp function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExp>(arg[0]));
}


//!
//@Module EXPM1 Exponential Minus One Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes @|exp(x)-1| function accurately for @|x|
//small.  The syntax for its use is
//@[
//   y = expm1(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Signature
//function expm1 ExpM1Function
//inputs x
//outputs y
//!

struct OpExpM1 {
  static inline float func(float x) {return expm1f(x);}
  static inline double func(double x) {return expm1(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expm1f(xr)*cosf(xi);
    yi = expm1f(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = expm1(xr)*cos(xi);
    yi = expm1(xr)*sin(xi);
  }
};

ArrayVector ExpM1Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ExpM1 function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExpM1>(arg[0]));
}


