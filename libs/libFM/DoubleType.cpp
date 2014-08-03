#include "DoubleType.hpp"
#include "BaseTypes.hpp"
#include "BoolType.hpp"

namespace FM
{
  Object DoubleType::Equals(const Object &a, const Object &b) {
    return cmpop<OpEQ>(a,b,_base->_bool);
  }

  Object DoubleType::NotEquals(const Object &a, const Object &b) {
    return cmpop<OpNE>(a,b,_base->_bool);
  }

  Object DoubleType::LessEquals(const Object &a, const Object &b) {
    return cmpop<OpLE>(a,b,_base->_bool);
  }

  Object DoubleType::LessThan(const Object &a, const Object &b) {
    return cmpop<OpLT>(a,b,_base->_bool);
  }

  Object DoubleType::GreaterEquals(const Object &a, const Object &b) {
    return cmpop<OpGE>(a,b,_base->_bool);
  }

  Object DoubleType::GreaterThan(const Object &a, const Object &b) {
    return cmpop<OpGT>(a,b,_base->_bool);
  }

  Object DoubleType::Or(const Object &a, const Object &b) {
    return cmpop<OpOr>(a,b,_base->_bool);
  }

  Object DoubleType::And(const Object &a, const Object &b) {
    return cmpop<OpAnd>(a,b,_base->_bool);
  }

}
