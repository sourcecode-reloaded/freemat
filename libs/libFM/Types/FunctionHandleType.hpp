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
    FunctionHandleType(ThreadContext *ctxt) : AggregateType<FunctionHandleData,ValueSemantics>(ctxt) {}
    void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
      const FunctionHandleData *fhd = this->ro(p);
      visitor(fhd->m_func);
      visitor(fhd->m_class);
    }
    virtual DataCode code() const {return TypeFunctionHandle;}
    virtual const FMString &name() const {static FMString _name = "function handle"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    Object fromFunction(const Object &func);
    Object import(const Object &foreign) {
      FunctionHandleType *them = foreign.asType<FunctionHandleType>();
      if (this == them) return foreign;
      Object ret = this->makeScalar();
      FunctionHandleData *fhd = this->rw(ret);
      const FunctionHandleData *cfhd = them->ro(foreign);
      fhd->m_classMethod = cfhd->m_classMethod;
      fhd->m_func = cfhd->m_func.exportTo(_ctxt);
      fhd->m_class = cfhd->m_class.exportTo(_ctxt);
      return ret;
    }
  };
}

#endif
