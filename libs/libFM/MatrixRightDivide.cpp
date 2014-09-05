#include "MatrixRightDivide.hpp"
#include "MatrixLeftDivide.hpp"
#include "DenseSolver.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

using namespace FM;

Object FM::MatrixRightDivideDouble(const Object &a, const Object &b, TermIF *io) {
  if (b.cols() != a.cols()) throw Exception("Matrix right division (e.g., a/B) requires a and B to have the same number of cols");
  DoubleType *dt = a.asType<DoubleType>();
  Object A = dt->Transpose(a);
  Object B = dt->Transpose(b);
  Object ret = MatrixLeftDivideDouble(B,A,io);
  return dt->Transpose(ret);
}

