#include "MatrixOps.hpp"
#include "ThreadContext.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"
#include "EigenDecompose.hpp"

using namespace FM;

template <typename T>
static Object TPowerScalarMatrix(const Object &A, const Object &B, FloatType<T> *ft) {
  // Do an eigendecomposition of B
  Object V = ft->empty();
  Object D = ft->empty();
  if (ft->isSymmetric(B))
    EigenDecomposeFullSymmetric(B,V,D,ft->context());
  else
    EigenDecomposeFull(B,V,D,false,ft->context());
  // Get the diagonal part of D
  Object E = ft->getDiagonal(D);
  // Call the vector version of the exponential
  Object F = ft->DotPower(A,E); // B, V, D, E, F
  // Construct a diagonal matrix from F
  Object G = ft->asDiagonalMatrix(F); // B, V, D, G, E, F
  // The output is (V*G)/V
  E = ft->Multiply(V,G); // B, V, D, E, F
  return ft->RightDivide(E,V); // B, D, F
}

// Raises Matrix^Scalar
template <typename T>
static Object TPowerMatrixScalar(const Object &A, const Object &B, FloatType<T> *ft) {
  // Do an eigendecomposition of A
  Object V = ft->empty();
  Object D = ft->empty();
  if (ft->isSymmetric(A))
    EigenDecomposeFullSymmetric(A,V,D,ft->context()); //A, B, V, D
  else
    EigenDecomposeFull(A,V,D,false,ft->context());
  // Get the diagonal part of D
  Object E = ft->getDiagonal(D); // A, B, V, D, E
  // Call the vector version of the exponential
  Object F = ft->DotPower(E,B); // F, A, V, D
  // Construct a diagonal matrix from F
  Object G = ft->asDiagonalMatrix(F); // G, A, V, D, F
  // The output is (V*G)/V
  E = ft->Multiply(V,G); // A, V, D, E, F
  return ft->RightDivide(E,V); // C, A, D, F
}


Object FM::MatrixPower(const Object &a, const Object &b, ThreadContext *ctxt) {
  if (a.isEmpty() || b.isEmpty()) return a.type()->empty();
  if (a.isScalar() && b.isScalar()) return a.type()->DotPower(a,b);
  if (b.isScalar() && !b.isComplex() && (b.asDouble() == -1))
    return MatrixInvert(a,ctxt);
  if (!(a.isSquare() && b.isSquare()))
    throw Exception("Power (^) operator can only be applied to scalar and square arguments.");
  if (a.typeCode() != b.typeCode())
    return ctxt->_single->convert(MatrixPower(ctxt->_double->convert(a),ctxt->_double->convert(b),ctxt));
  switch (a.typeCode()) {
  case TypeDouble:
    if (a.isScalar()) return TPowerScalarMatrix<double>(a,b,ctxt->_double);
    if (b.isScalar()) return TPowerMatrixScalar<double>(a,b,ctxt->_double);
    break;
  case TypeSingle:
    if (a.isScalar()) return TPowerScalarMatrix<float>(a,b,ctxt->_single);
    if (b.isScalar()) return TPowerMatrixScalar<float>(a,b,ctxt->_single);
    break;
  default:
    throw Exception("Data class does not support power operator");
  }
  throw Exception("One of the arguments to (^) must be a scalar");
}
