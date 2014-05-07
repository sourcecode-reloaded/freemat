#ifndef __VM_hpp__
#define __VM_hpp__

#include "Array.hpp"
#include "Code.hpp"

typedef std::pair<FMString,ArrayVector> assignment;

// A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end


class Frame
{
public:
  FMString _name;
  std::vector<Array> _regfile;
  std::vector<Array> _localvars;
  std::vector<Array> _localnames;
  std::vector<bool> _dynflags;
  std::vector<Array> _dynvars;
  FMMap<FMString,int> _symtab;
  bool _closed;
public:
  Frame();
  bool defines(const FMString &name);
  Array getDynamicVar(const FMString &name);
  Array& getDynamicVarRef(const FMString &name);
};

// This can be split out later...
class VM
{
  std::vector<Array> _stack;
  std::vector<Frame*> _frames;
  int _sp;
private:
  void doSubsasgnOp( Array &ref, const Array &b);
  void doSubsrefOp( Array & ref);
  void popVector(ArrayVector &x);
  void decodeAssignments(std::vector<assignment> & assignments);
  void dump();
  Array deref(const assignment&, const Array &);
  Array assign(Array A, const assignment & p, const Array &b);
  Array doAssign(Array A, const std::vector<assignment> & assignments, const Array &b);
public:
  VM();
  void executeBlock(const Array &codeObject, bool singleStep = false);
  void executeScript(const Array &codeObject);
  void executeCodeObject(const Array &codeObject);
};


#endif
