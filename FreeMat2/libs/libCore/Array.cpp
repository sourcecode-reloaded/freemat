#include "Array.hpp"
#include "MemPtr.hpp"
#include <QtCore>

//!
//@Module PERMUTE Array Permutation Function
//@@Section ARRAY
//@@Usage
//The @|permute| function rearranges the contents of an array according
//to the specified permutation vector.  The syntax for its use is
//@[
//    y = permute(x,p)
//@]
//where @|p| is a permutation vector - i.e., a vector containing the 
//integers @|1...ndims(x)| each occuring exactly once.  The resulting
//array @|y| contains the same data as the array @|x|, but ordered
//according to the permutation.  This function is a generalization of
//the matrix transpose operation.
//@@Example
//Here we use @|permute| to transpose a simple matrix (note that permute
//also works for sparse matrices):
//@<
//A = [1,2;4,5]
//permute(A,[2,1])
//A'
//@>
//Now we permute a larger n-dimensional array:
//@<
//A = randn(13,5,7,2);
//size(A)
//B = permute(A,[3,4,2,1]);
//size(B)
//@>
//@@Tests
//@$"y=permute([1,2;3,4],[2,1])","[1,3;2,4]","exact"
//@$"y=size(permute(randn(13,5,7,2),[3,4,2,1]))","[7,2,5,13]","exact"
//@{ test_permute1.m
//function test_val = test_permute1
//z = rand(3,5,2,4,7);
//perm = [3,5,1,4,2];
//sizez = size(z);
//y = permute(z,perm);
//sizey = size(y);
//test_val = all(sizey == sizez(perm));
//@}
//@@Signature
//function permute PermuteFunction
//inputs x p
//outputs y
//!
ArrayVector PermuteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("permute requires 2 inputs, the array to permute, and the permutation vector");
  Array permutation(arg[1].asDenseArray().toClass(UInt32));
  int Adims = arg[0].dimensions().lastNotOne();
  if (permutation.length() != Adims)
    throw Exception("permutation vector must contain as many elements as the array to permute has dimensions");
  // Check that it is, in fact a permutation
  MemBlock<bool> p(Adims);
  bool *d = &p;
  for (int i=0;i<Adims;i++) d[i] = false;
  const BasicArray<uint32> &dp = permutation.constReal<uint32>();
  for (int i=1;i!=Adims;i++) {
    if ((dp[i] < 1) || (dp[i] > ((uint32)Adims)))
      throw Exception("permutation vector elements are limited to 1..ndims(A), where A is the array to permute");
    d[dp[i]-1] = true;
  }
  // Check that all are covered
  for (int i=0;i<Adims;i++)
    if (!d[i]) throw Exception("second argument to permute function is not a permutation (no duplicates allowed)");
  return ArrayVector(Permute(arg[0],dp));
}

//!
//@Module REPMAT Array Replication Function
//@@Section ARRAY
//@@Usage
//The @|repmat| function replicates an array the specified
//number of times.  The source and destination arrays may
//be multidimensional.  There are three distinct syntaxes for
//the @|repmap| function.  The first form:
//@[
//  y = repmat(x,n)
//@]
//replicates the array @|x| on an @|n-times-n| tiling, to create
//a matrix @|y| that has @|n| times as many rows and columns
//as @|x|.  The output @|y| will match @|x| in all remaining
//dimensions.  The second form is
//@[
//  y = repmat(x,m,n)
//@]
//And creates a tiling of @|x| with @|m| copies of @|x| in the
//row direction, and @|n| copies of @|x| in the column direction.
//The final form is the most general
//@[
//  y = repmat(x,[m n p...])
//@]
//where the supplied vector indicates the replication factor in 
//each dimension.  
//@@Example
//Here is an example of using the @|repmat| function to replicate
//a row 5 times.  Note that the same effect can be accomplished
//(although somewhat less efficiently) by a multiplication.
//@<
//x = [1 2 3 4]
//y = repmat(x,[5,1])
//@>
//The @|repmat| function can also be used to create a matrix of scalars
//or to provide replication in arbitrary dimensions.  Here we use it to
//replicate a 2D matrix into a 3D volume.
//@<
//x = [1 2;3 4]
//y = repmat(x,[1,1,3])
//@>
//@@Signature
//function repmat RepMatFunction
//inputs x rows cols
//outputs y
//!

