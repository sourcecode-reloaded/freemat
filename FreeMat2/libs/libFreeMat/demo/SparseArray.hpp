#ifndef __SparseArray_hpp__
#define __SparseArray_hpp__

#include <QMap>
#include <QtGlobal>
#include "BaseArray.hpp"
#include <iostream>

template <typename T>
class SparseSlice : public QMap<index_t,T> {};

template <typename T>
class SparseMatrix : public BaseArray<T> {
  QMap<index_t, SparseSlice<T> > m_data;
  NTuple m_dims;
public:
  SparseMatrix(QVector<index_t> row, QVector<index_t> col, QVector<T> val) {
    std::cout << "row " << row.size() << "\n";
    std::cout << "col " << col.size() << "\n";
    std::cout << "val " << val.size() << "\n";
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
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      if (!m_data.contains(pos[1]))
	return 0;
      const SparseSlice<T> & col = m_data.value(pos[1]);
      if (!col.contains(pos[0]))
	return 0;
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
};

template <typename T>
class SparseIterator {
  SparseMatrix<T> *m_ptr;
  int m_dim;
  
public:
  SparseIterator(SparseMatrix<T> *ptr, int dim) : 
    m_ptr(ptr), m_dim(dim) {}
  
  
};

#endif
