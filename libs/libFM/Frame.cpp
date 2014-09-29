#include "Frame.hpp"
#include "VM.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "DoubleType.hpp"

using namespace FM;

// Can returns and arguments avoid "allocate variable"?

bool Frame::defines(const FMString &name) 
{
  return (getAddress(name) != -1);
}

int Frame::getAddress(const FMString &name)
{
  const Object *cp = _ctxt->_list->readOnlyData(_sym_names);
  for (int i=0;i<_sym_names.elementCount();i++)
    if (_ctxt->_string->getString(cp[i]) == name) return i;
  return -1;
}

int Frame::allocateVariable(const FMString &name)
{
  int p = getAddress(name);
  if (p != -1) return p;
  p = _sym_names.elementCount();
  _ctxt->_list->push(_sym_names,_ctxt->_string->makeString(name));
  _ctxt->_list->push(_vars,_ctxt->_double->empty());
  return p;
}

void Frame::setVariableSlow(const FMString &name, const Object &value)
{
  int p = allocateVariable(name);
  Object *cp = _ctxt->_list->readWriteData(_vars);
  cp[p] = value;
}

Frame::Frame(ThreadContext *ctxt) : _sym_names(ctxt->_list->empty()), 
				    _vars(ctxt->_list->empty()), 
				    _addrs(ctxt->_index->empty())
{
  _ctxt = ctxt;
  //  _sym_names = _ctxt->_list->empty();
  //  _vars = _ctxt->_list->empty();
  //  _addrs = _ctxt->_index->empty();
  _closed = false;
  _reg_offset = 0;
}

