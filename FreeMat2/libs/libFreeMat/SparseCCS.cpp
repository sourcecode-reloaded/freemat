#include "SparseCCS.hpp"
#include "Math.hpp"

void ConvertSparseToCCS(const SparseMatrix<double>&A, CCSForm &B) {
  B.Acolstart.resize(int(A.cols()+1));
  int m = 1;
  int col = 0;
  B.Acolstart[0] = 0;
  ConstSparseIterator<double> iter(&A);
  while (iter.isValid()) {
    while (m <iter.col()) {
      B.Acolstart[m] = B.Adata.size();
      m++;
    }
    while (iter.moreInSlice()) {
      B.Arowindex << int(iter.row()-1);
      B.Adata << iter.value();
      iter.next();
    }
    iter.nextSlice();
  }
  while (m <= A.cols()) {
    B.Acolstart[m] = B.Adata.size();
    m++;
  }
}

void ConvertSparseToCCS(const SparseMatrix<double> &Areal, const SparseMatrix<double> &Aimag, CCSForm &B) {
  B.Acolstart.resize(int(Areal.cols()+1));
  int m = 1;
  int col = 0;
  B.Acolstart[0] = 0;
  ConstComplexSparseIterator<double> iter(&Areal,&Aimag);
  while (iter.isValid()) {
    while (m <iter.col()) {
      B.Acolstart[m] = B.Adata.size();
      m++;
    }
    while (iter.moreInSlice()) {
      B.Arowindex << int(iter.row()-1);
      B.Adata << iter.realValue();
      B.Aimag << iter.imagValue();
      iter.next();
    }
    iter.nextSlice();
  }
  while (m <= Areal.cols()) {
    B.Acolstart[m] = B.Adata.size();
    m++;
  }
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
