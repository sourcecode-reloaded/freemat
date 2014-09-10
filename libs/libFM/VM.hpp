#ifndef __VM_hpp__
#define __VM_hpp__

#include "Object.hpp"
#include "Code.hpp"
#include "Frame.hpp"

#define VM_LOCALVAR 1
#define VM_DYNVAR 2

// A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end

namespace FM
{

  typedef std::pair<FMString,ObjectVector> assignment;
  
  struct ThreadContext;

  class VM
  {
    std::vector<Object> _registers;
    std::vector<Frame*> _frames;
    int _fp;
    int _rp;
    ThreadContext *_ctxt;
  public:
    VM(ThreadContext *ctxt);
    void executeBlock(const Object &codeObject, bool singleStep = false);
    void executeScript(const Object &codeObject);
    Object executeFunction(const Object &codeObject, const Object &parameters);
    void executeCodeObject(const Object &codeObject);
    void defineBaseVariable(const FMString &name, const Object &value);
    void dump();
  };
}


#endif
