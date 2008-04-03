#include "BasicArray.hpp"
#include "Variant.hpp"

static bool IsColonOp(const IndexArray& ndx) {
  if (!ndx.isScalar()) return false;
  return (ndx.get(1) == -1);
}

static IndexArray ExpandColons(const IndexArray& ndx, index_t len) {
  if (!IsColonOp(ndx)) return ndx;
  IndexArray retvec(NTuple(len,1));
  for (int i=0;i<len;i++) 
    retvec.set(i+1,i+1);
  return retvec;
}

template <typename T>
void BasicArray<T>::resize(index_t len) {
  if (len > length()) {
    NTuple newDim;
    if (isEmpty() || isScalar()) {
      newDim = NTuple(1,len);
    } else if (isVector()) {
      if (rows() != 1)
	newDim = NTuple(len,1);
      else
	newDim = NTuple(1,len);
    } else {
      m_dims = NTuple(1,length());
      newDim = NTuple(1,len);
    }
    resize(newDim);
  }
}

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
T MaxValue(const BasicArray<T>& arg) {
  if (arg.isEmpty()) 
    throw Exception("Cannot use this max function on empty arrays.");
  T retval = arg[1];
  for (int i=2;i<=arg.length();i++)
    if (retval < arg[i])
      retval = arg[i];
  return retval;
}

template <typename T>
bool AllZeros(const BasicArray<T>& arg) {
  for (int i=1;i<=arg.length();i++)
    if (arg.get(i) != T(0)) return false;
  return true;
}

bool AllTrue(const BasicArray<bool>& arg) {
  for (int i=1;i<=arg.length();i++)
    if (!arg.get(i)) return false;
  return true;
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

template <typename T>
BasicArray<T> BasicArray<T>::get(const IndexArray& index) const {
  if (index.isEmpty())
    return BasicArray<T>(index.dimensions());
  if (IsColonOp(index)) {
    BasicArray<T> retvec(*this);
    retvec.reshape(NTuple(length(),1));
    return retvec;
  }
  if (isEmpty())
    throw Exception("Cannot index into empty variable,");
  NTuple retdims;
  if (isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  BasicArray<T> retvec(retdims);
  for (int i=1;i<=retvec.length();i++)
    retvec[i] = get(index.get(i));
  return retvec;
}

// Check for the case of (:,4,2,5)
static bool isSliceIndexCase(const IndexArrayList& index) {
  if (IsColonOp(index[0])) {
    bool allScalars = true;
    for (int i=1;i<index.size();i++)
      allScalars = allScalars && index[i].isScalar();
    return allScalars;
  }
  return false;
}

template <typename T>
void BasicArray<T>::set(const IndexArray& index,
				    const T& data) {
  if (index.isEmpty()) return;
  if (IsColonOp(index)) {
    for (int i=1;i<=length();i++) set(i,data);
    return;
  } 
  index_t max_ndx = MaxValue(index);
  if (max_ndx > length()) resize(max_ndx);
  for (int i=1;i<=index.length();i++)
    set(index[i],data);
}

template <typename T>
void BasicArray<T>::set(const IndexArrayList& index,
				  const BasicArray<T>& data) {
  if (index.empty()) return;
  if (data.isEmpty()) {
    del(index);
    return;
  }
  if (isSliceIndexCase(index)) {
    setSlice(index,data);
    return;
  }
  IndexArrayList ndx;
  NTuple secdims;
  NTuple maxsze;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],dimensions()[i]));
    ndx.push_back(index[i]);
    secdims[i] = ndx[i].length();
    maxsze[i] = MaxValue(ndx[i]);
  }
  if (secdims.count() != data.length())
    throw Exception("mismatch in size for assignment A(I1,I2,...) = B");
  if (dimensions() <= maxsze)
    resize(maxsze);
  NTuple pos(1,1);
  index_t j=1;
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i][pos[i]];
    set(qp,data.get(j++));
    secdims.increment(pos);
  }
}

template <typename T>
void BasicArray<T>::setSlice(const IndexArrayList& index,
			     const T& data) {
  index_t offset = getSliceIndex(dimensions(),index);
  for (index_t i=1;i<=dimensions()[0];i++)
    set(offset+i,data);
}

template <typename T>
void BasicArray<T>::setSlice(const IndexArrayList& index,
			     const BasicArray<T>& data) {
  index_t offset = getSliceIndex(dimensions(),index);
  for (index_t i=1;i<=dimensions()[0];i++)
    set(offset+i,data.get(i));
}

// if (AllDoubleScalars(index)) {
//   NTuple pos;
//   for (int i=0;i<index.size();i++)
//     pos[i] = (index_t) index[i].realScalar<double>();
//   set(pos,data);
//   return;
// }


