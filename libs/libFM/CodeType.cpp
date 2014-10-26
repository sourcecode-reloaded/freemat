#include "CodeType.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "ListType.hpp"

using namespace FM;

CodeData* CodeType::makeEmptyDataType() {
  return(new CodeData(_ctxt));
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

Object CodeType::bindFunction(const Object &func, const Object &argument)
{
  /*
  // Create a code object that binds a function to an argument
  Object code = this->empty();
  CodeData *cp = this->readWriteData(code);
  const CodeData *fp = this->readOnlyData(func);
  cp->name = _ctxt->_string->makeString("Binding of " + func.description() + " to " + argument.description());
  // There are two constants... the code object (0) and the argument (1)
  cp->m_consts = _ctxt->_list->empty();
  _ctxt->_list->push(cp->m_consts,code);
  _ctxt->_list->push(cp->m_consts,argument);
  // For the arguments, we have to copy the arguments of the func block, and delete the first argument
  cp->m_params = _ctxt->_list->empty();
  cp->m_names = _ctxt->_list->empty();
  const Object *fp_names = _ctxt->_list->readOnlyData(fp->m_names);
  const Object *fp_params = _ctxt->_list->readOnlyData(fp->m_params);
  for (int i=1;i<fp->m_params.elementCount();++i) {
    addIndexToList(_ctxt,cp->m_params,i-1);
    _ctxt->_list->push(cp->m_names,fp->names[fp_params
  }
  */
  return this->empty();
}
