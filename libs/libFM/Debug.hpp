#ifndef __Debug_hpp__
#define __Debug_hpp__

#include "ThreadContext.hpp"
#include "Object.hpp"


namespace FM
{
  void DBStop(ThreadContext *ctxt, const Object &args);
};

#endif
