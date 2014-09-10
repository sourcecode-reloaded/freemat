#ifndef __NCat_hpp__
#define __NCat_hpp__

#include "Object.hpp"

namespace FM
{
  struct ThreadContext;

  Object NCat(ThreadContext *ctxt, const Object &x, int dimension);
}

#endif
