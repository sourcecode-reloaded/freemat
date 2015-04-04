#include "MatrixInvert.hpp"

using namespace FM;



Object FM::MatrixInvert(const Object &a, ThreadContext *ctxt) {
  if (!a.is2D()) throw Exception("Cannot invert N-dimensional arrays.");
  if (!a.isSquare()) throw Exception("Cannot invert non-square matrices.");
  if (a.isEmpty()) return a;
  switch (a.typeCode()) {
  default: throw Exception("Invert does not support this data class");
  case TypeSingle:
    if (a.isComplex())
      return MatrixInvertComplex<float,TypeSingle>(a.data(),cgetrf_,cgetri_,ctxt->_single);
    else
      return MatrixInvertReal<float,TypeSingle>(a.data(),sgetrf_,sgetri_,ctxt->_single);
  case TypeDouble:
    if (a.isComplex())
      return MatrixInvertComplex<double,TypeDouble>(a.data(),zgetrf_,zgetri_,ctxt->_double);
    else
      return MatrixInvertReal<double,TypeDouble>(a.data(),dgetrf_,dgetri_,ctxt->_double);
  }
}
