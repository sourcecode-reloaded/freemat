#ifndef __Power_hpp__
#define __Power_hpp__

#include "Object.hpp"

namespace FM
{
  // Computes a^b
  Object MatrixPower(const Object &a, const Object&b, ThreadContext *ctxt);
}

#endif
