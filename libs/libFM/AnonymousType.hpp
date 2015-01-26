#ifndef __AnonymousType_hpp__
#define __AnonymousType_hpp__

#include "AggregateType.hpp"
#include "HashMap.hpp"

namespace FM
{
  struct ThreadContext;
  class Frame;
  
  class AnonymousData {
  public:
    Object m_code;              // The code object
    Object m_printable;         // The printable string that describes the object
    HashMap<Object> m_captured; // The captured variables for the anonymous function
    AnonymousData(ThreadContext *ctxt) : 
      m_code(ctxt), m_printable(ctxt) {}
  };

  class AnonymousType : public AggregateType<AnonymousData,ValueSemantics> {
  public:
    AnonymousType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
      const AnonymousData *ad = this->ro(p);
      for (auto i : ad->m_captured)
	visitor(i.second);
    }
    virtual DataCode code() const {return TypeAnonymous;}
    virtual const FMString &name() const {static FMString _name = "anonymous function"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    Object build(const Object &description, const Object &codeblock, Frame *frame);
  };
};

#endif
