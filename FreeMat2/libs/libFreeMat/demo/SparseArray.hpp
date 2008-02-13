#ifndef __SparseArray_hpp__
#define __SparseArray_hpp__

#include <QMap>

template <typename T>
class SparseSlice : public QMap<index_t,T> {};

template <typename T>
class SparseMatrix {
  QMap<index_t, SparseSlice<T> > m_data;
public:
  SparseMatrix(QVector<index_t> row, QVector<index_t> col, QVector<T> val) {
    if (!(row.size() == col.size() == val.size()))
      throw Exception("Invalid sizes for IJV style constructor");
    for (int i=0;i<row.size();i++) {
      m_data[col[i]][row[i]] = val[i];
    }
  }
};



#endif
