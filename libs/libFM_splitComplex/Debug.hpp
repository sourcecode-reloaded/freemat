#ifndef __Debug_hpp__
#define __Debug_hpp__

#include "ThreadContext.hpp"
#include "Object.hpp"


namespace FM
{
  Object dbstop(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbquit(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbclear(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbstep(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbup(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbcont(const Object &args, ndx_t nargout, ThreadContext *ctxt);
  Object dbdown(const Object &args, ndx_t nargout, ThreadContext *ctxt);
};

#endif
