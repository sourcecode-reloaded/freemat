#include "Operators.hpp"
#include "Math.hpp"


template <class T>
struct XNEntryReal {
  index_t n;
  T x;
};

template <class T>
static bool RealLess(const XNEntryReal<T>& a, const XNEntryReal<T>& b) {
  return a.x < b.x;
}

template <>
static bool RealLess(const XNEntryReal<Array>& a, const XNEntryReal<Array>& b) {
  return a.x.asString() < b.x.asString();
}

template <class T>
static bool RealGreater(const XNEntryReal<T>& a, const XNEntryReal<T>& b) {
  return a.x > b.x;
}

template <>
static bool RealGreater(const XNEntryReal<Array>& a, const XNEntryReal<Array>& b) {
  return a.x.asString() > b.x.asString();
}

template <class T>
struct XNEntryComplex {
  index_t n;
  T x;
  T y;
};

template <class T>
static bool ComplexLess(const XNEntryComplex<T>& a, const XNEntryComplex<T>& b) {
  return complex_lt(a.x,a.y,b.x,b.y);
}

template <class T>
static bool ComplexGreater(const XNEntryComplex<T>& a, const XNEntryComplex<T>& b) {
  return complex_gt(a.x,a.y,b.x,b.y);
}


struct OpVecSortAscend {
  template <typename T>
  static inline void func(ConstSparseIterator<T> &,
			  SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src,
			  BasicArray<T> &dest,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryReal<T> > tmp(int(src.length()));
    for (index_t i=1;i<=src.length();i++) {
      tmp[int(i-1)].n = i; tmp[int(i-1)].x = src[i];
    }
    qStableSort(tmp.begin(),tmp.end(),RealLess<T>);
    for (int i=0;i<tmp.size();i++) {
      dest[i+1] = tmp[i].x;
      dest_index[i+1] = tmp[i].n;
    }
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T> & dest_real,
			  BasicArray<T> & dest_imag,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryComplex<T> > tmp(int(src_real.length()));
    for (index_t i=1;i<=src_real.length();i++) {
      tmp[int(i-1)].n = i; 
      tmp[int(i-1)].x = src_real[i];
      tmp[int(i-1)].y = src_imag[i];
    }
    qStableSort(tmp.begin(),tmp.end(),ComplexLess<T>);
    for (int i=0;i<tmp.size();i++) {
      dest_real[index_t(i+1)] = tmp[i].x;
      dest_imag[index_t(i+1)] = tmp[i].y;
      dest_index[index_t(i+1)] = tmp[i].n;
    }
  }
};

struct OpVecSortDescend {
  template <typename T>
  static inline void func(ConstSparseIterator<T> &,
			  SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src,
			  BasicArray<T> &dest,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryReal<T> > tmp(int(src.length()));
    for (index_t i=1;i<=src.length();i++) {
      tmp[int(i-1)].n = i; tmp[int(i-1)].x = src[i];
    }
    qStableSort(tmp.begin(),tmp.end(),RealGreater<T>);
    for (int i=0;i<tmp.size();i++) {
      dest[i+1] = tmp[i].x;
      dest_index[i+1] = tmp[i].n;
    }
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T> & dest_real,
			  BasicArray<T> & dest_imag,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryComplex<T> > tmp(int(src_real.length()));
    for (index_t i=1;i<=src_real.length();i++) {
      tmp[int(i-1)].n = i; 
      tmp[int(i-1)].x = src_real[i];
      tmp[int(i-1)].y = src_imag[i];
    }
    qStableSort(tmp.begin(),tmp.end(),ComplexGreater<T>);
    for (int i=0;i<tmp.size();i++) {
      dest_real[index_t(i+1)] = tmp[i].x;
      dest_imag[index_t(i+1)] = tmp[i].y;
      dest_index[index_t(i+1)] = tmp[i].n;
    }
  }
};

