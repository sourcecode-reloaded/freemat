#ifndef __GetSet_hpp__
#define __GetSet_hpp__

#include "IndexArray.hpp"

template <typename T>
void SetSlice(T& arg, const IndexArrayVector& index, const T& data) {
  index_t offset = getSliceIndex(arg.dimensions(),index);
  for (index_t i=1;i<=arg.dimensions()[0];i++)
    arg.set(offset+i,data.get(i));
}

template <typename T, typename S>
void SetSlice(T& arg, const IndexArrayVector& index, const S& data) {
  index_t offset = getSliceIndex(arg.dimensions(),index);
  for (index_t i=1;i<=arg.dimensions()[0];i++)
    arg.set(offset+i,data);  
}

template <typename T>
void Set(T& arg, const IndexArray& index, const T& data) {
  if (index.isEmpty()) return;
  if (data.isEmpty()) {
    arg.del(index);
    return;
  }
  if (IsColonOp(index)) {
    NTuple myDims;
    if (!arg.isEmpty())
      myDims = arg.dimensions();
    else
      myDims = data.dimensions();
    if (myDims.count() != data.length()) 
      throw Exception("assignment A(:) = B requires A and B to be the same size");
    T retvec(data);
    retvec.reshape(myDims);
    arg = retvec;
    return;
  }
  index_t max_ndx = MaxValue(index);
  if (max_ndx > arg.length()) arg.resize(max_ndx);
  if (index.length() != data.length()) 
    throw Exception("Assignment A(I) = B requires I and B to be the same size");
  for (index_t i=1;i<=index.length();i++)
    arg.set(index.get(i),data.get(i));
}


template <typename T>
void Set(T& arg, const IndexArrayVector& index, const T& data) {
  if (index.empty()) return;
  if (data.isEmpty()) {
    arg.del(index);
    return;
  }
  if (isSliceIndexCase(index)) {
    SetSlice(arg,index,data);
    return;
  }
  IndexArrayVector ndx;
  NTuple secdims;
  NTuple maxsze;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
    secdims[i] = ndx[i].length();
    maxsze[i] = MaxValue(ndx[i]);
  }
  if (secdims.count() != data.length())
    throw Exception("mismatch in size for assignment A(I1,I2,...) = B");
  if (arg.dimensions() <= maxsze)
    arg.resize(maxsze);
  NTuple pos(1,1);
  index_t j=1;
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i].get(pos[i]);
    arg.set(qp,data.get(j++));
    secdims.increment(pos);
  }
}

template <typename T, typename S>
void Set(T& arg, const IndexArrayVector& index, const S& data) {
  if (index.empty()) return;
  if (isSliceIndexCase(index)) {
    SetSlice(arg,index,data);
    return;
  }
  IndexArrayVector ndx;
  NTuple secdims;
  NTuple maxsze;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
    secdims[i] = ndx[i].length();
    maxsze[i] = MaxValue(ndx[i]);
  }
  if (arg.dimensions() <= maxsze)
    arg.resize(maxsze);
  NTuple pos(1,1);
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i].get(pos[i]);
    arg.set(qp,data);
    secdims.increment(pos);
  }
}

template <typename T, typename S>
void Set(T& arg, const IndexArray& index, const S& data) {
  if (index.isEmpty()) return;
  if (IsColonOp(index)) {
    for (index_t i=1;i<=arg.length();i++) 
      arg.set(i,data);
    return;
  } 
  index_t max_ndx = MaxValue(index);
  if (max_ndx > arg.length()) arg.resize(max_ndx);
  for (index_t i=1;i<=index.length();i++)
    arg.set(index.get(i),data);  
}

template <typename T>
const T Get(const T& arg, const IndexArray& index) {
  if (index.isEmpty()) return T(index.dimensions());
  if (IsColonOp(index)) {
    T retvec(arg);
    retvec.reshape(NTuple(arg.length(),1));
    return retvec;
  }
  if (arg.isEmpty())
    throw Exception("Cannot index into empty variable,");
  NTuple retdims;
  if (arg.isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  T retvec(retdims);
  for (index_t i=1;i<=retvec.length();i++)
    retvec.set(i,arg.get(index.get(i)));
  return retvec;
}

template <typename T>
const T Get(const T& arg, const IndexArrayVector& index) {
  if (index.empty()) return T();
  if (isSliceIndexCase(index)) return arg.slice(index);
  IndexArrayVector ndx;
  NTuple outdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
    outdims[i] = ndx[i].length();
  }
  T retval(outdims);
  NTuple pos(1,1);
  while (pos <= outdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++) 
      qp[i] = ndx[i].get(pos[i]);
    retval.set(pos,arg.get(qp));
    outdims.increment(pos);
  }
  return retval;
}

#endif
