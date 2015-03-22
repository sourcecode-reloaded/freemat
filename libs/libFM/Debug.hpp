#ifndef __Debug_hpp__
#define __Debug_hpp__

#include "ThreadContext.hpp"
#include "Object.hpp"


namespace FM
{
  Object dbstop(const Object &args, int nargout, ThreadContext *ctxt);
  Object dbquit(const Object &args, int nargout, ThreadContext *ctxt);
  Object dbclear(const Object &args, int nargout, ThreadContext *ctxt);
};

#endif
