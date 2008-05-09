#ifndef __SparseMatrix_hpp__
#define __SparseMatrix_hpp__

#include <QMap>
#include <QtGlobal>
#include <iostream>
#include "Types.hpp"
#include "NTuple.hpp"
#include "BasicArray.hpp"
#include <cmath>

template <typename T>
class SparseSlice : public QMap<index_t,T> {
};

template <typename T>
class SparseData : public QMap<index_t,SparseSlice<T> > {
};

template <typename T>
class ConstSparseIterator;

template <typename T>
class SparseMatrix {
  SparseData<T> m_data;
  NTuple m_dims;
public:
  SparseMatrix(const NTuple &dims) {
    m_dims = dims;
  }
  SparseMatrix() : m_dims(NTuple(0,0)) {}
  const SparseData<T>& constData() const {return m_data;}
  SparseData<T>& data() {return m_data;}
  SparseMatrix(QVector<index_t> row, QVector<index_t> col, QVector<T> val) {
    if (!((row.size() == col.size()) &&
	  (row.size() == val.size())))
      throw Exception("Invalid sizes for IJV style constructor");
    index_t maxcol = 0;
    index_t maxrow = 0;
    for (int i=0;i<row.size();i++) {
      m_data[col[i]][row[i]] = val[i];
      maxcol = qMax(maxcol,col[i]);
      maxrow = qMax(maxrow,row[i]);
    }
    m_dims = NTuple(maxrow,maxcol);
  }
  SparseMatrix(const BasicArray<T> &A) {
    ConstBasicIterator<T> source(&A,0);
    index_t col = 1;
    while (source.isValid()) {
      SparseSlice<T> this_col;
      for (index_t row=1;row<=source.size();row++) {
	if (source.get() != 0)
	  this_col[row] = source.get();
	source.next();
      }
      if (this_col.count() > 0)
	m_data[col] = this_col;
      ++col;
      source.nextSlice();
    }
    m_dims = A.dimensions();
  }
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline index_t length() const {return m_dims.count();}
  inline index_t isScalar() const {return length() == 1;}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t cols() const {return m_dims.cols();}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      if (!m_data.contains(pos[1]))
	return T(0);
      const SparseSlice<T>& col = m_data.value(pos[1]);
      if (!col.contains(pos[0]))
	return T(0);
      return col.value(pos[0]);
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      return m_data[pos[1]][pos[0]];
    }
    throw Exception("Out of range");
  }
  inline const T operator[](index_t pos) const {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline T& operator[](index_t pos) {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline const T get(index_t pos) const {
    return (*this)[pos];
  }
  inline const T get(const NTuple& pos) const {
    return (*this)[pos];
  }
  void erase(const NTuple& pos) {
    if (!m_data.contains(pos[1])) return;
    SparseSlice<T> &col = m_data[pos[1]];
    if (!col.contains(pos[0])) return;
    col.remove(pos[0]);
  }
  void set(const NTuple& pos, const T& val) {
    if (dimensions() <= pos) resize(pos);
    if (val == 0) {
      erase(pos);
      return;
    }
    (*this)[pos] = val;
  }
  void set(index_t pos, const T& val) {
    if (dimensions().count() <= pos) resize(pos);
    if (val == 0) {
      NTuple tpos;
      m_dims.map(pos,tpos);
      erase(tpos);
      return;
    }
    (*this)[pos] = val;
  }
  SparseMatrix<T> getSlice(const IndexArrayVector& index) const {
    SparseMatrix<T> ret;
    ret.m_dims = NTuple(m_dims[0],1);
    ret.m_data[1] = m_data[index[1].get((index_t)1)];
    return ret;
  }
  void deleteColumns(const IndexArray& index) {
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();i++)
      delete_set.insert(uint64(index.get(i)));
    SparseData<T> copy;
    index_t deleted_count = 0;
    for (index_t i=1;i<=cols();i++) {
      if (delete_set.contains(uint64(i))) deleted_count++;
      if (m_data.contains(i))
	copy[i-deleted_count] = m_data[i];
    }
    m_dims = NTuple(m_dims[0],m_dims[1] - delete_set.count());
    m_data = copy;
  }
  void deleteRows(const IndexArray& index) {
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();++i)
      delete_set.insert(uint64(index.get(i)));
    for (typename SparseData<T>::iterator i=m_data.begin();i!=m_data.end();++i) {
      index_t deleted_count = 0;
      SparseSlice<T> copy;
      index_t dp = 1;
      for (typename SparseSlice<T>::iterator j=i.value().begin();j!=i.value().end();++j) {
	if (!delete_set.contains(uint64(j.key()))) {
	  while (dp < j.key()) {
	    if (delete_set.contains(uint64(dp))) ++deleted_count;
	    ++dp;
	  }
	  copy[j.key()-deleted_count] = j.value();
	}
      }
      i.value() = copy;
    }
    m_dims = NTuple(m_dims[0] - delete_set.count(),m_dims[1]);
  }
  void del(const IndexArray& index) {
    if (IsColonOp(index)) {
      *this = SparseMatrix();
      return;
    }
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();i++)
      delete_set.insert(uint64(index.get(i)));
    index_t newSize = length() - delete_set.count();
    if (newSize == length()) return;
    NTuple newDim;
    if (isRowVector())
      newDim = NTuple(1,newSize);
    else
      newDim = NTuple(newSize,1);
    SparseMatrix<T> ret(newDim);
    ConstSparseIterator<T> source(this);
    index_t deleted_count = 0;
    index_t dp = 1;
    while (source.isValid()) {
      index_t source_pos = m_dims.map(source.pos());
      if (!delete_set.contains(uint64(source_pos))) {
	// This element was not deleted.  
	while (dp < source_pos) {
	  if (delete_set.contains(uint64(dp))) ++deleted_count;
	  ++dp;
	}
	ret.set(source_pos-deleted_count,source.value());
      }
      source.next();
    }
    *this = ret;
  }
  void del(const IndexArrayVector& index) {
    if (IsColonOp(index[0]) && IsColonOp(index[1])) {
      *this = SparseMatrix();
      return;
    }
    if (IsColonOp(index[0]))
      deleteColumns(index[1]);
    else if (IsColonOp(index[1]))
      deleteRows(index[0]);
    else
      throw Exception("Unsupported deletion for sparse matrices.");
  }
  void printMe(std::ostream& o) const {
    ConstSparseIterator<T> source(this);
    while (source.isValid()) {
      NTuple pos(source.pos());
      o << pos[1] << "," << pos[0] << "," << source.value() << "\n";
      source.next();
    }
  }
  void resize(const NTuple& pos) {
    SparseMatrix<T> ret(pos);
    ConstSparseIterator<T> source(this);
    while (source.isValid()) {
      ret.set(dimensions().map(source.pos()),source.value());
      source.next();
    }
    *this = ret;
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
  void reshape(const NTuple& pos) {
    if (length() != pos.count()) 
      throw Exception("Illegal reshape");
    resize(pos);
  }
  const BasicArray<T> asDense() const {
    ConstSparseIterator<T> source(this);
    BasicArray<T> retvec(dimensions());
    while (source.isValid()) {
      retvec.set(source.pos(),source.value());
      source.next();
    }
    return retvec;
  }
  bool operator==(const SparseMatrix<T> &data) const {
    ConstSparseIterator<T> source(this);
    ConstSparseIterator<T> dest(&data);
    while (source.isValid() && dest.isValid()) {
      if (!(source.pos() == dest.pos())) return false;
      if (source.value() != dest.value()) return false;
      source.next();
      dest.next();
    }
    return true;
  }
};

