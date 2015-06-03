#ifndef __MatrixLeftDivide_hpp__
#define __MatrixLeftDivide_hpp__

#include "Object.hpp"

namespace FM
{
  Object MatrixLeftDivideDouble(const Object &a, const Object &b);
  Object MatrixLeftDivideSingle(const Object &a, const Object &b);
  Object MatrixRightDivideDouble(const Object &a, const Object &b);
  Object MatrixRightDivideSingle(const Object &a, const Object &b);
};

#endif
