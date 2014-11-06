#ifndef __CapturedType_hpp__
#define __CapturedType_hpp__

#include "Object.hpp"
#include "Type.hpp"

namespace FM
{
  struct ThreadContext;
  
  class CapturedData {
  public:
    Object m_data;
    CapturedData(ThreadContext *_ctxt) : m_data(_ctxt) {}
  };

  class CapturedType : public AggregateType<CapturedData,HandleSemantics> {
  public:
    CapturedType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeCaptured;}
    virtual const FMString& name() const {static FMString _name = "captured"; return _name;}
    virtual FMString describe(const Object &captured) {
      return "Captured <<" + this->ro(captured)->m_data.description() + ">>\n";
    }
    virtual FMString brief(const Object &captured) {
      return "<<" + this->ro(captured)->m_data.brief() + ">>";
    }
    virtual bool equals(const Object &a, const Object &b) {
      return false;
    }
    CapturedData* makeEmptyDataType() {return new CapturedData(_ctxt);}
    void set(Object &captured, const Object &contents) {this->rw(captured)->m_data = contents;}
    Object get(const Object &captured) {return(this->ro(captured)->m_data);}
  };
};

#endif
