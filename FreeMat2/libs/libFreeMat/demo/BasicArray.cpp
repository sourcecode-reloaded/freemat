#include "BasicArray.hpp"
#include "Variant.hpp"
#include "VariantList.hpp"

template <typename T>
void BasicArray<T>::vectorResize(index_t len) {
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

template 
bool AllZeros<double>(const BasicArray<double>&);

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
BasicArray<T> BasicArray<T>::getVectorSubset(const Variant& index) const {
  if (index.isEmpty())
    return BasicArray<T>(index.dimensions());
  if (IsColonOp(index)) {
    BasicArray<T> retvec(*this);
    retvec.reshape(NTuple(length(),1));
    return retvec;
  }
  if (isEmpty())
    throw Exception("Cannot index into empty variable,");
  BasicArray<index_t> ndx(IndexTypeFromVariant(index,length()));
  NTuple retdims;
  if (isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  BasicArray<T> retvec(retdims);
  for (int i=1;i<=retvec.length();i++)
    retvec[i] = get(ndx.get(i));
  return retvec;
}

// Check for the case of (:,4,2,5)
static bool isSliceIndexCase(const VariantList& index) {
  if (IsColonOp(index[0])) {
    bool allScalars = true;
    for (int i=1;i<index.size();i++)
      allScalars = allScalars && index[i].isScalar();
    return allScalars;
  }
  return false;
}

template <typename T>
void BasicArray<T>::setVectorSubset(const Variant& index,
				    const T& data) {
  if (index.isEmpty()) return;
  if (IsColonOp(index)) {
    for (int i=1;i<=length();i++) set(i,data);
    return;
  } 
  BasicArray<index_t> ndx(IndexTypeFromVariant(index,length()));
  if (ndx.isEmpty()) return;
  index_t max_ndx = MaxValue(ndx);
  if (max_ndx > length()) vectorResize(max_ndx);
  for (int i=1;i<=ndx.length();i++)
    set(ndx[i],data);
}

template <typename T>
void BasicArray<T>::setNDimSubset(const VariantList& index,
				  const BasicArray<T>& data) {
  if (index.empty()) return;
  if (data.isEmpty()) {
    deleteNDimSubset(index);
    return;
  }
  if (isSliceIndexCase(index)) {
    setSlice(index,data);
    return;
  }
  FastList<BasicArray<index_t> > ndx;
  NTuple secdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(IndexTypeFromVariant(index[i],dimensions()[i]));
    secdims[i] = ndx[i].length();
  }
  if (secdims.count() != data.length())
    throw Exception("mismatch in size for assignment A(I1,I2,...) = B");
  NTuple pos(1,1);
  int j=1;
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i][pos[i]];
    set(qp,data.get(j++));
    secdims.increment(pos);
  }
}

template <typename T>
void BasicArray<T>::setSlice(const VariantList& index,
			     const T& data) {
  index_t offset = getSliceIndex(dimensions(),index);
  for (int i=1;i<=dimensions()[0];i++)
    set(offset+i,data);
}

template <typename T>
void BasicArray<T>::setSlice(const VariantList& index,
			     const BasicArray<T>& data) {
  index_t offset = getSliceIndex(dimensions(),index);
  for (int i=1;i<=dimensions()[0];i++)
    set(offset+i,data[i+1]);
}

static inline bool AllDoubleScalars(const VariantList& index) {
  for (int i=0;i<index.size();i++)
    if (index[i].type() != DoubleScalar) return false;
  return true;
}

// #1, The Larch
template <typename T>
void BasicArray<T>::setNDimSubset(const VariantList& index,
				  const T& data) {
  if (AllDoubleScalars(index)) {
    NTuple pos;
    for (int i=0;i<index.size();i++)
      pos[i] = (index_t) index[i].realScalar<double>();
    set(pos,data);
    return;
  }
  if (index.empty()) return;
  if (isSliceIndexCase(index)) {
    setSlice(index,data);
    return;
  }
  FastList<BasicArray<index_t> > ndx;
  NTuple secdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(IndexTypeFromVariant(index[i],dimensions()[i]));
    secdims[i] = ndx[i].length();
  }
  NTuple pos(1,1);
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i][pos[i]];
    set(qp,data);
    secdims.increment(pos);
  }
}

