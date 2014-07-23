#include "Frame.hpp"
#include "VM.hpp"

using namespace FM;

bool Frame::defines(const FMString &name) 
{
  return _symtab.contains(name);
}

Object Frame::getDynamicVar(const FMString &name)
{
  if (!_symtab.contains(name)) throw Exception("Variable " + name + " undefined!");
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return _localvars[flags >> 16];
  else
    return _dynvars[flags >> 16];
}

Object& Frame::getDynamicVarRef(const FMString &name)
{
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
