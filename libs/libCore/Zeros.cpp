#include "Array.hpp"
#include "Utils.hpp"

//!
//@Module ZEROS Array of Zeros
//@@Section ARRAY
//@@Usage
//Creates an array of zeros of the specified size.  Two seperate 
//syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//   y = zeros(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//all zeros.  The type of @|y| is @|double|, a 64-bit floating
//point array.  To get arrays of other types, use the typecast 
//functions (e.g., @|uint8|, @|int8|, etc.).  An alternative syntax
//is to use the following notation:
//@[
//   y = zeros(d1,d2,...,dn,classname)
//@]
//where @|classname| is one of 'double', 'single', 'int8', 'uint8',
//'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64', 'float', 'logical'.  
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//   y = zeros([d1,d2,...,dn]),
//@]
//or
//@[
//   y = zeros([d1,d2,...,dn],classname).
//@]
//This syntax is more convenient for calling @|zeros| using a 
//variable for the argument.  In both cases, specifying only one
//dimension results in a square matrix output.
//@@Example
//The following examples demonstrate generation of some zero arrays 
//using the first form.
//@<
//zeros(2,3,2)
//zeros(1,3)
//@>
//The same expressions, using the second form.
//@<
//zeros([2,6])
//zeros([1,3])
//@>
//Finally, an example of using the type casting function @|uint16| to generate an array of 16-bit unsigned integers with zero values.
//@<
//uint16(zeros(3))
//@>
//Here we use the second syntax where the class of the output is specified 
//explicitly
//@<
//zeros(3,'int16')
//@>
//@@Tests
//@{ test_sparse69.m
//% Test the zeros function
//function x = test_sparse69
//xi = int32(sparse(100,200));
//yi = int32(zeros(100,200));
//xf = float(sparse(100,200));
//yf = float(zeros(100,200));
//xd = double(sparse(100,200));
//yd = double(zeros(100,200));
//xc = complex(sparse(100,200));
//yc = complex(zeros(100,200));
//xz = dcomplex(sparse(100,200));
//yz = dcomplex(zeros(100,200));
//x = testeq(xi,yi) & testeq(xf,yf) & testeq(xd,yd) & testeq(xc,yc) & testeq(xz,yz);
//@}
//@@Signature
//function zeros ZerosFunction
//inputs varargin
//outputs y
//!

#define MacroZeros(ctype,cls) \
  if (cp == #ctype) dataclass = cls;

ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      MacroExpandCasesSimple(MacroZeros);
      if (cp == "single") dataclass = Float;
      if (cp == "logical") dataclass = Bool;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to zeros function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  return ArrayVector(Array(dataclass,dims));
}

#undef MacroZeros
