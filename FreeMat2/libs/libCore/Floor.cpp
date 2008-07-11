#include "Operators.hpp"
#include "Array.hpp"
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

//!
//@Module FLOOR Floor Function
//@@Section ELEMENTARY
//@@Usage
//Computes the floor of an n-dimensional array elementwise.  The
//floor of a number is defined as the smallest integer that is
//less than or equal to that number. The general syntax for its use
//is
//@[
//   y = floor(x)
//@]
//where @|x| is a multidimensional array of numerical type.  The @|floor| 
//function preserves the type of the argument.  So integer arguments 
//are not modified, and @|float| arrays return @|float| arrays as 
//outputs, and similarly for @|double| arrays.  The @|floor| function 
//is not defined for @|complex| or @|dcomplex| types.
//@@Example
//The following demonstrates the @|floor| function applied to various
//(numerical) arguments.  For integer arguments, the floor function has
//no effect:
//@<
//floor(3)
//floor(-3)
//@>
//Next, we take the @|floor| of a floating point value:
//@<
//floor(3.023f)
//floor(-2.341f)
//@>
//Note that the return type is a @|float| also.  Finally, for a @|double|
//type:
//@<
//floor(4.312)
//floor(-5.32)
//@>
//@@Signature
//function floor FloorFunction
//input x
//output y
//!
ArrayVector FloorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("floor requires one argument");
  return ArrayVector(UnaryOp<OpFloor>(arg[0]));
}
