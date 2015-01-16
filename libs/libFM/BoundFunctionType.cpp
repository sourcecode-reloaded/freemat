#include "BoundFunctionType.hpp"
#include "ListType.hpp"
#include "FunctionType.hpp"

using namespace FM;

Object BoundFunctionType::bindFunction(const Object &func, const Object &arg) {
  Object a = this->empty();
  this->rw(a)->m_func = func;
  this->rw(a)->m_arg = arg;
  return a;
}

FMString BoundFunctionType::describe(const Object &a) {
  return "Bound function: " + this->ro(a)->m_func.description() + " with argument " + 
    this->ro(a)->m_arg.description();
}

FMString BoundFunctionType::brief(const Object &a) {
  return "Bound function: " + this->ro(a)->m_func.brief() + " with argument " + 
    this->ro(a)->m_arg.brief();
}

Object BoundFunctionType::getParens(const Object &a, const Object &b) {
  Object args = _ctxt->_list->makeScalar(this->ro(a)->m_arg);
  _ctxt->_list->merge(args,b);
  return this->ro(a)->m_func.type()->getParens(this->ro(a)->m_func,args);
}

Object BoundFunctionType::call(const Object &a, const Object &args, int nargout) {
  Object all_args = _ctxt->_list->makeScalar(this->ro(a)->m_arg);
  _ctxt->_list->merge(all_args,args);
  return this->ro(a)->m_func.type()->call(this->ro(a)->m_func,all_args,nargout);
}

Object BoundFunctionType::deref(const Object &a) {
  return this->ro(a)->m_func.type()->getParens(this->ro(a)->m_func,
					       _ctxt->_list->makeScalar(this->ro(a)->m_arg));
}
