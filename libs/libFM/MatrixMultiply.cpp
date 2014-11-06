#include "MatrixMultiply.hpp"
#include "BLAS.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

using namespace FM;

Object FM::MatrixMultiplyDouble(const Object &a, const Object &b) {
  if (a.cols() != b.rows()) throw Exception("Matrix multiplication requires arguments to be conformant");
  DoubleType *dt = a.asType<DoubleType>();
  Object ret = dt->makeMatrix(a.rows(),b.cols(),a.isComplex() || b.isComplex());
  if (a.isComplex() || b.isComplex())
    FM::BLAS_zgemm(a.rows(),a.cols(),b.cols(),
		   reinterpret_cast<const double*>(dt->roComplex(dt->asComplex(a))),
		   reinterpret_cast<const double*>(dt->roComplex(dt->asComplex(b))),
		   reinterpret_cast<double*>(dt->rwComplex(ret)));
  else
    FM::BLAS_dgemm(a.rows(),a.cols(),b.cols(),
		   dt->ro(a),dt->ro(b),dt->rw(ret));
  return ret;
}

Object FM::MatrixMultiplySingle(const Object &a, const Object &b) {
  if (a.cols() != b.rows()) throw Exception("Matrix multiplication requires arguments to be conformant");
  SingleType *dt = a.asType<SingleType>();
  Object ret = dt->makeMatrix(a.rows(),b.cols(),a.isComplex() || b.isComplex());
  if (a.isComplex() || b.isComplex())
    FM::BLAS_cgemm(a.rows(),a.cols(),b.cols(),
		   reinterpret_cast<const float*>(dt->roComplex(dt->asComplex(a))),
		   reinterpret_cast<const float*>(dt->roComplex(dt->asComplex(b))),
		   reinterpret_cast<float*>(dt->rwComplex(ret)));
  else
    FM::BLAS_sgemm(a.rows(),a.cols(),b.cols(),
		   dt->ro(a),dt->ro(b),dt->rw(ret));
  return ret;
}

