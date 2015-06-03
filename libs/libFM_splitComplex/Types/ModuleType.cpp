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

Object ModuleType::call(const Object &a, const Object &args, ndx_t nargout)
{
  return _ctxt->_vm->executeModule(a,args);
}

Object ModuleType::deref(const Object &a)
{
  Object args = _ctxt->_list->empty();
  Object ret = this->call(a,args,1);
  if (ret.isEmpty())
    return _ctxt->_double->empty();
  return _ctxt->_list->first(ret);
}

Object ModuleType::builtin(const FMString &name, fncptr addr)
{
  Object b = this->makeScalar();
  ModuleData *md = this->rw(b);
  md->m_name = name;
  md->m_modtype = BuiltinModuleType;
  md->m_ptr = addr;
  return b;
}

static Object _pass(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return args;
}

Object ModuleType::pass()
{
  return this->builtin("pass",_pass);  
}
