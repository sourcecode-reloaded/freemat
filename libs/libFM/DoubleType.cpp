#include "DoubleType.hpp"
#include "BaseTypes.hpp"
#include "BoolType.hpp"

namespace FM
{
  Object DoubleType::LessEquals(const Object &a, const Object &b) {
    return cmpop<OpLE>(a,b,_base->_bool);
  }

  Object DoubleType::LessThan(const Object &a, const Object &b) {
    return cmpop<OpLT>(a,b,_base->_bool);
  }
}
