#ifndef __MatrixLeftDivide_hpp__
#define __MatrixLeftDivide_hpp__

#include "Object.hpp"
#include "FloatType.hpp"

namespace FM
{
  class TermIF;

  template <class T, FM::DataCode codeNum>
  Object MatrixLeftDivide(const Object &a, const Object &b, TermIF *io, FloatType<T,codeNum> *ft);
};

#endif
