#ifndef __VM_hpp__
#define __VM_hpp__

#include "Object.hpp"
#include "Frame.hpp"
#include "Debug.hpp"
#include "HashMap.hpp"

#define VM_LOCALVAR 1
#define VM_DYNVAR 2

// A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end

namespace FM
{

  typedef std::pair<FMString,ObjectVector> assignment;
  
  struct ThreadContext;

  class VMDBQuitException {
  public:
    int _catchCount;
    VMDBQuitException(int p = 0) : _catchCount(p) {}
  };
  
  class VM
  {
    Object _registers;
    Object _modules;
    std::vector<Frame*> _frames;
    int _fp;
    int _rp;
    ThreadContext *_ctxt;
    Object _exception;
    int _try_depth;
    // Put these into a vector of flags?
    bool _retscrpt_found;
    Object _mybps;
    bool _debug_mode;
    int _debug_flags;
    void debugCycle();
    Frame* findPreviousClosedFrame(Frame *b);
    Frame* findNextClosedFrame(Frame *b);
    bool checkBreakpoints(Frame *frame, Frame *closed_frame, int ip);
    void updateDebugMode(bool onEntry);

    class FrameReserver {
      VM *_vm;
    public:
      FrameReserver(VM *vm);
      ~FrameReserver();
    };
  public:    
    enum class FrameType {openFrame, closedFrame};
    VM(ThreadContext *ctxt);
    void executeScript(const Object &codeObject, const FMString &debugname = "");
    Object executeFunction(const Object &functionObject, const Object &parameters, const FMString &debugname = "");
    Object executeAnonymousFunction(const Object &codeObject, const Object &parameters, const HashMap<Object> &captures);
    Object executeModule(const Object &moduleObject, const Object &parameters);
    void executeCodeObject(const Object &codeObject);
    void defineBaseVariable(const FMString &name, const Object &value);
    void defineClass(const Object &name, const Object &arguments);
    void defineFrame(const Object &names, int registerCount, const FrameTypeCode &ftype);
    void dump();
    Frame* activeFrame() {return _frames[_fp];}
    Object backtrace();
    void signalReturn() {_retscrpt_found = true;}
  };
}


#endif
