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

#include "Array.hpp"
#include "Algorithms.hpp"

//!
//@Module LOGICAL Convert to Logical
//@@Section TYPECAST
//@@Usage
//Converts the argument to a logical array.  The syntax
//for its use is
//@[
//   y = logical(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Any nonzero 
//element maps to a logical 1.
//@@Example
//Here we convert an integer array to @|logical|:
//@<
//logical([1,2,3,0,0,0,5,2,2])
//@>
//The sampe example with double precision values:
//@<
//logical([pi,pi,0,e,0,-1])
//@>
//@@Signature
//function logical LogicalFunction
//inputs x
//outputs y
//!
ArrayVector LogicalFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Bool));
}

//!
//@Module STRING Convert Array to String
//@@Section TYPECAST
//@@Usage
//Converts the argument array into a string.  The syntax for its
//use is
//@[
//   y = string(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.
//@@Example
//Here we take an array containing ASCII codes for a string, and
//convert it into a string.
//@<
//a = [104,101,108,108,111]
//string(a)
//@>
//@@Signature
//function string StringFunction
//inputs x
//outputs y
//!
ArrayVector StringFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(StringArray));
}

//!
//@Module UINT8 Convert to Unsigned 8-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an unsigned 8-bit Integer.  The syntax
//for its use is
//@[
//   y = uint8(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for an unsigned 8-bit integer of @|[0,255]|, the least significant
//8 bits of @|x| are used after conversion to an integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|uint8|.
//@<
//uint8(200)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 8 least significant bits of the argument.
//@<
//uint8(400)
//@>
//In the next example, a negative integer is passed in.  The result is the 8 least significant bits of the argument, \emph{after} taking the 2's complement.
//@<
//uint8(-100)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
//@<
//uint8(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
//@<
//uint8(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//uint8('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//uint8({4})
//@>
//@@Signature
//function uint8 UInt8Function
//inputs x
//outputs y
//!
ArrayVector UInt8Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt8));
}

//!
//@Module UINT16 Convert to Unsigned 16-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an unsigned 16-bit Integer.  The syntax
//for its use is
//@[
//   y = uint16(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for an unsigned 16-bit integer of @|[0,65535]|, the least significant
//16 bits of @|x| are used after conversion to an integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|uint16|.
//@<
//uint16(200)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 16 least significant bits of the argument.
//@<
//uint16(99400)
//@>
//In the next example, a negative integer is passed in.  The result is the 16 least significant bits of the argument, \emph{after} taking the 2's complement.
//@<
//uint16(-100)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
//@<
//uint16(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
//@<
//uint16(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//uint16('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//uint16({4})
//@>
//@@Signature
//function uint16 UInt16Function
//inputs x
//outputs y
//!
ArrayVector UInt16Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt16));
}

//!
//@Module UINT32 Convert to Unsigned 32-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an unsigned 32-bit Integer.  The syntax
//for its use is
//@[
//   y = uint32(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for an unsigned 32-bit integer of @|[0,4294967295]|, the least 
//significant 32 bits of @|x| are used after conversion to an integer).  
//Note that both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|uint32|.
//@<
//uint32(200)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 32 least significant bits of the argument.
//@<
//uint32(40e9)
//@>
//In the next example, a negative integer is passed in.  The result is the 32 least significant bits of the argument, \emph{after} taking the 2's complement.
//@<
//uint32(-100)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
//@<
//uint32(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
//@<
//uint32(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//uint32('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//uint32({4})
//@>
//@@Signature
//function uint32 UInt32Function
//inputs x
//outputs y
//!
ArrayVector UInt32Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt32));
}

//!
//@Module UINT64 Convert to Unsigned 64-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an unsigned 64-bit Integer.  The syntax
//for its use is
//@[
//   y = uint64(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for an unsigned 64-bit integer of @|[0,2^64-1]|, the least 
//significant 64 bits of @|x| are used after conversion to an integer).  
//Note that both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|uint64|.
//@<
//uint64(200)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 64 least significant bits of the argument.
//@<
//uint64(40e9)
//@>
//In the next example, a negative integer is passed in.  The result is the 64 least significant bits of the argument, \emph{after} taking the 2's complement.
//@<
//uint64(-100)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
//@<
//uint64(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
//@<
//uint64(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//uint64('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//uint64({4})
//@>
//@@Signature
//function uint64 UInt64Function
//inputs x
//outputs y
//!
ArrayVector UInt64Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt64));
}

