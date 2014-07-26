#include "Frame.hpp"
#include "VM.hpp"

using namespace FM;


// What is a dynamic variable?  Consider the following function
//
// function foo
//    A = fft(1:10);
//
// In this case, we might think that "fft" is an external (global) symbol.  But 
// not necessarily.  Consider the case of an eval statement:
//
// function foo
//    eval('fft = 1:10');
//    A = fft(1:10);
//
// The presence of an eval means that "fft" is now a variable.  So when we lookup
// fft, we get a variable instead of the function.  The "eval" could just as
// easily be hidden in a script
//
// function foo
//    do_script
//    A = fft(1:10);
//
// do_script
//   fft = 1:10
//
// So dynamic variables are ones that just ones that have to be defined at runtime.
//
// Note that there is a problem in the current definition of local variables.  Consider
//
// function foo
//    a = 3;
//    a = a + 5;
//
// In this case, because "a" is not visible outside the scope of function "foo", it 
// would be marked a local variable (it is defined before being used).  But what happens
// if the variable is deleted?  Something like this:
//
// function foo
//    fft = 3;
//    clear('fft');
//    a = fft(1:15);
//
// The "clear" function will delete the local variable "fft", and the next call to "fft"
// will be mapped to the function "fft", not the local variable.  Yuck.  The presence of
// "clear" means that "local" variables are an optimization that we cannot really make.
// Instead, all variables that are not declared as global, persistent or defined as 
// parameters are dynamic.
//
// Instead of trying to do a bunch of complex analysis at compile time, I should have developed
// a better cache mechanism so that variables could be looked up without a big penalty.  
// Double yuck.
//


bool Frame::defines(const FMString &name) 
{
  return _symtab.contains(name);
}

int Frame::getDynamicAddress(const FMString &name)
{
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

Object* Frame::getDynamicVarPtr(const FMString &name)
{
  if (!_symtab.contains(name)) throw Exception("Variable " + name + " undefined!");
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return &_localvars[flags >> 16];
  else
    return &_dynvars[flags >> 16];
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
