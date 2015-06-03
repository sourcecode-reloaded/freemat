#ifndef __Frame_hpp__
#define __Frame_hpp__

#include "Object.hpp"
#include "FMLib.hpp"
#include "ThreadContext.hpp"

namespace FM
{

  enum class FrameTypeCode {Function, Script, Builtin, Debug};
  enum class FrameRunStateCode {Normal, Debug, StepIn, StepOut};
  
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
    Frame *_prevFrame;
    Frame *_nextFrame;
    std::vector<uint16_t> _debug_line_nos;
    uint16_t _transient_bp;
    std::vector<int> _exception_handlers;
    ThreadContext *_ctxt;
    bool _closed;
    int _reg_offset;
    int _ip;
    FrameTypeCode _type;
    FrameRunStateCode _state;
    int _stepin_lineno_trap;
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
    int mapIPToLineNumber(int ip);
    bool isDebuggable() {
      return ((_type == FrameTypeCode::Function) ||
	      (_type == FrameTypeCode::Script));
    }
  };
};

#endif
