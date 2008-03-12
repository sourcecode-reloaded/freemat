#include "BasicArray.hpp"

template <typename T>
void BasicArray<T>::resize(const NTuple& pos) {
  BasicArray<T> retval(pos);
  Transformer<BasicArray<T>,T> transform(&retval,this);
  while (transform.isValid()) {
    for (int i=0;i<transform.size();i++) {
      transform.set(transform.get());
      transform.next();
    }
    transform.nextSlice();
  }
  m_data = retval.m_data;
  m_dims = retval.m_dims;
  m_offset = 0;
}

template <typename T>
BasicArray<T> Transpose(const BasicArray<T>& arg) {
  if (!arg.dimensions().is2D())
    throw Exception("Cannot transpose multi-dimensional arrays");
  BasicArray<T> retval(NTuple(arg.cols(),arg.rows()));
  Transformer<BasicArray<T>,T> transform(&retval,&arg,0,1);
  while (transform.isValid()) {
    for (int i=0;i<transform.size();i++) {
      transform.set(transform.get());
      transform.next();
    }
    transform.nextSlice();
  }
  return retval;
}

template <typename T>
bool IsPositive(const BasicArray<T>& arg) {
  for (int i=0;i<arg.length();i++) 
    if (arg.get(i) < 0) return false;
  return true;
}

template <typename T>
bool IsSymmetric(const BasicArray<T>& arg) {
  if (!arg.dimensions().is2D()) 
    throw Exception("Symmetry check not valid for N-dimensional arrays");
  for (int i=0;i<arg.cols();i++) 
    for (int j=0;j<i;j++) 
      if (arg[NTuple(i,j)] != arg[NTuple(j,i)])
	return false;
  return true;
}

template <typename T>
bool AnyNotFinite(const BasicArray<T>& arg) {
  for (int i=0;i<arg.length();i++) 
    if (!IsFinite(arg.get(i))) return true;
  return false;
}

template <typename T>
bool AllZeros(const BasicArray<T>& arg) {
  for (int i=0;i<arg.length();i++)
    if (arg.get(i) != T(0)) return false;
  return true;
}

template <typename T>
BasicArray<T> BasicArray<T>::getVectorSubset(const Variant& index) {
  if (index.isEmpty())
    return BasicArray<T>(index.dimensions());
  if (IsColonOp(index) && isEmpty())
    return *this;
  if (isEmpty())
    throw Exception("Cannot index into empty variable,");
  if (IsColonOp(index)) {
    BasicArray<T> retvec(*this);
    retvec.reshape(length(),1);
    return retvec;
  }
  Variant ndx(IndexTypeFromVariant(index,dimensions()));
  NTuple retdims;
  if (isColumnVector() && index.isRowVector())
    retdims = NTuple(index.getLength(),1);
  else
    retdims = index.dimensions();
  BasicArray<T> retvec(retdims);
  for (int i=0;i<retvec.length();i++)
    retvec[i+1] = get(ndx[i]);
  return retvec;
}

template <typename T>
BasicArray<T> BasicArray<T>::getNDimSubset(const VariantList& index) {
  VariantList ndx;
  NTuple outdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(IndexTypeFromVariant(index,dimensions(),i));
    outdims[i] = ndx[i].length();
  }
  BasicArray<T> retval(outdims);
  // Suppose we have A(1:3,2:5) -- 
  NTuple pos(1,1);
  while (pos <= outdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++) 
      qp[i] = ndx[i][pos[i]];
  }
}

template <typename T>
BasicArray<T> Apply(const BasicArray<T>& arg, T (*func)(T)) {
  BasicArray<T> retval(arg.dimensions());
  Transformer<BasicArray<T>,T> transform(&retval,&arg);
  while (transform.isValid()) {
    for (int i=0;i<transform.size();i++) {
      transform.set(func(transform.get()));
      transform.next();
    }
    transform.nextSlice();
  }
  return retval;
}