//!
//@Module INT8 Convert to Signed 8-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an signed 8-bit Integer.  The syntax
//for its use is
//@[
//   y = int8(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for a signed 8-bit integer of @|[-128,127]|, the least significant
//8 bits of @|x| are used after conversion to a signed integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|int8|.  First, the routine uses
//@<
//int8(100)
//int8(-100)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 8 least significant bits of the argument.
//@<
//int8(400)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
//@<
//int8(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
//@<
//int8(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//int8('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//int8({4})
//@>
//@@Signature
//function int8 Int8Function
//inputs x
//outputs y
//!
ArrayVector Int8Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int8));
}

//!
//@Module INT16 Convert to Signed 16-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an signed 16-bit Integer.  The syntax
//for its use is
//@[
//   y = int16(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for a signed 16-bit integer of @|[-32768,32767]|, the least significant
//16 bits of @|x| are used after conversion to a signed integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|int16|.  First, the routine uses
//@<
//int16(100)
//int16(-100)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 16 least significant bits of the argument.
//@<
//int16(40000)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
//@<
//int16(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
//@<
//int16(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//int16('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//int16({4})
//@>
//@@Signature
//function int16 Int16Function
//inputs x
//outputs y
//!
ArrayVector Int16Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int16));
}

//!
//@Module INT32 Convert to Signed 32-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an signed 32-bit Integer.  The syntax
//for its use is
//@[
//   y = int32(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for a signed 32-bit integer of @|[-2147483648,2147483647]|, the least significant
//32 bits of @|x| are used after conversion to a signed integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|int32|.  First, the routine uses
//@<
//int32(100)
//int32(-100)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 32 least significant bits of the argument.
//@<
//int32(40e9)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
//@<
//int32(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
//@<
//int32(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//int32('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//int32({4})
//@>
//@@Signature
//function int32 Int32Function
//inputs x
//outputs y
//!
ArrayVector Int32Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int32));
}

//!
//@Module INT64 Convert to Signed 64-bit Integer
//@@Section TYPECAST
//@@Usage
//Converts the argument to an signed 64-bit Integer.  The syntax
//for its use is
//@[
//   y = int64(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion
//follows the general C rules (e.g., if @|x| is outside the normal
//range for a signed 64-bit integer of @|[-2^63,2^63-1]|, the least significant
//64 bits of @|x| are used after conversion to a signed integer).  Note that
//both @|NaN| and @|Inf| both map to 0.
//@@Example
//The following piece of code demonstrates several uses of @|int64|.  First, the routine uses
//@<
//int64(100)
//int64(-100)
//@>
//In the next example, an integer outside the range  of the type is passed in.  The result is the 64 least significant bits of the argument.
//@<
//int64(40e9)
//@>
//In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
//@<
//int64(pi)
//@>
//In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
//@<
//int64(5+2*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//int64('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//int64({4})
//@>
//@@Signature
//function int64 Int64Function
//inputs x
//outputs y
//!
ArrayVector Int64Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int64));
}

//!
//@Module SINGLE Convert to 32-bit Floating Point
//@@Section TYPECAST
//@@Usage
//A synonym for the @|float| function, converts the argument to
//a 32-bit floating point number.  The syntax
//for its use is
//@[
//   y = single(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion 
//follows the general C rules.  Note that both @|NaN| and @|Inf| 
//are both preserved under type conversion.
//!

