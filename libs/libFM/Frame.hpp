#ifndef __Frame_hpp__
#define __Frame_hpp__

#include "Object.hpp"
#include "FMLib.hpp"
#include "ThreadContext.hpp"

namespace FM
{
  class Frame
  {
  public:
    FMString _name;
    FMString _debugname;
    Object _sym_names;
    Object _vars;
    Object _addrs;
    Object _captures;
    Object _defined;
    Object _module;
    Object _code;
    Frame *_closedFrame;
    std::vector<uint16_t> _debug_line_nos;
    std::vector<bool> _debug_flag;
    std::vector<int> _exception_handlers;
    ThreadContext *_ctxt;
    bool _closed;
    int _reg_offset;
    int _fp;
  public:
    Frame(ThreadContext *ctxt);
    int mapNameToVariableIndex(const Object &name);
    bool defines(const FMString &name);
    int getAddress(const FMString &name);
    int allocateVariable(const FMString &name);
    void setVariableSlow(const FMString &name, const Object &value);
    bool getLocalVariableSlow(const FMString &name, Object &value);
    int lookupAddressForName(const Object &name, bool searchGlobals);
    int defineNewSymbol(const Object &name);
  };
};

#endif