template <typename T>
inline std::ostream& operator<<(std::ostream& o, const SparseMatrix<T>& arg) {
  arg.printMe(o);
  return o;
}

template <typename T>
class ConstSparseIterator {
  const SparseMatrix<T> *m_ptr;
  typename SparseData<T>::const_iterator m_col;
  typename SparseSlice<T>::const_iterator m_row;
public:
  ConstSparseIterator(const SparseMatrix<T> *ptr) {
    m_ptr = ptr;
    m_col = m_ptr->constData().constBegin();
    m_row = m_col.value().constBegin();
  }
  inline bool moreInSlice() const {
    return (m_row != m_col.value().constEnd());
  }
  inline void next() {
    if (moreInSlice())
      ++m_row;
  }
  inline void nextSlice() {
    if (isValid()) {
      ++m_col;
      if (isValid())
    	m_row = m_col.value().constBegin();
    }
  }
  bool isValid() const {
    return (m_col != m_ptr->constData().constEnd());
  }
  T value() const {
    return m_row.value();
  }
  const NTuple pos() const {
    return NTuple(m_row.key(),m_col.key());
  }
  index_t row() const {
    return m_row.key();
  }
  index_t col() const {
    return m_col.key();
  }
};

template <typename T>
class ConstComplexSparseIterator {
  ConstSparseIterator<T> m_real;
  ConstSparseIterator<T> m_imag;
  NTuple m_dims;
public:
  ConstComplexSparseIterator(const SparseMatrix<T> *real,
			     const SparseMatrix<T> *imag) : 
    m_real(real), m_imag(imag), m_dims(real->dimensions()) {}
  inline bool moreInSlice() const {
    return (m_real.moreInSlice() || m_imag.moreInSlice());
  }
  void next() {
    if (m_dims.map(m_real.pos()) < m_dims.map(m_imag.pos()))
      m_real.next();
    else if (m_dims.map(m_imag.pos()) < m_dims.map(m_real.pos()))
      m_imag.next();
    else {
      m_real.next();
      m_imag.next();
    }
  }
  void nextSlice() {
    m_real.nextSlice();
    m_imag.nextSlice();
  }
  const NTuple pos() const {
    if (m_dims.map(m_real.pos()) <= m_dims.map(m_imag.pos()))
      return m_real.pos();
    else
      return m_imag.pos();
  }
  bool isValid() const {
    return (m_real.isValid() || m_imag.isValid());
  }
  T realValue() const {
    if (m_dims.map(m_real.pos()) <= m_dims.map(m_imag.pos()))
      return m_real.value();
    else
      return 0;
  }
  T imagValue() const {
    if (m_dims.map(m_imag.pos()) <= m_dims.map(m_real.pos()))
      return m_imag.value();
    else
      return 0;
  }
};

