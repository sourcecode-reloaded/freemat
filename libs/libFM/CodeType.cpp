#include "CodeType.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "ListType.hpp"

using namespace FM;

Object CodeType::makeCodeObject() {
  Data *q = new Data;
  q->refcnt = 0;
  CodeData *cd = new CodeData(_ctxt);
  q->ptr = cd;
  ObjectBase *p = new ObjectBase(q,this,0,Tuple(1,1),0,1);
  return Object(p);
}

FMString CodeType::describe(const Object &a) {
  if (a.isScalar()) {
    const CodeData* cp = this->readOnlyData(a);
    FMString ret = "code object\n";
    ret += "   name: " + cp->m_name.description() + "\n";
    ret += "   code: " + cp->m_code.description() + "\n";
    ret += "   names: " + cp->m_names.description() + "\n";
    ret += "   consts: " + cp->m_consts.description() + "\n";
    ret += "   params: " + cp->m_params.description() + "\n";
    ret += "   returns: " + cp->m_returns.description() + "\n";
    return ret;
  } else {
    return a.dims().toString() + " code array\n";
  }
}

Object CodeType::getParens(const Object &a, const Object &b) 
{
  Object p = _ctxt->_vm->executeFunction(a,b);
  if (p.isEmpty()) return p;
  return _ctxt->_list->first(p);
}

Object CodeType::call(const Object &a, const Object &args, int nargout)
{
  return _ctxt->_vm->executeFunction(a,args);
}

Object CodeType::deref(const Object &a)
{
  Object args = _ctxt->_list->empty();
  return _ctxt->_list->first(this->call(a,args,1));
}
