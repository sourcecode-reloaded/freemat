#ifndef __BoundFunctionType_hpp__
#define __BoundFunctionType_hpp__

#include "AggregateType.hpp"

namespace FM
{
  struct ThreadContext;

  class BoundFunctionData {
  public:
    Object m_func;
    Object m_arg;
    BoundFunctionData(ThreadContext *ctxt) : 
      m_func(ctxt), m_arg(ctxt) {}
  };

  class BoundFunctionType : public AggregateType<BoundFunctionData,ValueSemantics> {
  public:
    BoundFunctionType(ThreadContext *ctxt) {_ctxt = ctxt;}
    Object bindFunction(const Object &func, const Object &arg);
    virtual void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
      visitor(this->ro(p)->m_arg);
      visitor(this->ro(p)->m_func);
    }
    virtual DataCode code() const {return TypeBoundFunction;}
    virtual const FMString& name() const {static FMString _name = "bound_function"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    virtual Object deref(const Object &a);
  };
};

#endif
