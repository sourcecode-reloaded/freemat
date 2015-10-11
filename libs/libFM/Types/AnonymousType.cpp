#include "AnonymousType.hpp"
#include "AllTypes.hpp"
#include "VM.hpp"

using namespace FM;

FMString AnonymousType::brief(const Object &a) {
  return "anonymous function " + this->ro(a)->m_printable.description();
}

FMString AnonymousType::describe(const Object &a) {
  if (a.isScalar())
    return this->brief(a);
  else
    return a.dims().toString() + " anonymous function array\n";
}

Object AnonymousType::getParens(const Object &a, const Object &b) 
{
  const AnonymousData *ad = this->ro(a);
  Object p = _ctxt->_vm->executeAnonymousFunction(ad->m_code,b,ad->m_captured);
  if (p.isEmpty()) return p;
  return _ctxt->_list->first(p);
}

Object AnonymousType::call(const Object &a, const Object &args, ndx_t)
{
  const AnonymousData *ad = this->ro(a);
  return _ctxt->_vm->executeAnonymousFunction(ad->m_code,args,ad->m_captured);
}

Object AnonymousType::build(const Object &description, const Object &codeblock, Frame *frame) 
{
  Object f = this->makeScalar();
  AnonymousData *ad = this->rw(f);
  ad->m_code = codeblock;
  ad->m_printable = description;
  const CodeData *cd = _ctxt->_code->ro(codeblock);
  const Object *np = _ctxt->_list->ro(cd->m_names);
  const Object *vars = _ctxt->_list->ro(frame->_vars);
  for (auto i=0;i<cd->m_names.count();i++) {
    auto addr = frame->lookupAddressForName(np[i],false);
    if (addr != -1) {
      std::cout << "Capturing variable " << np[i].description() << "\n";
      ad->m_captured.insert(std::make_pair(np[i],vars[addr]));
    }
  }
  return f;
}

