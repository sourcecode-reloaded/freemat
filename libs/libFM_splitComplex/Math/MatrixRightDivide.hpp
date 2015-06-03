#ifndef __MatrixRightDivide_hpp__
#define __MatrixRightDivide_hpp__

#include "Object.hpp"
#include "FloatType.hpp"

namespace FM
{
  class TermIF;
  
  template <class T, FM::DataCode codeNum>
  Object MatrixRightDivide(const Object &a, const Object &b, TermIF *io, FloatType<T,codeNum> *ft);
};

#endif
