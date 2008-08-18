#include "Array.hpp"
#include "Operators.hpp"

struct OpVecMean {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    T accum = 0;
    index_t col = src.col();
    while (src.col() == col) {
      accum += src.value();
      src.next();
    }
    dest[1] = accum/src.rows();
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    T accum_real = 0;
    T accum_imag = 0;
    index_t col = src.col();
    while (src.col() == col) {
      accum_real += src.realValue();
      accum_imag += src.imagValue();
      src.next();
    }
    dest_real[1] = accum_real/src.rows();
    dest_imag[1] = accum_imag/src.rows();
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    T accum = 0;
    for (index_t i=1;i<=src.length();i++)
      accum += src[i];
    dest[1] = accum/src.rows();
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    T accum_real = 0;
    T accum_imag = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      accum_real += src_real[i];
      accum_imag += src_imag[i];
    }
    dest_real[1] = accum_real/src_real.length();
    dest_imag[1] = accum_imag/src_imag.length();
  }
};

//!
//@Module MEAN Mean Function
//@@Section ELEMENTARY
//@@Usage
//Computes the mean of an array along a given dimension.  The general
//syntax for its use is
//@[
//  y = mean(x,d)
//@]
//where @|x| is an @|n|-dimensions array of numerical type.
//The output is of the same numerical type as the input.  The argument
//@|d| is optional, and denotes the dimension along which to take
//the mean.  The output @|y| is the same size as @|x|, except
//that it is singular along the mean direction.  So, for example,
//if @|x| is a @|3 x 3 x 4| array, and we compute the mean along
//dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
//@@Function Internals
//The output is computed via
//\[
//y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \frac{1}{N}
//\sum_{k=1}^{N} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//\]
//If @|d| is omitted, then the mean is taken along the 
//first non-singleton dimension of @|x|. 
//@@Example
//The following piece of code demonstrates various uses of the mean
//function
//@<
//A = [5,1,3;3,2,1;0,3,1]
//@>
//We start by calling @|mean| without a dimension argument, in which 
//case it defaults to the first nonsingular dimension (in this case, 
//along the columns or @|d = 1|).
//@<
//mean(A)
//@>
//Next, we take the mean along the rows.
//@<
//mean(A,2)
//@>
//@@Signature
//function mean MeanFunction
//inputs x dim
//outputs y
//!
ArrayVector MeanFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("mean requires at least one argument");
  Array input(arg[0]);
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  return ArrayVector(VectorOp<OpVecMean>(input,1,dim));
}
