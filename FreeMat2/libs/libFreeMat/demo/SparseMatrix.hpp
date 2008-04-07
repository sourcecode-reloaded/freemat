#ifndef __SparseMatrix_hpp__
#define __SparseMatrix_hpp__

#include <QMap>
#include <QtGlobal>
#include <iostream>
#include "Global.hpp"
#include "NTuple.hpp"
#include "BasicArray.hpp"

class SparseSlice : public QMap<index_t,double> {};

class SparseMatrix {
  QMap<index_t, SparseSlice> m_data;
  NTuple m_dims;
public:
  SparseMatrix(const NTuple &dims) {
    m_dims = dims;
  }
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
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline index_t length() const {return m_dims.count();}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
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
  SparseMatrix get(const IndexArray& index) const;
  SparseMatrix get(const IndexArrayList& index) const;
  void set(const IndexArray& index, const double& data);
  void set(const IndexArray& index, const SparseMatrix& data);
  void set(const IndexArrayList& index, const double& data);
  void set(const IndexArrayList& index, const SparseMatrix& data);
  
  void printMe(std::ostream& o) const;
  void resize(const NTuple& pos);
  void resize(index_t len);
  void reshape(const NTuple& pos);
  bool operator==(const SparseMatrix &data) const;

};

std::ostream& operator<<(std::ostream& o, const SparseMatrix& arg) {
  arg.printMe(o);
  return o;
}

class SparseIterator {
  SparseMatrix *m_ptr;
  int m_dim;
  
public:
  SparseIterator(SparseMatrix *ptr, int dim) : 
    m_ptr(ptr), m_dim(dim) {}
  
  
};

#endif
