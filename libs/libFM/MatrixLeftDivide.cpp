#include "MatrixLeftDivide.hpp"
#include "DenseSolver.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

using namespace FM;

Object FM::MatrixLeftDivideDouble(const Object &a, const Object &b, TermIF *io) {
  if (a.rows() != b.rows()) throw Exception("Matrix left division (e.g., A\b) requires A and b to have the same number of rows");
  DoubleType *dt = a.asType<DoubleType>();
  Object ret = dt->makeMatrix(a.cols(),b.cols(),a.isComplex() || b.isComplex());
  if (a.isComplex() || b.isComplex())
    DenseSolveComplex<double>(a.rows(),a.cols(),b.cols(),
			      reinterpret_cast<double*>(dt->rwComplex(ret)),
			      reinterpret_cast<const double*>(dt->roComplex(dt->asComplex(a))),
			      reinterpret_cast<const double*>(dt->roComplex(dt->asComplex(b))),
			      io);
  else
    DenseSolve<double>(a.rows(),a.cols(),b.cols(),
		       dt->rw(ret),dt->ro(a),dt->ro(b),io);
  return ret;
}
