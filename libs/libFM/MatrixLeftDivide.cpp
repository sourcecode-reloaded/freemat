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
			      reinterpret_cast<double*>(dt->readWriteDataComplex(ret)),
			      reinterpret_cast<const double*>(dt->readOnlyDataComplex(dt->asComplex(a))),
			      reinterpret_cast<const double*>(dt->readOnlyDataComplex(dt->asComplex(b))),
			      io);
  else
    DenseSolve<double>(a.rows(),a.cols(),b.cols(),
		       dt->readWriteData(ret),dt->readOnlyData(a),dt->readOnlyData(b),io);
  return ret;
}
