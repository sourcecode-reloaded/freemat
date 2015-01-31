#include "BuiltInType.hpp"
#include "ThreadContext.hpp"
#include "StringType.hpp"
#include "ListType.hpp"

using namespace FM;

FMString BuiltInType::brief(const Object &a) {
  return FMString("builtin " + _ctxt->_string->getString(this->ro(a)->m_name));
}

FMString BuiltInType::describe(const Object &a) {
  return this->brief(a);
}

void BuiltInType::setAddressAndName(Object &a, FMString name, fncptr b) {
  this->rw(a)->m_name = _ctxt->_string->makeString(name);
  this->rw(a)->m_ptr = b;
}

Object BuiltInType::getParens(const Object &a, const Object &b) 
{
  Object c = this->ro(a)->m_ptr(b,1,_ctxt);
  if (c.isEmpty()) return c;
  return _ctxt->_list->first(c);
}

Object BuiltInType::call(const Object &a, const Object &args, int nargout)
{
  return this->ro(a)->m_ptr(args,nargout,_ctxt);
}

Object BuiltInType::deref(const Object &a)
{
  Object args = _ctxt->_list->empty();
  Object ret = this->ro(a)->m_ptr(args,1,_ctxt);
  if (ret.count() >= 1)
    return _ctxt->_list->first(ret);
  return ret;
}

static Object _pass(const Object &args, int nargout, ThreadContext *ctxt) {
  return args;
}

Object BuiltInType::pass()
{
  return this->makeBuiltin("pass",_pass);
}

Object BuiltInType::makeBuiltin(FMString name, fncptr b)
{
  Object f = this->makeScalar();
  this->rw(f)->m_name = _ctxt->_string->makeString(name);
  this->rw(f)->m_ptr = b;
  return f;
}
