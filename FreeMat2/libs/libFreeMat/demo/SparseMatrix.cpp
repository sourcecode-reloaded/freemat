#include "SparseMatrix.hpp"
#include "BasicArray.hpp"

SparseMatrix SparseMatrix::get(const IndexArrayList& index) const {
}

SparseMatrix SparseMatrix::get(const IndexArray& index) const {
  if (index.isEmpty())
    return SparseMatrix(index.dimensions());
  if (IsColonOp(index)) {
    SparseMatrix retvec(*this);
    retvec.reshape(NTuple(length(),1));
    return retvec;
  }
  if (isEmpty())
    throw Exception("Caannot index into empty variable.");
  NTuple retdims;
  if (isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  SparseMatrix retvec(retdims);
  for (int i=1;i<=retvec.length();i++)
    retvec[i] = get(index.get(i));
  return retvec;
  
}
