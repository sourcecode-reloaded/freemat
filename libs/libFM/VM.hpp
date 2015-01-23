#ifndef __VM_hpp__
#define __VM_hpp__

#include "Object.hpp"
#include "Frame.hpp"
#include "HashMap.hpp"

#define VM_LOCALVAR 1
#define VM_DYNVAR 2

// A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end

namespace FM
{

  typedef std::pair<FMString,ObjectVector> assignment;
  
  struct ThreadContext;

  class VM
  {
    Object _registers;
    Object _modules;
    std::vector<Frame*> _frames;
    int _fp;
    int _rp;
    ThreadContext *_ctxt;
    Object _exception;
  public:
    VM(ThreadContext *ctxt);
    void executeScript(const Object &codeObject);
    Object executeFunction(const Object &functionObject, const Object &parameters);
    Object executeAnonymousFunction(const Object &codeObject, const Object &parameters, const HashMap<Object> &captures);
    Object executeModule(const Object &moduleObject, const Object &parameters);
    void executeCodeObject(const Object &codeObject);
    void defineBaseVariable(const FMString &name, const Object &value);
    void defineClass(const Object &name, const Object &arguments);
    void defineFrame(const Object &names, int registerCount);
    void dump();
  };
}


#endif
