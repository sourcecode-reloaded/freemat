#ifndef __BasicArray_hpp__
#define __BasicArray_hpp__

#include <QVector>
#include "CommonIterator.hpp"
#include "NTuple.hpp"
#include <QDebug>
#include "Types.hpp"
#include "FastList.hpp"

template <typename T>
class BasicArray;

typedef BasicArray<index_t> IndexArray;
typedef FastList<IndexArray> IndexArrayVector;
BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length);

bool IsColonOp(const IndexArray& ndx);
bool DimensionCovered(const IndexArray& ndx, index_t length);

template <typename T>
class BasicIterator;

template <typename T>
class ConstBasicIterator;

template <typename T>
class BasicArray {
  QVector<T> m_data;
  NTuple m_dims;
  index_t m_offset;
public:
  BasicArray() : m_data(), m_dims(0,0), m_offset(0) {}
  BasicArray(const NTuple& dim) : 
    m_data(), m_dims(dim), m_offset(0) {
    m_data.resize((int64)(m_dims.count()));
  }
  BasicArray(T val) {
    m_data << val; 
    m_dims = NTuple(1,1); 
    m_offset = 0;
  }
  ~BasicArray() {}
  inline index_t offset() const {return m_offset;}
  //  inline QVector<T>& data() {return m_data;}
  //inline const QVector<T>& data() const {return m_data;}
  inline const NTuple dimensions() const {return m_dims;}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t length() const {return m_dims.count();}
  inline index_t isScalar() const {return length() == 1;}
  inline bool is2D() const {return m_dims.is2D();}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T operator[](index_t pos) const {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[(int64)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](index_t pos) {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[(int64)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T* constData() const {
    return &(m_data.at(int64(m_offset)));
  }
  inline T* data() {
    return &(m_data[int64(m_offset)]);
  }
  bool operator==(const BasicArray<T>& b) const {
    for (index_t i=1;i<b.length();i++) 
      if (get(i) != b.get(i)) return false;
    return true;
  }
  inline void set(const NTuple& pos, const T& val) {
    if (dimensions() <= pos) resize(pos);
    m_data[(int64)(m_dims.map(pos)+m_offset-1)] = val;
  }
  inline void set(index_t pos, const T& val) {
    if (dimensions().count() <= pos) resize(pos);
    m_data[(int64)(pos+m_offset-1)] = val;
  }
  inline const T get(index_t pos) const {
    return m_data[(int64)(pos+m_offset-1)];
  }
  inline const T get(const NTuple& pos) const {
    return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
  }
  BasicArray<T> getSlice(const IndexArrayVector& index) const {
    index_t offset = getSliceIndex(dimensions(),index);
    BasicArray<T> retvec;
    retvec.m_dims = NTuple(dimensions()[0],1);
    retvec.m_offset = offset;
    retvec.m_data = m_data;
    return retvec;
  }
  void del(const IndexArrayVector& index) {
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
  void del(const IndexArray& index) {
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
  void resize(const NTuple& pos) {
    BasicArray<T> retval(pos);
    Transformer<BasicArray<T>,T> transform(&retval,this);
    while (transform.isValid()) {
      for (int i=0;i<transform.size();i++) {
	transform.set(transform.get());
	transform.next();
      }
      transform.nextSlice();
    }
    *this = retval;
  }
  void resize(index_t len) {
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
  void printMe(std::ostream& o) const {
    o << dimensions() << "[";
    for (int i=0;i<length();i++)
      o << get(i+1) << " ";
    o << "]\n";
  }
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const BasicArray<T>& arg) {
  arg.printMe(o);
  return o;
}

template <typename T>
BasicArray<T> MergeComplex(const BasicArray<T>& real, const BasicArray<T>& imag) {
  NTuple retdim(real.dimensions());
  if (retdim != imag.dimensions()) 
    throw Exception("Cannot merge arrays of unequal size into complex array");
  retdim[0] = retdim[0]*2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i <= real.length();i++) {
    retval.set(2*i-1,real.get(i));
    retval.set(2*i,imag.get(i));
  }
  return retval;
}

template <typename T>
BasicArray<T> SplitReal(const BasicArray<T>& A) {
  NTuple retdim(A.dimensions());
  retdim[0] = retdim[0]/2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i <= retval.length();i++) 
    retval.set(i,A.get(2*i-1));
  return retval;
}

template <typename T>
BasicArray<T> SplitImag(const BasicArray<T>& A) {
  NTuple retdim(A.dimensions());
  retdim[0] = retdim[0]/2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i <= retval.length();i++) 
    retval.set(i,A.get(2*i));
  return retval;
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
bool AllZeros(const BasicArray<T>& arg) {
  for (int i=1;i<=arg.length();i++)
    if (arg.get(i) != T(0)) return false;
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
IndexArray Find(const BasicArray<T>& vec) {
  index_t count = 0;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) count++;
  IndexArray retvec(NTuple(count,1));
  count = 1;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) {
      retvec.set(count,i);
      count++;
    }
  return retvec;
}

inline bool AllTrue(const BasicArray<bool>& arg) {
  for (int i=1;i<=arg.length();i++)
    if (!arg.get(i)) return false;
  return true;
}


template <typename S, typename T>
void ConvertBasicArray(const BasicArray<S>& source, BasicArray<T>& dest) {
  if (source.length() != dest.length())
    throw Exception("Mismatch between source and destination arrays in call to Convert BasicArray");
  for (index_t i=1;i<=source.length();i++)
    dest.set(i,(T) source.get(i));
}

template <typename S>
class BasicIterator : public BaseIterator<BasicArray<S>,S> {
public:
  BasicIterator(BasicArray<S> *ptr, int dim) : 
    BaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

template <typename S>
class ConstBasicIterator : public ConstBaseIterator<BasicArray<S>,S> {
public:
  ConstBasicIterator(const BasicArray<S> *ptr, int dim) : 
    ConstBaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

template <typename T>
class ConstSpinIterator {
  const BasicArray<T> *m_ptr;
  int m_indx;
public:
  ConstSpinIterator(const BasicArray<T> *ptr) : m_ptr(ptr), m_indx(1) {}
  inline T get() const {return m_ptr->get(m_indx);}
  inline void next() {
    m_indx++; m_indx = ((m_indx - 1) % m_ptr->length()) + 1;
  }
};

#endif
