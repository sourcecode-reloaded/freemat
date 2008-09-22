#include "Operators.hpp"
#include "Math.hpp"
#include "Complex.hpp"

struct OpVecProd {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    T result = 1;
    index_t count = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      result = result * src.value();
      src.next();
    }
    if (count < src.rows())
      result = 0;
    dest[1] = result;
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    T result_real = 1;
    T result_imag = 0;
    index_t count = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      complex_multiply(result_real,result_imag,src.realValue(),src.imagValue(),
		       result_real,result_imag);
      src.next();
    }
    if (count < src.rows()) {
      result_real = 0;
      result_imag = 0;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    T result = 1;
    for (index_t i=1;i<=src.length();i++)
      result = result * src[i];
    dest[1] = result;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    T result_real = 1;
    T result_imag = 0;
    for (index_t i=1;i<=src_real.length();i++)
      complex_multiply(result_real,result_imag,src_real[i],src_imag[i],
		       result_real,result_imag);
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
  }
};

//!
//@Module PROD Product Function
//@@Section ELEMENTARY
//@@Usage
//Computes the product of an array along a given dimension.  The general
//syntax for its use is
//@[
//   y = prod(x,d)
//@]
//where @|x| is an @|n|-dimensions array of numerical type.
//The output is of the same numerical type as the input, except 
//for integer types, which are automatically promoted to @|int32|.
// The argument @|d| is optional, and denotes the dimension along 
//which to take the product.  The output is computed via
//\[
//  y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
//    \prod_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//\]
//If @|d| is omitted, then the product is taken along the 
//first non-singleton dimension of @|x|. Note that by definition
//(starting with FreeMat 2.1) @|prod([]) = 1|.
//@@Example
//The following piece of code demonstrates various uses of the product
//function
//@<
//A = [5,1,3;3,2,1;0,3,1]
//@>
//We start by calling @|prod| without a dimension argument, in which case it defaults to the first nonsingular dimension (in this case, along the columns or @|d = 1|).
//@<
//prod(A)
//@>
//Next, we take the product along the rows.
//@<
//prod(A,2)
//@>
//@@Tests
//@$y1=prod(x1)
//@@Signature
//function prod ProdFunction
//inputs x dimension
//outputs y
//!
ArrayVector ProdFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("prod requires at least one argument");
  Array input(arg[0]);
    int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  return ArrayVector(VectorOp<OpVecProd>(input,1,dim));
}
