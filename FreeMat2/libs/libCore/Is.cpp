#include "Array.hpp"
#include "Interpreter.hpp"

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
  Array tmp(arg[0]);
  if (tmp.isReferenceType())
    throw Exception("isnan is not defined for reference types");
  ArrayVector retval;
  int len(tmp.getLength());
  logical *op = (logical *) Malloc(len*sizeof(logical));
  switch (tmp.dataClass()) {
  default: throw Exception("unhandled type as argument to isnan");
  case FM_STRING:
  case FM_LOGICAL:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_INT32:
  case FM_UINT64:
  case FM_INT64:
    break;
  case FM_FLOAT: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsNaN(dp[i]) ? 1 : 0;
    break;
  }
  case FM_DOUBLE: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsNaN(dp[i]) ? 1 : 0;
    break;
  }
  case FM_COMPLEX: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsNaN(dp[2*i]) || IsNaN(dp[2*i+1])) ? 1 : 0;
    break;
  }
  case FM_DCOMPLEX: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsNaN(dp[2*i]) || IsNaN(dp[2*i+1])) ? 1 : 0;
    break;
  }
  }
  retval.push_back(Array(FM_LOGICAL,tmp.dimensions(),op));
  return(retval);
}

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
//!
ArrayVector IsInfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isinf function takes one argument - the array to test");
  Array tmp(arg[0]);
  if (tmp.isReferenceType())
    throw Exception("isinf is not defined for reference types");
  ArrayVector retval;
  int len(tmp.getLength());
  logical *op = (logical *) Malloc(len*sizeof(logical));
  switch (tmp.dataClass()) {
  default: throw Exception("unhandled type as argument to isinf");
  case FM_STRING:
  case FM_LOGICAL:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_INT32:
  case FM_UINT64:
  case FM_INT64:
    break;
  case FM_FLOAT: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsInfinite(dp[i]) ? 1 : 0;
    break;
  }
  case FM_DOUBLE: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsInfinite(dp[i]) ? 1 : 0;
    break;
  }
  case FM_COMPLEX: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsInfinite(dp[2*i]) || IsInfinite(dp[2*i+1])) ? 1 : 0;
    break;
  }
  case FM_DCOMPLEX: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsInfinite(dp[2*i]) || IsInfinite(dp[2*i+1])) ? 1 : 0;
    break;
  }
  }
  retval.push_back(Array(FM_LOGICAL,tmp.dimensions(),op));
  return(retval);
}
