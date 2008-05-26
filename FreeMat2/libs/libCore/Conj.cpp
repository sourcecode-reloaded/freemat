#include "Operators.hpp"
#include "Array.hpp"
#include <cmath>

struct OpConj {
  static inline float func(float t) { return t; }
  static inline double func(double t) {return t; }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = x; ry = -y;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = x; ry = -y;
  }
};

//!
//@Module CONJ Conjugate Function
//@@Section ELEMENTARY
//@@Usage
//Returns the complex conjugate of the input array for all elements.  The 
//general syntax for its use is
//@[
//   y = conj(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.  The output 
//is the same numerical type as the input.  The @|conj| function does
//nothing to real and integer types.
//@@Example
//The following demonstrates the complex conjugate applied to a complex scalar.
//@<
//conj(3+4*i)
//@>
//The @|conj| function has no effect on real arguments:
//@<
//conj([2,3,4])
//@>
//For a double-precision complex array,
//@<
//conj([2.0+3.0*i,i])
//@>
//@@Signature
//function conj ConjFunction
//inputs x
//outputs y
//!
ArrayVector ConjFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("conj function requires 1 argument");
  return ArrayVector(UnaryOp<OpConj>(arg[0]));
}
