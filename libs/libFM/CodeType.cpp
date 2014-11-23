#include "CodeType.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "BoolType.hpp"
#include "IntegerType.hpp"
#include "Compiler.hpp"
#include "ClassType.hpp"

using namespace FM;

FMString CodeType::brief(const Object &a) {
  return "code named " + this->ro(a)->m_name.description();
}

FMString CodeType::describe(const Object &a) {
  if (a.isScalar()) {
    const CodeData* cp = this->ro(a);
    FMString ret = "code object\n";
    ret += "   name      : " + cp->m_name.description() + "\n";
    ret += "   code      : " + cp->m_code.description() + "\n";
    ret += "   names     : " + cp->m_names.description() + "\n";
    ret += "   consts    : " + cp->m_consts.description() + "\n";
    ret += "   params    : " + cp->m_params.description() + "\n";
    ret += "   returns   : " + cp->m_returns.description() + "\n";
    ret += "   varargin  : " + cp->m_varargin.description() + "\n";
    ret += "   varargout : " + cp->m_varargout.description() + "\n";
    return ret;
  } else {
    return a.dims().toString() + " code array\n";
  }
}
