#ifndef __BreakpointType_hpp__
#define __BreakpointType_hpp__

#include "AggregateType.hpp"

namespace FM
{

  struct ThreadContext;
  
  enum class BreakpointTypeCode {Unknown, Unconditional, Conditional, When, Transient};
  enum class BreakpointWhen {Always, Error, CaughtError, Warning, NanInf};

  class BreakpointData {
  public:
    FMString           frame_name;
    int                line_number;
    BreakpointTypeCode bp_type;
    BreakpointWhen     bp_when;
    int                id;
    int                instruction;
    Object             m_condition;
    BreakpointData(ThreadContext *ctxt);
  };

  class BreakpointType : public AggregateType<BreakpointData,ValueSemantics> {
  public:
    BreakpointType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeBreakpoint;}
    virtual const FMString& name() const {static FMString _name = "breakpoint"; return _name;}
    void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
      const BreakpointData *bd = this->ro(p);
      visitor(bd->m_condition);
    }
    FMString describe(const Object &a);
    FMString brief(const Object &a);
    Object import(const Object &foreign) {
      BreakpointType *them = foreign.asType<BreakpointType>();
      if (this == them) return foreign;
      Object ret = this->makeScalar();
      BreakpointData *bd = this->rw(ret);
      const BreakpointData *cbd = them->ro(foreign);
      bd->frame_name = cbd->frame_name;
      bd->line_number = cbd->line_number;
      bd->bp_type = cbd->bp_type;
      bd->bp_when = cbd->bp_when;
      bd->id = cbd->id;
      bd->instruction = cbd->instruction;
      bd->m_condition = cbd->m_condition.exportTo(_ctxt);
      return ret;
    }
  };
};

#endif
