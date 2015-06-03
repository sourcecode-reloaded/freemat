#include "FunctionHandleType.hpp"
#include "VM.hpp"
#include "AllTypes.hpp"

using namespace FM;

FMString FunctionHandleType::brief(const Object &a) {
  return "function handle for function " + this->ro(a)->m_func.description();
}

FMString FunctionHandleType::describe(const Object &a) {
  if (a.isScalar()) {
    const FunctionHandleData* cp = this->ro(a);
    FMString ret = "function handle\n";
    ret += "   isclassmethod : " + Stringify(cp->m_classMethod) + "\n";
    ret += "   function      : " + cp->m_func.description() + "\n";
    ret += "   class         : " + cp->m_class.description() + "\n";
    return ret;
  } else {
    return a.dims().toString() + " function array\n";
  }
}

Object FunctionHandleType::getParens(const Object &a, const Object &b) 
{
  const FunctionHandleData *fhd = this->ro(a);
  return fhd->m_func.type()->getParens(fhd->m_func,b);
}

Object FunctionHandleType::call(const Object &a, const Object &args, ndx_t nargout)
{
  const FunctionHandleData *fhd = this->ro(a);
  return fhd->m_func.type()->call(fhd->m_func,args,nargout);
}

Object FunctionHandleType::fromFunction(const Object &func)
{
  Object f = this->makeScalar();
  this->rw(f)->m_classMethod = false;
  this->rw(f)->m_func = func;
  return f;
}
