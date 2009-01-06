#include "Operators.hpp"
#include "Array.hpp"
#include <cmath>

struct OpCeil {
  static inline float func(float t) { return ceilf(t); }
  static inline double func(double t) {return ceil(t); }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = ceilf(x); ry = ceilf(y);
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = ceil(x); ry = ceil(y);
  }
};

//!
//@Module CEIL Ceiling Function
//@@Section ELEMENTARY
//@@Usage
//Computes the ceiling of an n-dimensional array elementwise.  The
//ceiling of a number is defined as the smallest integer that is
//larger than or equal to that number. The general syntax for its use
//is
//@[
//   y = ceil(x)
//@]
//where @|x| is a multidimensional array of numerical type.  The @|ceil| 
//function preserves the type of the argument.  So integer arguments 
//are not modified, and @|float| arrays return @|float| arrays as 
//outputs, and similarly for @|double| arrays.  The @|ceil| function 
//is not defined for @|complex| or @|dcomplex| types.
//@@Example
//The following demonstrates the @|ceil| function applied to various
//(numerical) arguments.  For integer arguments, the ceil function has
//no effect:
//@<
//ceil(3)
//ceil(-3)
//@>
//Next, we take the @|ceil| of a floating point value:
//@<
//ceil(float(3.023))
//ceil(float(-2.341))
//@>
//Note that the return type is a @|float| also.  Finally, for a @|double|
//type:
//@<
//ceil(4.312)
//ceil(-5.32)
//@>
//@@Tests
//@$exact|y1=ceil(x1)
//@@Signature
//function ceil CeilFunction
//input x
//output y
//!
ArrayVector CeilFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("ceil requires one argument");
  return ArrayVector(UnaryOp<OpCeil>(arg[0]));
}
