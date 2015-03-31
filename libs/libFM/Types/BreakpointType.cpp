#include "BreakpointType.hpp"
#include "ThreadContext.hpp"
#include "Object.hpp"

using namespace FM;

BreakpointData::BreakpointData(ThreadContext *ctxt) :
   frame_name(""), line_number(0), bp_type(BreakpointTypeCode::Unknown), id(0), instruction(0), m_condition(ctxt) {}

FMString BreakpointType::describe(const Object &a) {
  FMString ret;
  const BreakpointData *bd = this->ro(a);
  ret += "BP: " + bd->frame_name + " " + Stringify(bd->line_number);
  if (!bd->m_condition.isEmpty()) ret += " (conditional)";
  switch (bd->bp_when) {
  case BreakpointWhen::Always: ret += " always"; break;
  case BreakpointWhen::Error: ret + " when error"; break;
  case BreakpointWhen::CaughtError: ret + " when caught error"; break;
  case BreakpointWhen::Warning: ret + " when warning"; break;
  case BreakpointWhen::NanInf: ret + " when naninf"; break;
  }
  return ret;
}

FMString BreakpointType::brief(const Object &a) {
  const BreakpointData *bd = this->ro(a);
  return "Breakpoint " + Stringify(bd->id);
}