// #1, The Larch
template <typename T>
void BasicArray<T>::set(const IndexArrayList& index,
				  const T& data) {
  if (index.empty()) return;
  if (isSliceIndexCase(index)) {
    setSlice(index,data);
    return;
  }
  IndexArrayList ndx;
  NTuple secdims;
  NTuple maxsze;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],dimensions()[i]));
    secdims[i] = ndx[i].length();
    maxsze[i] = MaxValue(ndx[i]);
  }
  if (dimensions() <= maxsze)
    resize(maxsze);
  NTuple pos(1,1);
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i][pos[i]];
    set(qp,data);
    secdims.increment(pos);
  }
}

BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length) {
  BasicArray<bool> retvec(NTuple(length,1));
  for (index_t i=1;i<=vec.length();i++)
    retvec[vec[i]] = true;
  return retvec;
}

static inline bool DimensionCovered(const IndexArray& ndx, index_t length) {
  return AllTrue(GetDeletionMap(ndx,length));
}

template <typename T>
void BasicArray<T>::del(const IndexArrayList& index) {
  // The strategy for dealing with deletions is simplified relative
  // to 3.x code.  An NDim deletion is only valid if there is one
  // dimension that is not covered.
  QVector<bool> covered(index.size());
  for (int i=0;i<index.size();i++) {
    index_t dimLen = dimensions()[i];
    covered[i] = (IsColonOp(index[i]) || DimensionCovered(index[i],dimLen));
  }
  // Count the number of uncovered dimensions
  int uncovered_count = 0;
  int first_uncovered = 0;
  for (int i=0;i<index.size();i++)
    if (!covered[i]) {
      first_uncovered = i;
      uncovered_count++;
    }
  if (uncovered_count == 0) {
    m_data.clear();
    m_dims = NTuple(0,0);
    m_offset = 0;
    return;
  }
  if (uncovered_count > 1)
    throw Exception("Deletion A(:,...,:) = [] cannot have more than one non-singular dimension");
  index_t dimLen = dimensions()[first_uncovered];
  BasicArray<bool> map(GetDeletionMap(index[first_uncovered],dimLen));
  int newSize = 0;
  for (int i=1;i<=map.length();i++)
    if (!map.get(i)) newSize++;
  // Create an output array..
  NTuple outdims(dimensions());
  outdims[first_uncovered] = newSize;
  BasicArray<T> retvec(outdims);
  ConstBasicIterator<T> source(this,first_uncovered);
  BasicIterator<T> dest(&retvec,first_uncovered);
  while (source.isValid() && dest.isValid()) {
    for (index_t i=1;i<=dimLen;i++) {
      if (!map.get(i)) {
	dest.set(source.get());
	dest.next();
      }
      source.next();
    }
    source.nextSlice();
    dest.nextSlice();
  }
  m_data = retvec.m_data;
  m_offset = retvec.m_offset;
  m_dims = retvec.m_dims;
}

template <typename T>
void BasicArray<T>::del(const IndexArray& index) {
  if (IsColonOp(index)) {
    m_data.clear();
    m_dims = NTuple(0,0);
    m_offset = 0;
    return;
  }
  BasicArray<bool> map(GetDeletionMap(index,length()));
  index_t newSize = 0;
  for (int i=1;i<=map.length();i++)
    if (!map[i]) newSize++;
  if (newSize == length()) return;
  NTuple newDim;
  if (isRowVector())
    newDim = NTuple(1,newSize);
  else
    newDim = NTuple(newSize,1);
  QVector<T> rdata((int64)newSize);
  int j=0;
  for (index_t i=1;i<=map.length();i++) 
    if (!map[i]) rdata[j++] = get(i);
  m_data = rdata;
  m_offset = 0;
  m_dims = newDim;
}

template <typename T>
void BasicArray<T>::set(const IndexArray& index, 
			const BasicArray<T>& data) {
  if (index.isEmpty()) return;
  if (data.isEmpty()) {
    del(index);
    return;
  }
  if (IsColonOp(index)) {
    NTuple myDims;
    if (!isEmpty())
      myDims = dimensions();
    else
      myDims = data.dimensions();
    if (myDims.count() != data.length()) 
      throw Exception("assignment A(:) = B requires A and B to be the same size");
    m_data = data.m_data;
    m_dims = myDims;
    m_offset = data.m_offset;
    return;
  }
  index_t max_ndx = MaxValue(index);
  if (max_ndx > length()) resize(max_ndx);
  if (index.length() != data.length()) 
    throw Exception("Assignment A(I) = B requires I and B to be the same size");
  for (index_t i=1;i<=index.length();i++)
    set(index.get(i),data.get(i));
}


template <typename T>
bool BasicArray<T>::operator==(const BasicArray<T> &b) const {
  for (index_t i=1;i<b.length();i++) 
    if (get(i) != b.get(i)) return false;
  return true;
}

