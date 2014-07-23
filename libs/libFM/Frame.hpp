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
    std::vector<Object> _localvars;
    std::vector<Object> _localnames;
    std::vector<bool> _dynflags;
    std::vector<Object> _dynvars;
    FMMap<FMString,int> _symtab;
    bool _closed;
  public:
    Frame();
    bool defines(const FMString &name);
    Object getDynamicVar(const FMString &name);
    Object& getDynamicVarRef(const FMString &name);
    void dump();
  };
};

#endif
