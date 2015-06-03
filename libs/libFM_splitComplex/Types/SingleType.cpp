#include "SingleType.hpp"
#include "MatrixMultiply.hpp"
#include "MatrixLeftDivide.hpp"
#include "MatrixRightDivide.hpp"
#include "BoolType.hpp"
#include "DoubleType.hpp"

namespace FM
{
  Object SingleType::Multiply(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotMultiply(a,b);
    switch (b.type()->code())
      {
      case TypeDouble:
	return this->convert(MatrixMultiplyDouble(b.type()->convert(a),b));
      default:
	return MatrixMultiplySingle(a,b);
      }
    return this->empty();
  }
  Object SingleType::LeftDivide(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotLeftDivide(a,b);
    switch (b.type()->code())
      {
      case TypeDouble:
	return this->convert(MatrixLeftDivide<double,TypeDouble>(b.type()->convert(a),b,_ctxt->_io,_ctxt->_double));
      default:
	return MatrixLeftDivide<float,TypeSingle>(a,b,_ctxt->_io,this);
      }
    return this->empty();
  }
  Object SingleType::RightDivide(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotRightDivide(a,b);
    switch (b.type()->code())
      {
      case TypeDouble:
	return this->convert(MatrixRightDivide<double,TypeDouble>(b.type()->convert(a),b,_ctxt->_io,_ctxt->_double));
      default:
	return MatrixRightDivide<float,TypeSingle>(a,b,_ctxt->_io,this);
      }
    return this->empty();
  }
}
