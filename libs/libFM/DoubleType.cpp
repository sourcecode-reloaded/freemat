#include "DoubleType.hpp"
#include "BaseTypes.hpp"
#include "BoolType.hpp"
#include "MatrixMultiply.hpp"
#include "MatrixLeftDivide.hpp"
#include "MatrixRightDivide.hpp"
#include "ThreadContext.hpp"

namespace FM
{
  Object DoubleType::Multiply(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotMultiply(a,b);
    return b.type()->convert(MatrixMultiplyDouble(a,this->convert(b)));
  }
  Object DoubleType::LeftDivide(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotLeftDivide(a,b);
    return b.type()->convert(MatrixLeftDivideDouble(a,this->convert(b),_ctxt->_io));
  }
  Object DoubleType::RightDivide(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotRightDivide(a,b);
    return b.type()->convert(MatrixRightDivideDouble(a,this->convert(b),_ctxt->_io));
  }
}
