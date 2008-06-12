#include "Array.hpp"
#include "Interpreter.hpp"
#include "IEEEFP.hpp"
#include "Operators.hpp"

//!
//@Module ISSET Test If Variable Set
//@@Section INSPECTION
//@@Usage
//Tests for the existence and non-emptiness of a variable.
//the general syntax for its use is
//@[
//   y = isset('name')
//@]
//where @|name| is the name of the variable to test.  This
//is functionally equivalent to 
//@[
//   y = exist('name','var') & ~isempty(name)
//@]
//It returns a @|logical| 1 if the variable is defined 
//in the current workspace, and is not empty, and returns
//a 0 otherwise.
//@@Example
//Some simple examples of using @|isset|
//@<
//who
//isset('a')
//a = [];
//isset('a')
//a = 2;
//isset('a')
//@>
//@@Signature
//sfunction isset IsSetFunction
//inputs name
//outpus flag
//!
ArrayVector IsSetFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("isset function takes at least one argument - the name of the variable to check for");
  QString fname = arg[0].asString();
  bool isDefed;
  ArrayReference d = eval->getContext()->lookupVariable(fname);
  isDefed = (d.valid());
  if (isDefed && !d->isEmpty())
    return ArrayVector(Array(bool(true)));
  else
    return ArrayVector(Array(bool(false)));
}

//!
//@Module ISSPARSE Test for Sparse Matrix
//@@Section INSPECTION
//@@Usage
//Test a matrix to see if it is sparse or not.  The general
//format for its use is 
//@[
//   y = issparse(x)
//@]
//This function returns true if @|x| is encoded as a sparse
//matrix, and false otherwise.
//@@Example
//Here is an example of using @|issparse|:
//@<
//a = [1,0,0,5;0,3,2,0]
//issparse(a)
//A = sparse(a)
//issparse(A)
//@>
//@@Signature
//function issparse IsSparseFunction
//inputs x
//outputs flag
//!
ArrayVector IsSparseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("issparse function takes one argument - the array to test");
  return ArrayVector(Array(arg[0].isSparse()));
}

struct OpIsNaN {
  static inline float func(float t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
};


//!
//@Module ISNAN Test for Not-a-Numbers
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are NaN's (i.e.,
//Not-a-Numbers).  The usage is
//@[
//   y = isnan(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for complex data types that
//the result is true if either the real or imaginary parts
//are NaNs.
//@@Example
//Suppose we have an array of floats with one element that
//is @|nan|:
//@<
//a = [1.2 3.4 nan 5]
//isnan(a)
//@>
//@@Signature
//function isnan IsNaNFunction
//inputs x
//outputs y
//!
ArrayVector IsNaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
   throw Exception("isnan function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsNaN>(arg[0]).toClass(Bool));
}


struct OpIsInf {
  static inline float func(float t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
};

//!
//@Module ISINF Test for infinities
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are infs (i.e.,
//infinities).  The usage is
//@[
//   y = isinf(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for @|complex| or @|dcomplex| data types that
//the result is true if either the real or imaginary parts
//are infinite.
//@@Example
//Suppose we have an array of floats with one element that
//is @|inf|:
//@<
//a = [1.2 3.4 inf 5]
//isinf(a)
//b = 3./[2 5 0 3 1]
//@>
//@@Signature
//function isinf IsInfFunction
//inputs x
//outputs y
//!
ArrayVector IsInfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isinf function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsInf>(arg[0]).toClass(Bool));
}

//!
//@Module ISEMPTY Test For Variable Empty
//@@Section INSPECTION
//@@Usage
//The @|isempty| function returns a boolean that indicates
//if the argument variable is empty or not.  The general
//syntax for its use is
//@[
//  y = isempty(x).
//@]
//@@Examples
//Here are some examples of the @|isempty| function
//@<
//a = []
//isempty(a)
//b = 1:3
//isempty(b)
//@>
//Note that if the variable is not defined, @|isempty| 
//does not return true.
//@<1
//clear x
//isempty(x)
//@>
//@@Tests
//@{ test_empty.m
//% Test the arithmetic operators with empty arguments
//function test_val = test_empty
//test_val = isempty([]+[]);
//test_val = test_val & isempty([]-[]);
//test_val = test_val & isempty(-[]);
//test_val = test_val & isempty([]*[]);
//test_val = test_val & isempty([]/[]);
//test_val = test_val & isempty([]\[]);
//test_val = test_val & isempty([].*[]);
//test_val = test_val & isempty([]./[]);
//test_val = test_val & isempty([].\[]);
//test_val = test_val & isempty([]^[]);
//test_val = test_val & isempty([].^[]);
//test_val = test_val & isempty([]>[]);
//test_val = test_val & isempty([]>=[]);
//test_val = test_val & isempty([]<[]);
//test_val = test_val & isempty([]<=[]);
//@}
//@@Signature
//function isempty IsEmptyFunction
//inputs x
//outputs flag
//!
ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("isempty function requires at least input argument");
  return ArrayVector(Array(bool(arg[0].isEmpty())));
}
