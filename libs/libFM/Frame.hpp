#ifndef __Frame_hpp__
#define __Frame_hpp__

#include "Object.hpp"
#include "FMLib.hpp"

namespace FM
{
  class Frame
  {
  public:
    FMString _name;
    std::vector<Object> _regfile;
    std::vector<Object> _vars;
    FMMap<FMString,int> _symtab;
    std::vector<int> _addr;
    bool _closed;
  public:
    Frame();
    bool defines(const FMString &name);
    Object getDynamicVar(const FMString &name);
    Object& getDynamicVarRef(const FMString &name);
    Object* getDynamicVarPtr(const FMString &name);
    int getAddress(const FMString &name);
    int allocateVariable(const FMString &name);
    void dump();
  };
};

#endif
