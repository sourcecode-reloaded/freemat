#ifndef __Algorithms_hpp__
#define __Algorithms_hpp__

#include "Array.hpp"

void WarningMessage(QString msg);

bool IsColonOp(const Array &arg);
bool IsNonNegative(const Array &arg);
bool IsInteger(const Array &arg);
bool IsUnsigned(const Array &arg);

const IndexArray IndexArrayFromArray(const Array &index);

const ArrayVector ArrayVectorFromCellArray(const Array &arg);
Array ArrayFromCellArray(const Array &arg);
const Array CellArrayFromArrayVector(ArrayVector &arg, index_t cnt);
const Array CellArrayFromArray(const Array &arg);
const Array CellArrayFromStringVector(const StringVector& arg);
const Array StringArrayFromStringVector(const StringVector& arg);
StringVector StringVectorFromArray(const Array &arg);

void SetCellContents(Array &cell, const Array& index, ArrayVector& data);
void SetCellContents(Array &cell, const ArrayVector& index, ArrayVector& data);
QStringList FieldNames(const Array& arg);

bool IsCellStringArray(const Array &arg);

Array Vectorize(const Array& arg);

template <typename T>
BasicArray<T> ToBasicArray(const QVector<T> &x) {
  BasicArray<T> retvec(NTuple(index_t(x.size()),1));
  for (int i=0;i<x.size();i++)
    retvec[index_t(i)+1] = x[i];
  return retvec;
}

template <typename T>
SparseMatrix<T> ToRealSparse(const Array& data) {
  if (data.type().Sparse == 1) return data.constRealSparse<T>();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = data.asDenseArray();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  return SparseMatrix<T>(cdata.constReal<T>());
}

template <typename T>
SparseMatrix<T> ToImagSparse(const Array& data) {
  if (data.type().Sparse == 1) return data.constImagSparse<T>();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = cdata.asDenseArray();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  return SparseMatrix<T>(cdata.constImag<T>());
}

template <typename T>
Array DiagonalArray(const BasicArray<T> &in) {
  BasicArray<T> retmat(NTuple(in.length(),in.length()));
  for (index_t i=1;i<=in.length();i++)
    retmat.set(NTuple(i,i),in.get(i));
  return Array(retmat);
}

template <typename T>
Array DiagonalArray(const BasicArray<T> &in_r, 
		    const BasicArray<T> &in_i) {
  BasicArray<T> retmat_r(NTuple(in_r.length(),in_r.length()));
  BasicArray<T> retmat_i(NTuple(in_i.length(),in_i.length()));
  for (index_t i=1;i<=in_r.length();i++) {
    retmat_r.set(NTuple(i,i),in_r.get(i));
    retmat_i.set(NTuple(i,i),in_i.get(i));
  }
  return Array(retmat_r,retmat_i);
}

BasicArray<Array> ArrayVectorToBasicArray(const ArrayVector& a);

Array MatrixConstructor(const ArrayMatrix& data);
Array CellConstructor(const ArrayMatrix& data);
Array CellConstructor(const ArrayVector& data);
Array StructConstructor(const StringVector& fields, const ArrayVector& values);
inline Array EmptyConstructor() {return Array(Double);}

bool TestForCaseMatch(const Array &s, const Array &r);
bool RealAllZeros(const Array &t);

Type ScalarType(Type x);

Array GetDiagonal(const Array &a, int diagonal = 0);
Array DiagonalArray(const Array &f, int diagonal = 0);
bool IsSymmetric(const Array &a);
bool IsPositive(const Array &a);

Array RangeConstructor(double start, double step, double stop, bool vertical);
Array Transpose(const Array &A);
Array Hermitian(const Array &A);
Array Real(const Array &A);
Array Imag(const Array &A);
Array Permute(const Array &A, const NTuple &dp);

NTuple ConvertArrayToNTuple(const Array &A);
Array ConvertNTupleToArray(const NTuple &D);
bool AnyNotFinite(const Array &A);

Array MergeToComplex(const Array &Real, const Array &Imag);
Array MatIJVToSparse(const Array &ir, const Array &jc, const Array &pr, const Array &pi, bool complexFlag);
void SparseToIJVMat(const Array &x, Array &rows, Array &cols, Array &vals);

Array ToSparse(const Array& data);

bool Equals(const StringVector& a, const StringVector& b);
#endif
