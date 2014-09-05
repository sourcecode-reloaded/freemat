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
		   reinterpret_cast<const double*>(dt->readOnlyDataComplex(dt->asComplex(a))),
		   reinterpret_cast<const double*>(dt->readOnlyDataComplex(dt->asComplex(b))),
		   reinterpret_cast<double*>(dt->readWriteDataComplex(ret)));
  else
    FM::BLAS_dgemm(a.rows(),a.cols(),b.cols(),
		   dt->readOnlyData(a),dt->readOnlyData(b),dt->readWriteData(ret));
  return ret;
}

Object FM::MatrixMultiplySingle(const Object &a, const Object &b) {
  if (a.cols() != b.rows()) throw Exception("Matrix multiplication requires arguments to be conformant");
  SingleType *dt = a.asType<SingleType>();
  Object ret = dt->makeMatrix(a.rows(),b.cols(),a.isComplex() || b.isComplex());
  if (a.isComplex() || b.isComplex())
    FM::BLAS_cgemm(a.rows(),a.cols(),b.cols(),
		   reinterpret_cast<const float*>(dt->readOnlyDataComplex(dt->asComplex(a))),
		   reinterpret_cast<const float*>(dt->readOnlyDataComplex(dt->asComplex(b))),
		   reinterpret_cast<float*>(dt->readWriteDataComplex(ret)));
  else
    FM::BLAS_sgemm(a.rows(),a.cols(),b.cols(),
		   dt->readOnlyData(a),dt->readOnlyData(b),dt->readWriteData(ret));
  return ret;
}