//!
//@Module SORT Sort 
//@@Section ARRAY
//@@Usage
//Sorts an n-dimensional array along the specified dimensional.  The first
//form sorts the array along the first non-singular dimension.
//@[
//  B = sort(A)
//@]
//Alternately, the dimension along which to sort can be explicitly specified
//@[
//  B = sort(A,dim)
//@]
//FreeMat does not support vector arguments for @|dim| - if you need @|A| to be
//sorted along multiple dimensions (i.e., row first, then columns), make multiple
//calls to @|sort|.  Also, the direction of the sort can be specified using the 
//@|mode| argument
//@[
//  B = sort(A,dim,mode)
//@]
//where @|mode = 'ascend'| means to sort the data in ascending order (the default),
//and @|mode = 'descend'| means to sort the data into descending order.  
//
//When two outputs are requested from @|sort|, the indexes are also returned.
//Thus, for 
//@[
//  [B,IX] = sort(A)
//  [B,IX] = sort(A,dim)
//  [B,IX] = sort(A,dim,mode)
//@]
//an array @|IX| of the same size as @|A|, where @|IX| records the indices of @|A|
//(along the sorting dimension) corresponding to the output array @|B|. 
//
//Two additional issues worth noting.  First, a cell array can be sorted if each 
//cell contains a @|string|, in which case the strings are sorted by lexical order.
//The second issue is that FreeMat uses the same method as MATLAB to sort complex
//numbers.  In particular, a complex number @|a| is less than another complex
//number @|b| if @|abs(a) < abs(b)|.  If the magnitudes are the same then we 
//test the angle of @|a|, i.e. @|angle(a) < angle(b)|, where @|angle(a)| is the
//phase of @|a| between @|-pi,pi|.
//@@Example
//Here are some examples of sorting on numerical arrays.
//@<
//A = int32(10*rand(4,3))
//[B,IX] = sort(A)
//[B,IX] = sort(A,2)
//[B,IX] = sort(A,1,'descend')
//@>
//Here we sort a cell array of strings.
//@<
//a = {'hello','abba','goodbye','jockey','cake'}
//b = sort(a)
//@>
//@@Tests
//@{ test_sort.m
//function x = test_sort
//  x = sort(1);
//@}
//@@Signature
//function sort SortFunction
//inputs A dim mode
//outputs B Index
//!
ArrayVector SortFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("sort requires at least one argument");
  Array input(arg[0]);
  if (input.isScalar()) {
    ArrayVector ret;
    ret.push_back(input);
    ret.push_back(Array(double(1)));
    return ret;
  }
  int workDim = 0;
  if ((arg.size() >= 2) && (!arg[1].isEmpty()))
    workDim = arg[1].asInteger();
  else
    workDim = input.dimensions().firstNonsingular();
  bool ascendSort = true;
  if (arg.size() >= 3) {
    QString tdir(arg[2].asString().toLower());
    if (tdir[0] == 'a') 
      ascendSort = true;
    else if (tdir[0] == 'd')
      ascendSort = false;
    else
      throw Exception("String argument must be either 'ascend' or 'descend'");
  }
  if (IsCellStringArray(input)) {
    Array indx;
    Array F;
    if (ascendSort)
      F = BiVectorOp<Array,OpVecSortAscend>(input.constReal<Array>(),
					    int(input.dimensions()[workDim]),
					    workDim,indx);
    else
      F = BiVectorOp<Array,OpVecSortDescend>(input.constReal<Array>(),
					     int(input.dimensions()[workDim]),
					     workDim,indx);
    ArrayVector ret;
    ret.push_back(F);
    ret.push_back(indx);
    return ret;
  } else {
    if (ascendSort)
      return BiVectorOp<OpVecSortAscend>(input,
					 int(input.dimensions()[workDim]),
					 workDim);
    else
      return BiVectorOp<OpVecSortDescend>(input,
					  int(input.dimensions()[workDim]),
					  workDim);
  }
}