//!
//@Module FLOAT Convert to 32-bit Floating Point
//@@Section TYPECAST
//@@Usage
//Converts the argument to a 32-bit floating point number.  The syntax
//for its use is
//@[
//   y = float(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| are both preserved under type conversion.
//@@Example
//The following piece of code demonstrates several uses of @|float|.  First, we convert from an integer (the argument is an integer because no decimal is present):
//@<
//float(200)
//@>
//In the next example, a double precision argument is passed in (the presence of a decimal without the @|f| suffix implies double precision).
//@<
//float(400.0)
//@>
//In the next example, a dcomplex argument is passed in.  The result is the real part of the argument, and in this context, @|float| is equivalent to the function @|real|.
//@<
//float(3.0+4.0*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//float('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//float({4})
//@>
//@@Tests
//@{ test_sparse58.m
//% Test type conversion functions
//function x = test_sparse58
//[yi,zi] = sparse_test_mat('int32',400);
//[yf,zf] = sparse_test_mat('float',400);
//[yd,zd] = sparse_test_mat('double',400);
//[yc,zc] = sparse_test_mat('complex',400);
//[yz,zz] = sparse_test_mat('dcomplex',400);
//
//xi = testeq(int32(yi),sparse(int32(zi))) & ...
//    testeq(float(yi),sparse(float(zi))) & ...
//    testeq(double(yi),sparse(double(zi))) & ...
//    testeq(complex(yi),sparse(complex(zi))) & ...
//    testeq(dcomplex(yi),sparse(dcomplex(zi)));
//xf = testeq(int32(yf),sparse(int32(zf))) & ...
//    testeq(float(yf),sparse(float(zf))) & ...
//    testeq(double(yf),sparse(double(zf))) & ...
//    testeq(complex(yf),sparse(complex(zf))) & ...
//    testeq(dcomplex(yf),sparse(dcomplex(zf)));
//xd = testeq(int32(yd),sparse(int32(zd))) & ...
//    testeq(float(yd),sparse(float(zd))) & ...
//    testeq(double(yd),sparse(double(zd))) & ...
//    testeq(complex(yd),sparse(complex(zd))) & ...
//    testeq(dcomplex(yd),sparse(dcomplex(zd)));
//xc = testeq(int32(yc),sparse(int32(zc))) & ...
//    testeq(float(yc),sparse(float(zc))) & ...
//    testeq(double(yc),sparse(double(zc))) & ...
//    testeq(complex(yc),sparse(complex(zc))) & ...
//    testeq(dcomplex(yc),sparse(dcomplex(zc)));
//xz = testeq(int32(yz),sparse(int32(zz))) & ...
//    testeq(float(yz),sparse(float(zz))) & ...
//    testeq(double(yz),sparse(double(zz))) & ...
//    testeq(complex(yz),sparse(complex(zz))) & ...
//    testeq(dcomplex(yz),sparse(dcomplex(zz)));
//
//x = xz & xc & xd & xf & xi;
//@}
//@@Signature
//function float FloatFunction
//inputs x
//outputs y
//@@Signature
//function single FloatFunction
//inputs x
//outputs y
//!
ArrayVector FloatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Float));
}

//!
//@Module DOUBLE Convert to 64-bit Floating Point
//@@Section TYPECAST
//@@Usage
//Converts the argument to a 64-bit floating point number.  The syntax
//for its use is
//@[
//   y = double(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| are both preserved under type conversion.
//@@Example
//The following piece of code demonstrates several uses of @|double|.  First, we convert from an integer (the argument is an integer because no decimal is present):
//@<
//double(200)
//@>
//In the next example, a single precision argument is passed in (the presence of  the @|f| suffix implies single precision).
//@<
//double(400.0f)
//@>
//In the next example, a dcomplex argument is passed in.  The result is the real part of the argument, and in this context, @|double| is equivalent to the function @|real|.
//@<
//double(3.0+4.0*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//double('helo')
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//double({4})
//@>
//@@Signature
//function double DoubleFunction
//inputs x
//outputs y
//!
ArrayVector DoubleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Double));
}

//!
//@Module COMPLEX Create a Complex Number
//@@Section TYPECAST
//@@Usage
//Converts the two real input arguments into the real and imaginary part
//(respectively) of a complex number.  The syntax
//for its use is
//@[
//   y = complex(x,z)
//@]
//where @|x| and @|z| are @|n|-dimensional numerical arrays of the same size and type.  
//@@Signature
//function complex ComplexFunction
//inputs x z
//outputs y
//!
ArrayVector ComplexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) 
    throw Exception("type conversion function requires at least one argument");
  if (arg.size() == 1)
    return ArrayVector(arg[0]);
  else
    return ArrayVector(MergeToComplex(arg[0],arg[1]));
}

