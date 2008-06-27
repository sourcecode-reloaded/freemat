#include "SparseCCS.hpp"
#include "Math.hpp"

// Normally, the i,j,v representation for a sparse matrix has
// a great degree of redundancy in one of the components.  If
// j is the column coordinate then a vector such as:
//   1 3
//   3 3
//   5 3

QVector<uint32> CompressCCSCols(const QVector<uint32> &cols, int colcount) {
  QVector<uint32> y(colcount+1);
  int xp = 0;
  for (int i=0;i<colcount;i++) {
    if ((xp >= cols.size()) || (i < cols[xp]))
      y[i+1] = y[i];
    else {
      while ((xp < cols.size()) && (cols[xp] == i)) xp++;
      y[i+1] = xp-1;
    }
  }
  return y;
}
  
Array IJVForm::rows() {
  return m_rows;
}

Array IJVForm::cols() {
  return m_cols;
}

Array IJVForm::indices() {
  return Add(Multiply(Subtract(m_cols,Array(double(1))),
		      Array(double(m_size_rows))),m_rows); 
}

Array IJVForm::values() {
  return m_values;
}

index_t IJVForm::nnz() {
  return m_values.length();
}