template <typename T>
bool IsNonNegative(const SparseMatrix<T> &x) {
  ConstSparseIterator<T> i(&x);
  while (i.isValid()) {
    while (i.moreInSlice()) {
      if (i.value() < 0) return false;
      i.next();
    }
    i.nextSlice();
  }
  return true;
}

template <typename T>
bool IsInteger(const SparseMatrix<T> &x) {
  ConstSparseIterator<T> i(&x);
  while (i.isValid()) {
    while (i.moreInSlice()) {
      if (IsInteger(i.value())) return false;
      i.next();
    }
    i.nextSlice();
  }
  return true;
}

template <typename S, typename T>
SparseMatrix<T> ConvertSparseArray(const SparseMatrix<S> &x) {
  SparseMatrix<T> retvec(x.dimensions());
  ConstSparseIterator<S> i(&x);
  while (i.isValid()) {
    while (i.moreInSlice()) {
      retvec.set(i.pos(),T(i.value()));
      i.next();
    }
    i.nextSlice();
  }
  return retvec;
}

// c_ij = sum_k a_ik * b_kj
// For j fixed
// For all i
// c_i = sum_k a_ik * b_k
template <typename T>
SparseMatrix<T> MatrixMultiply(const SparseMatrix<T> &A, const SparseMatrix<T> &B) {
  if (A.columns() != B.rows())
    throw Exception("Non conforming arrays for matrix multiply");
  typename SparseData<T>::const_iterator B_iter(B.constData().constBegin());
  SparseMatrix<T> C;
  while (B_iter != B.constData().constEnd()) {
    SparseData<T> c_slice;
    SparseData<T> b_slice(B_iter.value());
    ConstSparseIterator<T> A_iter(&A);
    while (A_iter.isValid()) {
      while (A_iter.moreInSlice()) {
	c_slice[A_iter.row()] += A_iter.value()*b_slice[A_iter.col()];
	A_iter.next();
      }
      A_iter.nextSlice();
    }
    C.data()[B_iter.key()] = c_slice;
    B_iter.nextSlice();
  }
  return C;
}

template <typename T>
SparseMatrix<T> MatrixMultiply(const SparseMatrix<T> &A, const BasicArray<T> &B) {
  if (A.columns() != B.rows())
    throw Exception("Non conforming arrays for matrix multiply");
  SparseMatrix<T> C;
  for (index_t col = 1;col <= B.cols();col++) {
    SparseData<T> c_slice;
    ConstSparseIterator<T> A_iter(&A);
    while (A_iter.isValid()) {
      while (A_iter.moreInSlice()) {
	c_slice[A_iter.row()] += A_iter.value()*B.get(NTuple(A_iter.col(),col));
	A_iter.next();
      }
      A_iter.nextSlice();
    }
    C.data()[col] = c_slice;
    B_iter.nextSlice();
  }
  return C;
}

template <typename T>
SparseMatrix<T> MatrixMultiply(const BasicArray<T> &A, const SparseArray<T> &B) {
  
}
#endif
