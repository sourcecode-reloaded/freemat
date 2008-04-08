#include "SparseMatrix.hpp"
#include "BasicArray.hpp"


SparseMatrix SparseMatrix::getSlice(const IndexArrayList& index) const {
  SparseMatrix ret;
  ret.m_dims = NTuple(m_dims[0],1);
  ret.m_data[1] = m_data[index[1].get((index_t)1)];
  return ret;
}

