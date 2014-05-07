#ifndef __VM_hpp__
#define __VM_hpp__

#include "Object.hpp"
#include "Code.hpp"
#include "Frame.hpp"
#include "BaseTypes.hpp"

#define VM_LOCALVAR 1
#define VM_DYNVAR 2

// A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end

namespace FM
{

  typedef std::pair<FMString,ObjectVector> assignment;

  class VM
  {
    std::vector<Object> _stack;
    std::vector<Frame*> _frames;
    int _sp;
    BaseTypes *_types;
  private:
    void doSubsasgnOp( Object &ref, const Object &b);
    void doSubsrefOp( Object & ref);
    void popVector(ObjectVector &x);
    void decodeAssignments(std::vector<assignment> & assignments);
    void dump();
    Object deref(const assignment&, const Object &);
    Object assign(Object A, const assignment & p, const Object &b);
    Object doAssign(Object A, const std::vector<assignment> & assignments, const Object &b);
  public:
    VM(BaseTypes *types);
    void executeBlock(const Object &codeObject, bool singleStep = false);
    void executeScript(const Object &codeObject);
    void executeCodeObject(const Object &codeObject);
  };
}


#endif
