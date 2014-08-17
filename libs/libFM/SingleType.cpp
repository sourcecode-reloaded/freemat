#include "SingleType.hpp"
#include "MatrixMultiply.hpp"

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
}
