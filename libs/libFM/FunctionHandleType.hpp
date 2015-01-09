#ifndef __FunctionHandleType_hpp__
#define __FunctionHandleType_hpp__

#include "AggregateType.hpp"

namespace FM
{
  struct ThreadContext;

  class FunctionHandleData {
  public:
    bool m_classMethod; // The type of the function handle - e.g., function, class method
    Object m_func;
    Object m_class;
    FunctionHandleData(ThreadContext *ctxt) : 
      m_func(ctxt), m_class(ctxt) {}
  };

  class FunctionHandleType : public AggregateType<FunctionHandleData,ValueSemantics> {
  public:
    FunctionHandleType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeFunctionHandle;}
    virtual const FMString &name() const {static FMString _name = "function handle"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    Object fromFunction(const Object &func);
  };
}

#endif
