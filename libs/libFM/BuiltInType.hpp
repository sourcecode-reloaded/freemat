#ifndef __BuiltInType_hpp__
#define __BuiltInType_hpp__

#include "AggregateType.hpp"

namespace FM
{
  struct ThreadContext;

  typedef Object (*fncptr)(const Object &args, int nargout, ThreadContext *ctxt);
  
  class BuiltInData {
  public:
    Object m_name;
    fncptr m_ptr;
    BuiltInData(ThreadContext *ctxt) : m_name(ctxt),
				       m_ptr(NULL) {}
  };

  class BuiltInType : public AggregateType<BuiltInData,ValueSemantics> {
  public:
    BuiltInType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeBuiltIn;}
    virtual const FMString& name() const {static FMString _name = "builtin"; return _name;}
    virtual FMString brief(const Object &a);
    virtual FMString describe(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    virtual Object deref(const Object &a);
    Object pass();
    void setAddressAndName(Object &a, FMString name, fncptr b);
  };
};

#endif
