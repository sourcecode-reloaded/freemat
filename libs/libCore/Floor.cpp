/*
 * Copyright (c) 2009 Samit Basu
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

#include "Operators.hpp"
#include "Array.hpp"
#include "Complex.hpp"
#include <cmath>

struct OpFloor {
  static inline float func(float t) { return floorf(t); }
  static inline double func(double t) {return floor(t); }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = floorf(x); ry = floorf(y);
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = floor(x); ry = floor(y);
  }
};

//@@Signature
//function floor FloorFunction jitsafe
//input x
//output y
//DOCBLOCK elementary_floor
ArrayVector FloorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("floor requires one argument");
  return ArrayVector(UnaryOp<OpFloor>(arg[0]));
}

JitScalarFunc1(floor,OpFloor::func);

struct OpFix {
  static inline float func(float t) { 
    if (t>0)
      return floorf(t);
    else if (t<0)
      return ceilf(t);
    else
      return t;
  }
  static inline double func(double t) {
    if (t>0)
      return floor(t);
    else if (t<0)
      return ceil(t);
    else
      return t;
  }
  template <typename T>
  static inline void func(T x, T y, T &rx, T &ry) {
    rx = OpFix::func(x);
    ry = OpFix::func(y);
  }
};

JitScalarFunc1(fix,OpFix::func);

//@@Signature
//function fix FixFunction
//input x
//output y
//DOCBLOCK mathfunctions_fix
ArrayVector FixFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("fix requires one argument");
  return ArrayVector(UnaryOp<OpFix>(arg[0]));
}

struct OpRem {
  template <typename T>
  static inline T func(T x, T n) {
    return x - OpFix::func(x/n)*n;
  }
  template <typename T>
  static inline void func(T x_real, T x_imag, T n_real, T n_imag, T &z_real, T &z_imag) {
    T q_real, q_imag;
    complex_divide(x_real,x_imag,n_real,n_imag,q_real,q_imag);
    T r_real, r_imag;
    OpFix::func(q_real,q_imag,r_real,r_imag);
    T s_real, s_imag;
    complex_multiply(r_real,r_imag,n_real,n_imag,s_real,s_imag);
    z_real = x_real - s_real;
    z_imag = x_imag - s_imag;
  }
};


JitScalarFunc2(rem,OpRem::func);

//@@Signature
//function rem RemFunction
//input x n
//output y
//DOCBLOCK mathfunctions_rem
ArrayVector RemFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("rem requires two arguments");
  return ArrayVector(DotOp<OpRem>(arg[0],arg[1]));
}
