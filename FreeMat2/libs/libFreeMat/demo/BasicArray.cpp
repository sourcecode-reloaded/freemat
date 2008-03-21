#include "BasicArray.hpp"
#include "Variant.hpp"
#include "VariantList.hpp"

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
    for (int j=i+1;j<arg.rows();j++) 
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

template 
bool AllZeros<double>(const BasicArray<double>&);


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

template <typename T>
BasicArray<T> BasicArray<T>::getVectorSubset(const Variant& index) {
  if (index.isEmpty())
    return BasicArray<T>(index.dimensions());
  if (IsColonOp(index)) {
    BasicArray<T> retvec(*this);
    retvec.reshape(NTuple(length(),1));
    return retvec;
  }
  if (isEmpty())
    throw Exception("Cannot index into empty variable,");
  BasicArray<index_t> ndx(IndexTypeFromVariant(index,dimensions()[0]));
  NTuple retdims;
  if (isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  BasicArray<T> retvec(retdims);
  for (int i=0;i<retvec.length();i++)
    retvec[i+1] = get(ndx[i]);
  return retvec;
}

// A slice is a vector subset of a current array, obtainable by 
// simply offsetting into the array.
template <typename T>
BasicArray<T> BasicArray<T>::getSlice(const VariantList& index) {
  // Build a dimension vector
  NTuple dim;
  dim[0] = 1;
  for (int i=1;i<index.size();i++)
    dim[i] = IndexTypeFromVariantScalar(index[i]);
  // Map it to an offset
  index_t offset = dimensions().map(dim)-1;
  BasicArray<T> retvec;
  retvec.m_dims = NTuple(dimensions()[0],1);
  retvec.m_offset = offset;
  retvec.m_data = m_data;
  return retvec;
}

template <typename T>
BasicArray<T> BasicArray<T>::getNDimSubset(const VariantList& index) {
  if (index.empty()) return BasicArray<T>();
  // Check for the case of (:,4,2,5)
  if (IsColonOp(index[0])) {
    bool allScalars = true;
    for (int i=1;i<index.size();i++)
      allScalars = allScalars && index[i].isScalar();
    if (allScalars)
      return getSlice(index);
  }
  FastList<BasicArray<index_t> > ndx;
  NTuple outdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(IndexTypeFromVariant(index[i],dimensions()[i]));
    outdims[i] = ndx[i].length();
  }
  BasicArray<T> retval(outdims);
  // Suppose we have A(1:3,2:5) -- 
  NTuple pos(1,1);
  while (pos <= outdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++) 
      qp[i] = ndx[i][pos[i]];
    retval[pos] = get(qp);
    outdims.increment(pos);
  }
  return retval;
}

template
BasicArray<double> BasicArray<double>::getVectorSubset(const Variant&);

template
BasicArray<double> BasicArray<double>::getNDimSubset(const VariantList&);
