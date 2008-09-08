#include "Operators.hpp"
#include "Complex.hpp"
#include "Math.hpp"
#include <cmath>

struct OpAbs {
  static inline float func(float t) {return fabsf(t);}
  static inline double func(double t) {return fabs(t);}
  template <typename T>
  static inline void func(T x, T y, T &rx, T &ry) {
    rx = complex_abs(x,y); ry = 0;
  }
};

//!
//@Module ABS Absolute Value Function
//@@Section ELEMENTARY
//@@Usage
//Returns the absolute value of the input array for all elements.  The 
//general syntax for its use is
//@[
//   y = abs(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.  The output 
//is the same numerical type as the input, unless the input is @|complex|
//or @|dcomplex|.  For @|complex| inputs, the absolute value is a floating
//point array, so that the return type is @|float|.  For @|dcomplex|
//inputs, the absolute value is a double precision floating point array, so that
//the return type is @|double|.
//@@Example
//The following demonstrates the @|abs| applied to a complex scalar.
//@<
//abs(3+4*i)
//@>
//The @|abs| function applied to integer and real values:
//@<
//abs([-2,3,-4,5])
//@>
//For a double-precision complex array,
//@<
//abs([2.0+3.0*i,i])
//@>
//@@Tests
//@$"y=abs('hello')","[104,101,108,108,111]","exact"
//@$"y=abs(3+4i)","5","exact"
//@@Signature
//function abs AbsFunction
//inputs x
//outputs y
//!
ArrayVector AbsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("abs function requires 1 argument");
  return ArrayVector(Real(UnaryOp<OpAbs>(arg[0])));
}
