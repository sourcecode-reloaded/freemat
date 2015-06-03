#include "MatrixLeftDivide.hpp"
#include "DenseSolver.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

using namespace FM;

template <class T, FM::DataCode codeNum>
Object FM::MatrixLeftDivide(const Object &a, const Object &b, TermIF *io, FloatType<T,codeNum> *ft) {
  if (a.rows() != b.rows()) throw Exception("Matrix left division (e.g., A\b) requires A and b to have the same number of rows");
  Object ret = ft->makeMatrix(a.cols(),b.cols(),a.isComplex() || b.isComplex());
  if (a.isComplex() || b.isComplex())
    DenseSolveComplex<T>(a.irows(),a.icols(),b.icols(),
			 reinterpret_cast<T*>(ft->rwComplex(ret)),
			 reinterpret_cast<const T*>(ft->roComplex(ft->asComplex(a))),
			 reinterpret_cast<const T*>(ft->roComplex(ft->asComplex(b))),
			 io);
  else
    DenseSolve<T>(a.irows(),a.icols(),b.icols(),
		  ft->rw(ret),ft->ro(a),ft->ro(b),io);
  return ret;
}

template Object FM::MatrixLeftDivide(const Object &, const Object &, TermIF *, FloatType<double,TypeDouble> *);
template Object FM::MatrixLeftDivide(const Object &, const Object &, TermIF *, FloatType<float,TypeSingle> *);
