#ifndef __SparseCCS_hpp__
#define __SparseCCS_hpp__

#include <QVector>
#include "SparseMatrix.hpp"
#include "Array.hpp"

struct CCSForm {
  QVector<int> Acolstart;
  QVector<int> Arowindex;
  QVector<double> Adata;
  QVector<double> Aimag;
  const int* colstart() const {return Acolstart.constData();}
  const int* rowindx() const {return Arowindex.constData();}
  const double* data() const {return Adata.constData();}
  const double* imag() const {return Aimag.constData();}
};

void ConvertSparseToCCS(const SparseMatrix<double>&A, CCSForm &B);
void ConvertSparseToCCS(const SparseMatrix<double>&Ar, const SparseMatrix<double>&Ai, CCSForm &B);

class IJVForm {
  Array m_rows;
  Array m_cols;
  index_t m_size_rows;
  Array m_values;
public:
  IJVForm(const Array &x);
  Array rows();
  Array cols();
  Array indices();
  Array values();
  index_t nnz();
};

#endif