//!
//@Module DCOMPLEX Convert to 32-bit Complex Floating Point
//@@Section TYPECAST
//@@Usage
//Converts the argument to a 32-bit complex floating point number.  The syntax
//for its use is
//@[
//   y = dcomplex(x)
//@]
//where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| in the real and imaginary parts are both preserved under type conversion.
//@@Example
//The following piece of code demonstrates several uses of @|dcomplex|.  First, we convert from an integer (the argument is an integer because no decimal is present):
//@<
//dcomplex(200)
//@>
//In the next example, a double precision argument is passed in (the presence of a decimal without the @|f| suffix implies double precision).
//@<
//dcomplex(400.0)
//@>
//In the next example, a complex argument is passed in. 
//@<
//dcomplex(3.0+4.0*i)
//@>
//In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
//@<
//dcomplex('h')
//@>
//In the next example, the @|NaN| argument is converted.
//@<
//dcomplex(nan)
//@>
//In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
//@<1
//dcomplex({4})
//@>
//@@Signature
//function dcomplex DcomplexFunction
//inputs x
//outputs y
//!
ArrayVector DcomplexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Double));
}

//!
//@Module TYPEOF Determine the Type of an Argument
//@@Section INSPECTION
//@@Usage
//Returns a string describing the type of an array.  The syntax for its use is
//@[
//   y = typeof(x),
//@]
//The returned string is one of
//\begin{itemize}
//\item @|'cell'| for cell-arrays
//\item @|'struct'| for structure-arrays
//\item @|'logical'| for logical arrays
//\item @|'uint8'| for unsigned 8-bit integers
//\item @|'int8'| for signed 8-bit integers
//\item @|'uint16'| for unsigned 16-bit integers
//\item @|'int16'| for signed 16-bit integers
//\item @|'uint32'| for unsigned 32-bit integers
//\item @|'int32'| for signed 32-bit integers
//\item @|'float'| for 32-bit floating point numbers
//\item @|'double'| for 64-bit floating point numbers
//\item @|'complex'| for complex floating point numbers with 32-bits per field
//\item @|'dcomplex'| for complex floating point numbers with 64-bits per field
//\item @|'string'| for string arrays
//\end{itemize}
//@@Example
//The following piece of code demonstrates the output of the @|typeof| command for each possible type.  The first example is with a simple cell array.
//@<
//typeof({1})
//@>
//The next example uses the @|struct| constructor to make a simple scalar struct.
//@<
//typeof(struct('foo',3))
//@>
//The next example uses a comparison between two scalar integers to generate a scalar logical type.
//@<
//typeof(3>5)
//@>
//For the smaller integers, and the 32-bit unsigned integer types, the typecast operations are used to generate the arguments.
//@<
//typeof(uint8(3))
//typeof(int8(8))
//typeof(uint16(3))
//typeof(int16(8))
//typeof(uint32(3))
//@>
//The 32-bit signed integer type is the default for integer arguments.
//@<
//typeof(-3)
//typeof(8)
//@>
//Float, double, complex and double-precision complex types can be created using the suffixes.
//@<
//typeof(1.0f)
//typeof(1.0D)
//typeof(1.0f+i)
//typeof(1.0D+2.0D*i)
//@>
//@@Tests
//@{ test_typeof1.m
//% Check the typeof function for integer types
//function test_val = test_typeof1
//a = 2;
//test_val = test(strcmp(typeof(a),'double'));
//@}
//@{ test_typeof2.m
//% Check the typeof function for double types
//function test_val = test_typeof2
//a = 2.0;
//test_val = test(strcmp(typeof(a),'double'));
//@}
//@{ test_typeof3.m
//% Check the typeof function for float types
//function test_val = test_typeof3
//a = 2.0f;
//test_val = test(strcmp(typeof(a),'single'));
//@}
//@{ test_typeof4.m
//% Check the typeof function for complex types
//function test_val = test_typeof4
//a = 2.0f + i;
//test_val = test(strcmp(typeof(a),'single'));
//@}
//@{ test_typeof5.m
//% Check the typeof function for dcomplex types
//function test_val = test_typeof5
//a = 2.0 + i;
//test_val = test(strcmp(typeof(a),'double'));
//@}
//@@Signature
//function typeof TypeOfFunction
//inputs x
//outputs typename
//!
ArrayVector TypeOfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("typeof function requires exactly one argument");
  return ArrayVector(Array(arg[0].className()));
}
