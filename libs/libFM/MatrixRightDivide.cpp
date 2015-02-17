#include "MatrixRightDivide.hpp"
#include "MatrixLeftDivide.hpp"
#include "DenseSolver.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

using namespace FM;

template <class T, FM::DataCode codeNum>
Object FM::MatrixRightDivide(const Object &a, const Object &b, TermIF *io, FloatType<T,codeNum> *ft) {
  if (b.cols() != a.cols()) throw Exception("Matrix right division (e.g., a/B) requires a and B to have the same number of cols");
  Object A = ft->Transpose(a);
  Object B = ft->Transpose(b);
  Object ret = MatrixLeftDivide<T,codeNum>(B,A,io,ft);
  return ft->Transpose(ret);
}

template Object FM::MatrixRightDivide(const Object &a, const Object &b, TermIF *io, FloatType<double,TypeDouble> *ft);
template Object FM::MatrixRightDivide(const Object &a, const Object &b, TermIF *io, FloatType<float,TypeSingle> *ft);

