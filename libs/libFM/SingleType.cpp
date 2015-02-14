#include "SingleType.hpp"
#include "MatrixMultiply.hpp"
#include "BoolType.hpp"

namespace FM
{
  Object SingleType::Or(const Object &a, const Object &b) {
    return cmpop<OpOr>(a,b,_ctxt->_bool);
  }

  Object SingleType::And(const Object &a, const Object &b) {
    return cmpop<OpAnd>(a,b,_ctxt->_bool);
  }

  
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
}