static index_t getSliceIndex(const NTuple &dimensions,
			     const IndexArrayList& index) {
  NTuple dim;
  dim[0] = 1;
  for (int i=1;i<index.size();i++)
    dim[i] = index[i][1];
  index_t offset = dimensions.map(dim)-1;
  return offset;
}

// A slice is a vector subset of a current array, obtainable by 
// simply offsetting into the array.
template <typename T>
BasicArray<T> BasicArray<T>::getSlice(const IndexArrayList& index) const {
  index_t offset = getSliceIndex(dimensions(),index);
  BasicArray<T> retvec;
  retvec.m_dims = NTuple(dimensions()[0],1);
  retvec.m_offset = offset;
  retvec.m_data = m_data;
  return retvec;
}

template <typename T>
BasicArray<T> BasicArray<T>::get(const IndexArrayList& index) const {
  if (index.empty()) return BasicArray<T>();
  if (isSliceIndexCase(index)) return getSlice(index);
  IndexArrayList ndx;
  NTuple outdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],dimensions()[i]));
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

template BasicArray<bool>   BasicArray<bool>::get(const IndexArray&) const;
template BasicArray<uint8>  BasicArray<uint8>::get(const IndexArray&) const;
template BasicArray<int8>   BasicArray<int8>::get(const IndexArray&) const;
template BasicArray<uint16> BasicArray<uint16>::get(const IndexArray&) const;
template BasicArray<int16>  BasicArray<int16>::get(const IndexArray&) const;
template BasicArray<uint32> BasicArray<uint32>::get(const IndexArray&) const;
template BasicArray<int32>  BasicArray<int32>::get(const IndexArray&) const;
template BasicArray<uint64> BasicArray<uint64>::get(const IndexArray&) const;
template BasicArray<int64>  BasicArray<int64>::get(const IndexArray&) const;
template BasicArray<float>  BasicArray<float>::get(const IndexArray&) const;
template BasicArray<double> BasicArray<double>::get(const IndexArray&) const;
template BasicArray<Variant> BasicArray<Variant>::get(const IndexArray&) const;

template BasicArray<bool>   BasicArray<bool>::get(const IndexArrayList&) const;
template BasicArray<uint8>  BasicArray<uint8>::get(const IndexArrayList&) const;
template BasicArray<int8>   BasicArray<int8>::get(const IndexArrayList&) const;
template BasicArray<uint16> BasicArray<uint16>::get(const IndexArrayList&) const;
template BasicArray<int16>  BasicArray<int16>::get(const IndexArrayList&) const;
template BasicArray<uint32> BasicArray<uint32>::get(const IndexArrayList&) const;
template BasicArray<int32>  BasicArray<int32>::get(const IndexArrayList&) const;
template BasicArray<uint64> BasicArray<uint64>::get(const IndexArrayList&) const;
template BasicArray<int64>  BasicArray<int64>::get(const IndexArrayList&) const;
template BasicArray<float>  BasicArray<float>::get(const IndexArrayList&) const;
template BasicArray<double> BasicArray<double>::get(const IndexArrayList&) const;
template BasicArray<Variant> BasicArray<Variant>::get(const IndexArrayList&) const;

template void BasicArray<bool>::set(const IndexArray&,const BasicArray<bool>&);
template void BasicArray<uint8>::set(const IndexArray&,const BasicArray<uint8>&);
template void BasicArray<int8>::set(const IndexArray&,const BasicArray<int8>&);
template void BasicArray<uint16>::set(const IndexArray&,const BasicArray<uint16>&);
template void BasicArray<int16>::set(const IndexArray&,const BasicArray<int16>&);
template void BasicArray<uint32>::set(const IndexArray&,const BasicArray<uint32>&);
template void BasicArray<int32>::set(const IndexArray&,const BasicArray<int32>&);
template void BasicArray<uint64>::set(const IndexArray&,const BasicArray<uint64>&);
template void BasicArray<int64>::set(const IndexArray&,const BasicArray<int64>&);
template void BasicArray<float>::set(const IndexArray&,const BasicArray<float>&);
template void BasicArray<double>::set(const IndexArray&,const BasicArray<double>&);
template void BasicArray<Variant>::set(const IndexArray&,const BasicArray<Variant>&);

template void BasicArray<bool>::set(const IndexArray&, const bool&);
template void BasicArray<uint8>::set(const IndexArray&, const uint8&);
template void BasicArray<int8>::set(const IndexArray&, const int8&);
template void BasicArray<uint16>::set(const IndexArray&, const uint16&);
template void BasicArray<int16>::set(const IndexArray&, const int16&);
template void BasicArray<uint32>::set(const IndexArray&, const uint32&);
template void BasicArray<int32>::set(const IndexArray&, const int32&);
template void BasicArray<uint64>::set(const IndexArray&, const uint64&);
template void BasicArray<int64>::set(const IndexArray&, const int64&);
template void BasicArray<float>::set(const IndexArray&, const float&);
template void BasicArray<double>::set(const IndexArray&, const double&);
template void BasicArray<Variant>::set(const IndexArray&, const Variant&);

