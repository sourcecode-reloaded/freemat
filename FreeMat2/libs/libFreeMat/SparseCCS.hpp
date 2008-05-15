#ifdef __SparseCCS_hpp__
#define __SparseCCS_hpp__

#include <QVector>
#include "SparseMatrix.hpp"

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

#endif