template <typename T>
static Array RepMat(const BasicArray<T> &dp, const NTuple &outdim, const NTuple &repcount) {
  // Copy can work by pushing or by pulling.  I have opted for
  // pushing, because we can push a column at a time, which might
  // be slightly more efficient.
  index_t colsize = dp.rows();
  index_t colcount = dp.length()/colsize;
  // copySelect stores which copy we are pushing.
  NTuple originalSize(dp.dimensions());
  NTuple copySelect;
  // anchor is used to calculate where this copy lands in the output matrix
  // sourceAddress is used to track which column we are pushing in the
  // source matrix
  index_t copyCount = repcount.count();
  BasicArray<T> x(outdim);
  for (index_t i=1;i!=copyCount;i++) {
    // Reset the source address
    NTuple sourceAddress;
    // Next, we loop over the columns of the source matrix
    for (index_t j=1;j!=colcount;j++) {
      NTuple anchor;
      // We can calculate the anchor of this copy by multiplying the source
      // address by the copySelect vector
      for (int k=0;k<NDims;k++)
	anchor[k] = copySelect[k]*originalSize[k]+sourceAddress[k];
      // Now, we map this to a point in the destination array
      index_t destanchor = outdim.map(anchor);
      // And copy the elements
      for (index_t n=1;n!=colsize;n++)
	x[destanchor+n] = dp[j*colsize+n];
      // Now increment the source address
      originalSize.increment(sourceAddress,0);
    }
    repcount.increment(copySelect);
  }
  return Array(x);
}

template <typename T>
static Array RepMat(const Array &dp, const NTuple &outdim, const NTuple &repcount) {
  if (dp.isScalar()) {
    if (dp.allReal()) 
      return Array(Uniform(outdim,dp.constRealScalar<T>()));
    else
      return Array(Uniform(outdim,dp.constRealScalar<T>()),
		   Uniform(outdim,dp.constImagScalar<T>()));
  }
  if (dp.isSparse()) {
    if (dp.allReal())
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount));
    else
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount),
		   RepMat(dp.constImagSparse<T>(),outdim,repcount));
  }
  if (dp.allReal())
    return Array(RepMat(dp.constReal<T>(),outdim,repcount));
  else
    return Array(RepMat(dp.constReal<T>(),outdim,repcount),
		 RepMat(dp.constImag<T>(),outdim,repcount));
}

#define MacroRepMat(ctype,cls)					\
  case cls: return ArrayVector(RepMat<ctype>(x,outdims,repcount));

ArrayVector RepMatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("repmat function requires at least two arguments");
  Array x(arg[0]);
  if (x.isEmpty()) return ArrayVector(arg[0]);
  NTuple repcount;
  // Case 1, look for a scalar second argument
  if ((arg.size() == 2) && (arg[1].isScalar())) {
    Array t(arg[1]);
    repcount[0] = t.asInteger();
    repcount[1] = t.asInteger();
  } 
  // Case 2, look for two scalar arguments
  else if ((arg.size() == 3) && (arg[1].isScalar()) && (arg[2].isScalar())) {
    repcount[0] = arg[1].asInteger();
    repcount[1] = arg[1].asInteger();
  }
  // Case 3, look for a vector second argument
  else {
    if (arg.size() > 2) throw Exception("unrecognized form of arguments for repmat function");
    repcount = ConvertArrayToNTuple(arg[1]);
  }
  if (!repcount.isValid())
    throw Exception("negative replication counts not allowed in argument to repmat function");
  // All is peachy.  Allocate an output array of sufficient size.
  NTuple outdims;
  for (int i=0;i<NDims;i++)
    outdims[i] = x.dimensions()[i]*repcount[i];
  switch (x.dataClass()) {
  default: throw Exception("Unhandled type for repmat");
    MacroExpandCasesAll(MacroRepMat);
  case Struct:
    return ArrayVector(RepMat(x.constStructPtr(),outdims,repcount));
  }
}