BasicArray<bool> GetDeletionMap(const BasicArray<index_t>& vec,
				index_t length) {
  BasicArray<bool> retvec(NTuple(length,1));
  for (int i=1;i<=vec.length();i++)
    retvec[vec[i]] = true;
  return retvec;
}

BasicArray<bool> GetDeletionMapFromVariant(const Variant& ndx,
					   index_t length) {
  BasicArray<index_t> ndx_i(IndexTypeFromVariant(ndx,length));
  BasicArray<bool> map(GetDeletionMap(ndx_i,length));
  return map;
}

static inline bool DimensionCovered(const Variant& ndx, index_t length) {
  return AllTrue(GetDeletionMapFromVariant(ndx,length));
}

template <typename T>
void BasicArray<T>::deleteNDimSubset(const VariantList& index) {
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
  BasicArray<bool> map(GetDeletionMapFromVariant(index[first_uncovered],dimLen));
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
    for (int i=1;i<=dimLen;i++) {
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
void BasicArray<T>::deleteVectorSubset(const Variant& index) {
  if (IsColonOp(index)) {
    m_data.clear();
    m_dims = NTuple(0,0);
    m_offset = 0;
    return;
  }
  BasicArray<bool> map(GetDeletionMapFromVariant(index,length()));
  index_t newSize = 0;
  for (int i=1;i<=map.length();i++)
    if (!map[i]) newSize++;
  if (newSize == length()) return;
  NTuple newDim;
  if (isRowVector())
    newDim = NTuple(1,newSize);
  else
    newDim = NTuple(newSize,1);
  QVector<T> rdata(newSize);
  int j=0;
  for (int i=1;i<=map.length();i++) 
    if (!map[i]) rdata[j++] = get(i);
  m_data = rdata;
  m_offset = 0;
  m_dims = newDim;
}

template <typename T>
void BasicArray<T>::setVectorSubset(const Variant& index, 
				    const BasicArray<T>& data) {
  if (index.isEmpty()) return;
  if (data.isEmpty()) {
    deleteVectorSubset(index);
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
  BasicArray<index_t> ndx(IndexTypeFromVariant(index,length()));
  if (ndx.isEmpty()) return;
  index_t max_ndx = MaxValue(ndx);
  if (max_ndx > length()) vectorResize(max_ndx);
  if (ndx.length() != data.length()) 
    throw Exception("Assignment A(I) = B requires I and B to be the same size");
  for (int i=1;i<=ndx.length();i++)
    set(ndx.get(i),data.get(i));
}


static index_t getSliceIndex(const NTuple &dimensions,
			     const VariantList& index) {
  NTuple dim;
  dim[0] = 1;
  for (int i=1;i<index.size();i++)
    dim[i] = IndexTypeFromVariantScalar(index[i]);
  index_t offset = dimensions.map(dim)-1;
  return offset;
}

// A slice is a vector subset of a current array, obtainable by 
// simply offsetting into the array.
template <typename T>
BasicArray<T> BasicArray<T>::getSlice(const VariantList& index) const {
  index_t offset = getSliceIndex(dimensions(),index);
  BasicArray<T> retvec;
  retvec.m_dims = NTuple(dimensions()[0],1);
  retvec.m_offset = offset;
  retvec.m_data = m_data;
  return retvec;
}

template <typename T>
BasicArray<T> BasicArray<T>::getNDimSubset(const VariantList& index) const {
  if (index.empty()) return BasicArray<T>();
  if (isSliceIndexCase(index)) return getSlice(index);
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
BasicArray<double> BasicArray<double>::getVectorSubset(const Variant&) const;

template
BasicArray<double> BasicArray<double>::getNDimSubset(const VariantList&) const;

template
void BasicArray<double>::setVectorSubset(const Variant&, 
					 const BasicArray<double>&);

template
void BasicArray<double>::setVectorSubset(const Variant&, const double&);

template
void BasicArray<double>::setNDimSubset(const VariantList&, 
				       const BasicArray<double>&);

template
void BasicArray<double>::setNDimSubset(const VariantList&, const double&);

				       
