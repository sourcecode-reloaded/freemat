#ifndef __CodeType_hpp__
#define __CodeType_hpp__

#include "PODType.hpp"
#include "Object.hpp"
#include "AggregateType.hpp"

namespace FM
{
  struct CodeData {
    Object m_name;
    Object m_code;
    Object m_names;
    Object m_consts;
    Object m_params;
    Object m_returns;
  };

  struct ThreadContext;

  class CodeType : public AggregateType<CodeData> {
  public:
    CodeType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeCode;}
    virtual const FMString& name() const {static FMString _name = "code"; return _name;}
    bool equals(const Object &a, const Object &b) {
      throw Exception("Code type doesn't support equals yet.");
    }
    Object makeCodeObject();
    virtual FMString describe(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object call(const Object &a, const Object &args, int nargout);
    virtual Object deref(const Object &a);
    virtual Object empty() {throw Exception("Code type cannot be empty.");}
  };
}

#endif
