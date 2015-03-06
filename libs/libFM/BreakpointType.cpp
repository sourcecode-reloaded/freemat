#include "BreakpointType.hpp"
#include "ThreadContext.hpp"
#include "Object.hpp"

using namespace FM;

BreakpointData::BreakpointData(ThreadContext *ctxt) :
   frame_name(""), line_number(0), bp_type(BreakpointTypeCode::Unknown), id(0), instruction(0), m_condition(ctxt) {}

FMString BreakpointType::describe(const Object &a) {
  FMString ret;
  const BreakpointData *bd = this->ro(a);
  ret += "BP: " + bd->frame_name + " " + Stringify(bd->line_number) + " >" + Stringify(bd->instruction);
  return ret;
}

FMString BreakpointType::brief(const Object &a) {
  const BreakpointData *bd = this->ro(a);
  return "Breakpoint " + Stringify(bd->id);
}