template void BasicArray<bool>::set(const IndexArrayList&,const BasicArray<bool>&);
template void BasicArray<uint8>::set(const IndexArrayList&,const BasicArray<uint8>&);
template void BasicArray<int8>::set(const IndexArrayList&,const BasicArray<int8>&);
template void BasicArray<uint16>::set(const IndexArrayList&,const BasicArray<uint16>&);
template void BasicArray<int16>::set(const IndexArrayList&,const BasicArray<int16>&);
template void BasicArray<uint32>::set(const IndexArrayList&,const BasicArray<uint32>&);
template void BasicArray<int32>::set(const IndexArrayList&,const BasicArray<int32>&);
template void BasicArray<uint64>::set(const IndexArrayList&,const BasicArray<uint64>&);
template void BasicArray<int64>::set(const IndexArrayList&,const BasicArray<int64>&);
template void BasicArray<float>::set(const IndexArrayList&,const BasicArray<float>&);
template void BasicArray<double>::set(const IndexArrayList&,const BasicArray<double>&);
template void BasicArray<Variant>::set(const IndexArrayList&,const BasicArray<Variant>&);

template void BasicArray<bool>::set(const IndexArrayList&, const bool&);
template void BasicArray<uint8>::set(const IndexArrayList&, const uint8&);
template void BasicArray<int8>::set(const IndexArrayList&, const int8&);
template void BasicArray<uint16>::set(const IndexArrayList&, const uint16&);
template void BasicArray<int16>::set(const IndexArrayList&, const int16&);
template void BasicArray<uint32>::set(const IndexArrayList&, const uint32&);
template void BasicArray<int32>::set(const IndexArrayList&, const int32&);
template void BasicArray<uint64>::set(const IndexArrayList&, const uint64&);
template void BasicArray<int64>::set(const IndexArrayList&, const int64&);
template void BasicArray<float>::set(const IndexArrayList&, const float&);
template void BasicArray<double>::set(const IndexArrayList&, const double&);
template void BasicArray<Variant>::set(const IndexArrayList&, const Variant&);

template void BasicArray<bool>::set(const NTuple&, const bool&);
template void BasicArray<uint8>::set(const NTuple&, const uint8&);
template void BasicArray<int8>::set(const NTuple&, const int8&);
template void BasicArray<uint16>::set(const NTuple&, const uint16&);
template void BasicArray<int16>::set(const NTuple&, const int16&);
template void BasicArray<uint32>::set(const NTuple&, const uint32&);
template void BasicArray<int32>::set(const NTuple&, const int32&);
template void BasicArray<uint64>::set(const NTuple&, const uint64&);
template void BasicArray<int64>::set(const NTuple&, const int64&);
template void BasicArray<float>::set(const NTuple&, const float&);
template void BasicArray<double>::set(const NTuple&, const double&);
template void BasicArray<Variant>::set(const NTuple&, const Variant&);

template bool BasicArray<bool>::operator==(const BasicArray<bool>&) const;
template bool BasicArray<uint8>::operator==(const BasicArray<uint8>&) const;
template bool BasicArray<int8>::operator==(const BasicArray<int8>&) const;
template bool BasicArray<uint16>::operator==(const BasicArray<uint16>&) const;
template bool BasicArray<int16>::operator==(const BasicArray<int16>&) const;
template bool BasicArray<uint32>::operator==(const BasicArray<uint32>&) const;
template bool BasicArray<int32>::operator==(const BasicArray<int32>&) const;
template bool BasicArray<uint64>::operator==(const BasicArray<uint64>&) const;
template bool BasicArray<int64>::operator==(const BasicArray<int64>&) const;
template bool BasicArray<float>::operator==(const BasicArray<float>&) const;
template bool BasicArray<double>::operator==(const BasicArray<double>&) const;
template bool BasicArray<Variant>::operator==(const BasicArray<Variant>&) const;

template bool AllZeros(const BasicArray<bool>&);
template bool AllZeros(const BasicArray<uint8>&);
template bool AllZeros(const BasicArray<int8>&);
template bool AllZeros(const BasicArray<uint16>&);
template bool AllZeros(const BasicArray<int16>&);
template bool AllZeros(const BasicArray<uint32>&);
template bool AllZeros(const BasicArray<int32>&);
template bool AllZeros(const BasicArray<uint64>&);
template bool AllZeros(const BasicArray<int64>&);
template bool AllZeros(const BasicArray<float>&);
template bool AllZeros(const BasicArray<double>&);
//template bool AllZeros(const BasicArray<Variant>&);
