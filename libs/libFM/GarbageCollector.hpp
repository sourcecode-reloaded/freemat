#ifndef __GarbageCollector_hpp__
#define __GarbageCollector_hpp__

#include "Object.hpp"

namespace FM {
  Object builtin_gc(const Object &args, int nargout, ThreadContext *ctxt);
}

#endif
