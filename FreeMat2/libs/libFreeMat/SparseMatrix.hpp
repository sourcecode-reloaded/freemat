#ifndef __SparseMatrix_hpp__
#define __SparseMatrix_hpp__

#include <QMap>
#include <QtGlobal>
#include <iostream>
#include "Types.hpp"
#include "NTuple.hpp"
#include "BasicArray.hpp"
#include <cmath>

typedef QMap<index_t,double> SparseSlice;
typedef QMap<index_t,SparseSlice> SparseData;

class SparseMatrix {
  SparseData m_data;
  NTuple m_dims;
public:
  SparseMatrix(const NTuple &dims) {
    m_dims = dims;
  }
  SparseMatrix() : m_dims(NTuple(0,0)) {}
  const SparseData& constData() const {return m_data;}
  SparseMatrix(QVector<index_t> row, QVector<index_t> col, QVector<double> val) {
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
  SparseMatrix(const BasicArray<double> &A);
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
  inline const double operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      if (!m_data.contains(pos[1]))
	return double();
      const SparseSlice& col = m_data.value(pos[1]);
      if (!col.contains(pos[0]))
	return double();
      return col.value(pos[0]);
    }
    throw Exception("Out of range");
  }
  inline double& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      return m_data[pos[1]][pos[0]];
    }
    throw Exception("Out of range");
  }
  inline const double operator[](index_t pos) const {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline double& operator[](index_t pos) {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline const double get(index_t pos) const {
    return (*this)[pos];
  }
  inline const double get(const NTuple& pos) const {
    return (*this)[pos];
  }
  void erase(const NTuple& pos) {
    if (!m_data.contains(pos[1])) return;
    SparseSlice &col = m_data[pos[1]];
    if (!col.contains(pos[0])) return;
    col.remove(pos[0]);
  }
  void set(const NTuple& pos, const double& val) {
    if (dimensions() <= pos) resize(pos);
    if (val == 0) {
      erase(pos);
      return;
    }
    (*this)[pos] = val;
  }
  void set(index_t pos, const double& val) {
    if (dimensions().count() <= pos) resize(pos);
    if (val == 0) {
      NTuple tpos;
      m_dims.map(pos,tpos);
      erase(tpos);
      return;
    }
    (*this)[pos] = val;
  }
  SparseMatrix getSlice(const IndexArrayVector& index) const;
  void deleteColumns(const IndexArray& index);
  void deleteRows(const IndexArray& index);
  void del(const IndexArray& index);
  void del(const IndexArrayVector& index);
  void printMe(std::ostream& o) const;
  void resize(const NTuple& pos);
  void resize(index_t len);
  void reshape(const NTuple& pos);
  bool operator==(const SparseMatrix &data) const;
};


inline std::ostream& operator<<(std::ostream& o, const SparseMatrix& arg) {
  arg.printMe(o);
  return o;
}

class ConstSparseIterator {
  const SparseMatrix *m_ptr;
  SparseData::const_iterator m_col;
  SparseSlice::const_iterator m_row;
public:
  ConstSparseIterator(const SparseMatrix *ptr);
  void next();
  bool isValid() const;
  double value();
  const NTuple pos() const;
};

bool IsNonNegative(const SparseMatrix &x) {
  ConstSparseIterator i(&x);
  while (i.isValid()) {
    if (i.value() < 0) return false;
    i.next();
  }
  return true;
}

bool IsInteger(const SparseMatrix &x) {
  ConstSparseIterator i(&x);
  while (i.isValid()) {
    if (IsInteger(i.value())) return false;
    i.next();
  }
  return true;
}

#endif
