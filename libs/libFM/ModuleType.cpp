#include "ModuleType.hpp"
#include "CodeType.hpp"
#include "VM.hpp"
#include "AllTypes.hpp"

using namespace FM;

Object ModuleType::getParens(const Object &a, const Object &b) 
{
  Object p = _ctxt->_vm->executeModule(a,b);
  if (p.isEmpty()) return p;
  return _ctxt->_list->first(p);
}

Object ModuleType::call(const Object &a, const Object &args, int nargout)
{
  return _ctxt->_vm->executeModule(a,args);
}

Object ModuleType::deref(const Object &a)
{
  Object args = _ctxt->_list->empty();
  return _ctxt->_list->first(this->call(a,args,1));
}

