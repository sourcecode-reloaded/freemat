#include "Frame.hpp"
#include "VM.hpp"

using namespace FM;



bool Frame::defines(const FMString &name) 
{
  return _symtab.contains(name);
}

// int Frame::getDynamicAddress(const FMString &name)
// {
// }

Object Frame::getDynamicVar(const FMString &name)
{
  /*
  if (!_symtab.contains(name)) throw Exception("Variable " + name + " undefined!");
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return _localvars[flags >> 16];
  else
    return _dynvars[flags >> 16];
  */
}

Object* Frame::getDynamicVarPtr(const FMString &name)
{
  /*
  if (!_symtab.contains(name)) throw Exception("Variable " + name + " undefined!");
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return &_localvars[flags >> 16];
  else
    return &_dynvars[flags >> 16];
  */
}

Object& Frame::getDynamicVarRef(const FMString &name)
{
  /*
  if (!_symtab.contains(name))
    {
      _symtab[name] = VM_DYNVAR | (_dynvars.size() << 16);
      _dynvars.push_back(Object());
      return _dynvars.back();
    }
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return _localvars[flags >> 16];
  return _dynvars[flags >> 16];
  */
}

int Frame::getAddress(const FMString &name)
{
  //  std::cout << "Get Address of " << name << "\n";
  if (_symtab.contains(name)) 
    {
      //      std::cout << "Returning address " << _symtab.value(name) << "\n";
      return _symtab.value(name);
    }
  return 0;
}

int Frame::allocateVariable(const FMString &name)
{
  if (_symtab.contains(name)) return _symtab.value(name);
  int addr = _vars.size() + 1;
  //  std::cout << "Allocating variable named " << name << " with address " << addr <<"\n";
  _symtab.insert(name,addr);
  _vars.push_back(Object());
  return addr;
}

Frame::Frame()
{
}

void Frame::dump()
{
  std::cout << "Frame: " << _name << "\n";
  std::cout << "  Dynamic vars\n";
  for (FMMap<FMString,int>::const_iterator i=_symtab.constBegin();
       i!=_symtab.constEnd();++i)
    std::cout << "    " << i.key() << " : " << getDynamicVar(i.key()).description() << "\n";
}
