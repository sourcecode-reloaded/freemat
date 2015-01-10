#include "FunctionType.hpp"
#include "VM.hpp"
#include "AllTypes.hpp"

using namespace FM;


FMString FunctionType::brief(const Object &a) {
  return "function named " + this->ro(a)->m_name.description();
}

FMString FunctionType::describe(const Object &a) {
  if (a.isScalar()) {
    const FunctionData* cp = this->ro(a);
    FMString ret = "function object\n";
    ret += "   name      : " + cp->m_name.description() + "\n";
    ret += "   code      : " + cp->m_code.description() + "\n";
    ret += "   closure   : " + cp->m_closure.description() + "\n";
    return ret;
  } else {
    return a.dims().toString() + " function array\n";
  }
}

Object FunctionType::methodCall(const Object & func, const Object &obj, const Object &args)
{
  // FIXME - move this up and remove methodCall
  Object olist = _ctxt->_list->makeScalar(obj);
  _ctxt->_list->merge(olist,args);
  Object p = _ctxt->_vm->executeFunction(func,olist);
  if (p.isEmpty()) return p;
  return _ctxt->_list->first(p);
}

Object FunctionType::getParens(const Object &a, const Object &b) 
{
  Object p = _ctxt->_vm->executeFunction(a,b);
  if (p.isEmpty()) return p;
  return _ctxt->_list->first(p);
}

Object FunctionType::call(const Object &a, const Object &args, int nargout)
{
  return _ctxt->_vm->executeFunction(a,args);
}

Object FunctionType::deref(const Object &a)
{
  Object args = _ctxt->_list->empty();
  Object ret = this->call(a,args,1);
  if (ret.count() >= 1)
    return _ctxt->_list->first(ret);
  return ret;
}

Object FunctionType::fromCode(const Object &codeObject)
{
  return fromCode(codeObject,_ctxt->_list->empty());
}

Object FunctionType::fromCode(const Object &codeObject, const Object &closure)
{
  assert(codeObject.is(TypeCode));
  assert(closure.isList());
  Object f = this->makeScalar();
  this->rw(f)->m_code = codeObject;
  this->rw(f)->m_name = _ctxt->_code->ro(codeObject)->m_name;
  this->rw(f)->m_closure = closure;
  return f;
}
