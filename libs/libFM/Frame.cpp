#include "Frame.hpp"
#include "VM.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "DoubleType.hpp"
#include "ModuleType.hpp"

using namespace FM;

// Can returns and arguments avoid "allocate variable"?

bool Frame::defines(const FMString &name) 
{
  return (getAddress(name) != -1);
}

int Frame::mapNameToVariableIndex(const Object &name) {
  const Object *cp = _ctxt->_list->ro(_sym_names);
  for (int i=0;i<_sym_names.count();i++)
    if (cp[i] == name) return i;
  return -1;
}

int Frame::lookupAddressForName(const Object &name, bool searchGlobals) {
  // First lookup the address in the list of symbols
  int ndx = this->mapNameToVariableIndex(name);
  if ((ndx == -1) && searchGlobals)
    {
      // Do not know the name at all
      // First look in the module
      if (!_module.isEmpty())
	{
	  std::cout << "Searching module for " << _ctxt->_string->getString(name) << "\n";
	  auto mfunc = _ctxt->_module->ro(_module)->m_locals.find(name);
	  if (mfunc != _ctxt->_module->ro(_module)->m_locals.end())
	    {
	      ndx = _sym_names.count();
	      _ctxt->_list->push(_sym_names,name);
	      _ctxt->_list->push(_vars,mfunc->second);
	      return ndx;
	    }
	}
      // Is it defined in the global scope?
      std::cout << "Searching globals for " << _ctxt->_string->getString(name) << "\n";
      auto gfunc = _ctxt->_globals->find(_ctxt->_string->getString(name)); //TODO Remove conversion to string?
      if (gfunc != _ctxt->_globals->end())
	{
	  // We are a proxy for someone who wants this symbol (not us!)
	  // Allocate a space for it
	  ndx = _sym_names.count();
	  _ctxt->_list->push(_sym_names,name);
	  _ctxt->_list->push(_vars,gfunc->second);
	  // We don't need to cache the address for this symbol
	  return ndx;
	}
      return -1;
    }
  // If the symbol is defined, cache it's address
  // This is really a cue that the symbol has been
  // searched before.  The VM will then skip the lookup
  // step for this symbol (unless the scope changes).
  const ndx_t *addrs = _ctxt->_index->ro(_addrs);
  if ((ndx != -1) && searchGlobals && (addrs[ndx] == -1))
    {
      // We have a symbol with the given name, but
      // have not defined it yet.  This could be because
      // of an error (i.e., an undefined symbol), or
      // because the definition is in the global space.
      // Check for the latter case.
      if (!_module.isEmpty())
	{
	  std::cout << "Searching module for " << _ctxt->_string->getString(name) << "\n";
	  auto mfunc = _ctxt->_module->ro(_module)->m_locals.find(name);
	  if (mfunc != _ctxt->_module->ro(_module)->m_locals.end())
	    {
	      _ctxt->_list->rw(_vars)[ndx] = mfunc->second;
	      _ctxt->_index->rw(_addrs)[ndx] = ndx;
	      return ndx;
	    }      
	}
      std::cout << "Searching globals for named symbol " << _ctxt->_string->getString(name) << "\n";
      auto gfunc = _ctxt->_globals->find(_ctxt->_string->getString(name)); //TODO Remove conversion to string?
      if (gfunc != _ctxt->_globals->end())
	{
	  _ctxt->_list->rw(_vars)[ndx] = gfunc->second;
	  _ctxt->_index->rw(_addrs)[ndx] = ndx;
	  return ndx;
	}
      return -1;
    }
  return ndx;
}

int Frame::getAddress(const FMString &name)
{
  const Object *cp = _ctxt->_list->ro(_sym_names);
  for (int i=0;i<_sym_names.count();i++)
    if (_ctxt->_string->getString(cp[i]) == name) return i;
  return -1;
}

int Frame::allocateVariable(const FMString &name)
{
  int p = getAddress(name);
  if (p != -1) return p;
  p = _sym_names.count();
  _ctxt->_list->push(_sym_names,_ctxt->_string->makeString(name));
  _ctxt->_list->push(_vars,_ctxt->_double->empty());
  return p;
}

int Frame::defineNewSymbol(const Object &name)
{
  int p = _sym_names.count();
  _ctxt->_list->push(_sym_names,name);
  _ctxt->_list->push(_vars,_ctxt->_double->empty());
  return p;
}

void Frame::setVariableSlow(const FMString &name, const Object &value)
{
  int p = allocateVariable(name);
  Object *cp = _ctxt->_list->rw(_vars);
  cp[p] = value;
}

Frame::Frame(ThreadContext *ctxt) : _sym_names(ctxt->_list->empty()), 
				    _vars(ctxt->_list->empty()), 
				    _addrs(ctxt->_index->empty()),
				    _defined(ctxt->_bool->empty()),
				    _module(ctxt->_module->empty()),
				    _captures(ctxt->_list->empty()),
				    _obj(ctxt->_double->empty())
{
  _ctxt = ctxt;
  //  _sym_names = _ctxt->_list->empty();
  //  _vars = _ctxt->_list->empty();
  //  _addrs = _ctxt->_index->empty();
  _closed = false;
  _reg_offset = 0;
}

