#include "Math.hpp"

//!
//@Module INV Invert Matrix
//@@Section TRANSFORMS
//@@Usage
//Inverts the argument matrix, provided it is square and invertible.
//The syntax for its use is
//@[
//   y = inv(x)
//@]
//Internally, the @|inv| function uses the matrix divide operators.
//For sparse matrices, a sparse matrix solver is used.
//@@Example
//Here we invert some simple matrices
//@<
//a = randi(zeros(3),5*ones(3))
//b = inv(a)
//a*b
//b*a
//@>
//@@Signature
//function inv InvFunction
//inputs x
//outputs y
//!
ArrayVector InvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("inv function needs at least one argument");
  return ArrayVector(InvertMatrix(arg[0]));
}
