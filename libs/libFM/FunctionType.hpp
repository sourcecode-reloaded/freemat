#ifndef __FunctionType_hpp__
#define __FunctionType_hpp__

#include "AggregateType.hpp"

namespace FM
{
  struct ThreadContext;

  class FunctionData {
  public:
    Object m_name;      // The name of the functon object
    Object m_code;      // The code object for the function
    Object m_closure;   // The closure of the function, list of captured variables which map to the free variables
    FunctionData(ThreadContext *ctxt) : 
      m_name(ctxt), m_code(ctxt), m_closure(ctxt) {}
  };

  class FunctionType : public AggregateType<FunctionData,ValueSemantics> {
  public:
    FunctionType(ThreadContext *ctxt) {_ctxt = ctxt;}
    // Note - Only the closure can contain references to handle objects
    virtual const Object* containedObjects(const ObjectBase *p, dim_t &count) const {
      const FunctionData *fd = static_cast<const FunctionData *>(p->data->ptr);
      count = 1;
      return &(fd->m_closure);
    }
    virtual DataCode code() const {return TypeFunction;}
    virtual const FMString& name() const {static FMString _name = "function"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    virtual Object deref(const Object &a);
    Object fromCode(const Object &codeObject, const Object &closure);
    Object fromCode(const Object &codeObject);
  };

};
#endif
