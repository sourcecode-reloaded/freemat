#include "VM.hpp"
#include "Print.hpp"
#include "Algorithms.hpp"
#include "Struct.hpp"
#include "Math.hpp"
#include "TermIF.hpp"
#include <boost/timer/timer.hpp>

#define opcode(x) ((x) & 0xFF)
#define reg1(x) (((x) >> 8) & 0xFF)
#define reg2(x) (((x) >> 16) & 0xFF)
#define reg3(x) (((x) >> 24) & 0xFF)
#define constant(x) (((x) >> 16) & 0xFFFF)

#define VM_LOCALVAR 1
#define VM_DYNVAR 2

//A = 0; A(100000,1) = 0; for i= 1:1:100000; A(i) = i; end

//A = []; A(512,512) = 0; for i=1:1:512; for j=1:1:512; A(i,j) = i-j; end; end


// Calling a function
// The code has pushed the arguments to the stack
//
//   BeginArgVector
//     Arg0
//     Arg1
//      ...
//     ArgN
//   EndArgVector
//   Nargout
//
// Python avoids an extra copy by just leaving the arguments on
// the stack and allocating the local variables on top.
// Return values are pushed onto the stack also.  
//
// To do something similar is probably impractical, because we 
// allow for variable numbers of input and output arguments.
//
// So the process for establishing a frame should be:
//  popVector --> argvec
//  popNargout --> nargout
// Allocate the local vars
// Copy arguments from argvec --> localvars
// Execute the code
//  push the arguments back onto the stack



// What happens if the type of a register isn't Array?
// 

bool Frame::defines(const FMString &name) 
{
  return _symtab.contains(name);
}

Array Frame::getDynamicVar(const FMString &name)
{
  if (!_symtab.contains(name)) throw Exception("Variable " + name + " undefined!");
  int flags = _symtab[name];
  if (flags & VM_LOCALVAR)
    return _localvars[flags >> 16];
  else
    return _dynvars[flags >> 16];
}

Array& Frame::getDynamicVarRef(const FMString &name)
{
  if (!_symtab.contains(name))
    {
      _symtab[name] = VM_DYNVAR | (_dynvars.size() << 16);
      _dynvars.push_back(Array());
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

VM::VM()
{
  _stack.resize(1024);
  _sp = 0;
  Frame *f = new Frame;
  f->_name = "base";
  f->_closed = true;
  _frames.push_back(f);
}

void VM::dump()
{
  /*
  std::cout << "Stack:" << "\n";
  for (int i=0;i<_sp;i++)
    {
      printf("%03d ",i); 
      std::cout << SummarizeArrayCellEntry(_stack[i]) << "\n";
    }
  std::cout << "Vars:" << "\n";
  for (int i=0;i<_varlist.size();i++)
    {
      printf("%-7s  ",_varlist[i].c_str());
      std::cout << SummarizeArrayCellEntry(_vars[i]) << "\n";
    }
  std::cout << "Regs:" << "\n";
  for (int i=0;i<10;i++)
    {
      std::cout << "r" << i << " = " << SummarizeArrayCellEntry(_regfile[i]) << "  ";
    }
  std::cout << "\n";
  */
}

#define REG1 f->_regfile[reg1(insn)]
#define REG2 f->_regfile[reg2(insn)]
#define REG3 f->_regfile[reg3(insn)]

#define UNARYOP(fnc,funcname)				\
  if (!(REG2.isUserClass()))				\
    REG1 = fnc(REG2); 					\
  else							\
    printf("unhandled class op!"); //REG1 = ClassUnaryOperator(REG2) ;

#define BINOP(fnc,funcname) 				\
  if (!(REG2.isUserClass() || REG3.isUserClass())) 	\
    REG1 = fnc(REG2,REG3);				\
  else							\
    printf("Unhandled class op!"); //REG1 = ClassBinaryOperator(REG2,REG3,funcname);	\ 


void VM::decodeAssignments(std::vector<assignment> &assignments)
{
  int32_t argptr = _stack[_sp-1].asInteger()-1;
  int32_t arg_len = argptr+1;
  while (argptr > 0)
    {
      assignment x;
      int32_t cnt = _stack[argptr].asInteger();
      for (int i=0;i<cnt;i++)
	x.second.push_back(_stack[argptr-cnt+i]);
      x.first = _stack[argptr-cnt-1].asString();
      argptr -= (cnt+2);
      assignments.push_back(x);
    }
  _sp -= (arg_len+1);
}

void VM::popVector(ArrayVector &x)
{
  int32_t cnt = _stack[_sp-1].asInteger();
  for (int i=0;i<cnt;i++)
    x.push_back(_stack[_sp-1-cnt+i]);
  _sp -= (cnt+1);
}

Array VM::deref(const assignment& p, const Array &var )
{
  if (p.first == "()")
    return var.get(p.second);
  else if (p.first == "{}")
    return ArrayFromCellArray(var.get(p.second));
  else if (p.first == ".")
    {
      ArrayVector v = var.get(p.second[0].asString());
      if (v.size() > 1) throw Exception("Illegal expression");
      return v[0];
    }
}

// A possible refinement here - we can check for user-defined classes in the loop 
// and dispatch as subsref calls.  That allows a.foo(32) to be dispatched correctly
// if a.foo is a userclass.
void VM::doSubsrefOp( Array & ref)
{
  // First, we rebuild the assignment instruction
  std::vector<assignment> assignments;
  decodeAssignments(assignments);
  /*
  for (int i=0;i<assignments.size();i++)
    {
      std::cout << "Assignment : " << i << "\n";
      std::cout << "  Type : " << assignments[i].first << "\n";
      std::cout << "  Params : < ";
      for (int j=0;j<assignments[i].second.size();j++) {
	std::cout << SummarizeArrayCellEntry(assignments[i].second[j]) << " ";
      }
      std::cout << ">\n";
    }
  */
  Array var = ref;
  // Loop through the assignments
  for (int i=assignments.size()-1;i>=1;--i)
    var = deref(assignments[i],var);
  assignment &p = assignments[0];
  if (p.first == "()")
    _stack[_sp++] = var.get(p.second);
  else if (p.first == "{}")
    {
      ArrayVector q = ArrayVectorFromCellArray(var.get(p.second));
      for (int i=0;i<q.size();i++)
	_stack[_sp++] = q[i];
    }
  else
    {
      ArrayVector q = var.get(p.second[0].asString());
      for (int i=0;i<q.size();i++)
	_stack[_sp++] = q[i];
    }
}

Array VM::assign(Array A, const assignment & p, const Array &b)
{
  if (p.first == "()")
    A.set(p.second,b);
  else if (p.first == "{}")
    {
      ArrayVector c(b);
      SetCellContents(A,p.second,c);
    }
  else
    {
      ArrayVector c(b);
      A.set(p.second[0].asString(),c);
    }
  return A;
}

Array VM::doAssign(Array A, const std::vector<assignment> &assignments, const Array &b)
{
  if (assignments.size() == 1) 
    return assign(A,assignments[0],b);
  std::vector<assignment> copy_assignments(assignments);
  copy_assignments.pop_back();
  Array Asub;
  try
    {
      Asub = deref(assignments.back(),A);
    }
  catch (Exception &e)
    {
      Asub = EmptyConstructor();
    }
  return assign(A,assignments.back(),doAssign(Asub,copy_assignments,b));
}

// This would be so much cleaner if get could return a reference
// to an element in the array - but it cannot, except for an 
// array of arrays.
void VM::doSubsasgnOp(Array &ref, const Array & b)
{
  // First, we rebuild the assignment instruction
  std::vector<assignment> assignments;
  decodeAssignments(assignments);
  // for (int i=0;i<assignments.size();i++)
  //   {
  //     std::cout << "Assignment : " << i << "\n";
  //     std::cout << "  Type : " << assignments[i].first << "\n";
  //     std::cout << "  Params : < ";
  //     for (int j=0;j<assignments[i].second.size();j++) {
  // 	std::cout << SummarizeArrayCellEntry(assignments[i].second[j]) << " ";
  //     }
  //     std::cout << ">\n";
  //   }
  ref = doAssign(ref, assignments, b);
}


void VM::executeScript(const Array &codeObject)
{
  Frame *f = new Frame;
  const StructArray &qp = codeObject.constStructPtr();
  f->_name = qp["name"][1].asString();
  f->_regfile.resize(256);
  f->_localvars.resize(qp["vars"][1].length());
  f->_dynflags.resize(qp["names"][1].length());
  const BasicArray<Array>& vars(qp["vars"][1].constReal<Array>());
  const Array *varlist = vars.constData();
  for (int i=0;i<vars.length();i++)
    {
      FMString varname = varlist[i].asString();
      f->_symtab[varname] = VM_LOCALVAR | (i << 16);
    }
  f->_closed = false;
  _frames.push_back(f);
  // execute the code
  executeCodeObject(codeObject);
  // No return values.. pop the frame
  _frames.pop_back();
  delete f;
}


extern "C"
double num_for_loop_iter( double first, double step, double last );

// Dynamic load/stores - these behave differently 
// depending on if we are a script or not.  If we are
// a script, a dynamic load/store can see the calling
// scopes - up until we encounter a closed scope.
void VM::executeCodeObject(const Array &codeObject)
{
  const StructArray &qp = codeObject.constStructPtr();
  const BasicArray<Array>& consts(qp["consts"][1].constReal<Array>());
  const BasicArray<uint32_t>& opcodes(qp["code"][1].constReal<uint32_t>());
  const BasicArray<Array>& names(qp["names"][1].constReal<Array>());

  std::vector<double> etimes;
  etimes.resize(opcodes.length());

  int ip = 0;
  
  //  int nargout = _stack[--_sp].asInteger();
  //  ArrayVector args;
  //  popVector(args);
  
  // FIXME - do something with the argument vector
  bool returnFound = false;
  const uint32_t *code = opcodes.constData();
  const Array *const_list = consts.constData();
  const Array *names_list = names.constData();
  Frame *f = _frames.back();
  Frame *closed_frame;
  for (int i=0;i<_frames.size();i++)
    if (_frames[i]->_closed)
      {
	closed_frame = _frames[i];
	break;
      }
  if (!closed_frame)  throw Exception("Closed frame not found!  Should never happen!");
  StdIOTermIF io;
  bool timeit = false;
  int save_ip;
  boost::timer::cpu_timer timer;

  while (!returnFound)
    {
      if (timeit) 
	{
	  timer.start();
	  save_ip = ip;
	}
      uint32_t insn = code[ip];
      switch (opcode(insn))
	{
	case OP_NOP:
	  break;
	case OP_RETURN:
	  returnFound = true;
	  break;
	case OP_PUSH:
	  _stack[_sp++] = REG1;
	  break;
	case OP_POP:
	  REG1 = _stack[--_sp];
	  break;
	case OP_CALL:
	case OP_LOAD_FREE:
	case OP_SAVE_FREE:
	case OP_LOAD_CAPTURED:
	case OP_SAVE_CAPTURED:
	  // Finish me
	  break;
	case OP_SAVE:
	  f->_localvars[constant(insn)] = REG1;
	  break;
	case OP_DCOLON:
	  REG1 = DoubleColon(_stack[_sp-2], _stack[_sp-1], _stack[_sp]);
	  _sp -= 3;
	  break;
	case OP_SUBSASGN:
	  doSubsasgnOp(f->_localvars[constant(insn)],REG1);
	  break;
	case OP_LOAD_CONST:
	  REG1 = const_list[constant(insn)];
	  break;
	case OP_LOAD:
	  REG1 = f->_localvars[constant(insn)];
	  break;
	case OP_START_LIST:
	  REG1 = Array(double(_sp));
	  break;
	case OP_END_LIST:
	  _stack[_sp] = Array(double(_sp - REG1.asInteger()));
	  _sp++;
	  break;
	case OP_LOAD_STACK:
	  _sp = REG1.asInteger() + constant(insn);
	  break;
	case OP_SUBSREF:
	  doSubsrefOp(REG1);
	  break;
	case OP_COLON:
	  REG1 = UnitColon(REG2,REG3);
	  break;
	case OP_ADD:
	  BINOP(Add,"plus");
	  break;
	case OP_MINUS:
	  BINOP(Subtract,"minus");
	  break;
	case OP_MTIMES:
	  BINOP(Multiply,"mtimes");
	  break;
	case OP_MRDIVIDE:
	  BINOP(RightDivide,"mrdivide");
	  break;
	case OP_MLDIVIDE:
	  BINOP(LeftDivide,"mldivide");
	  break;
	case OP_OR:
	  BINOP(Or,"or");
	  break;
	case OP_AND:
	  BINOP(And,"and");
	  break;
	case OP_LT:
	  BINOP(LessThan,"lt");
	  break;
	case OP_LE:
	  BINOP(LessEquals,"le");
	  break;
	case OP_GT:
	  BINOP(GreaterThan,"gt");
	  break;
	case OP_GE:
	  BINOP(GreaterEquals,"ge");
	  break;
	case OP_EQ:
	  BINOP(Equals,"eq");
	  break;
	case OP_NE:
	  BINOP(NotEquals,"ne");
	  break;
	case OP_TIMES:
	  BINOP(DotMultiply,"times");
	  break;
	case OP_RDIVIDE:
	  BINOP(DotRightDivide,"rdivide");
	  break;
	case OP_CASE:
	  // FIXME
	  break;
	case OP_COLUMN:
	  REG1 = REG2.column(REG3.asDouble());
	  break;
	case OP_LDIVIDE:
	  BINOP(DotLeftDivide,"ldivide");
	  break;
	case  OP_NUMCOLS:
	  REG1 = IterationColumns(REG2);
	  break;
	case OP_NEG:
	  UNARYOP(Negate,"uminus");
	  break;
	case OP_PLUS:
	  UNARYOP(Plus,"uplus");
	  break;
	case OP_NOT:
	  UNARYOP(Not,"not");
	  break;
	case OP_POWER:
	  BINOP(Power,"mpower");
	  break;
	case OP_DOTPOWER:
	  BINOP(DotPower,"power");
	  break;
	case OP_HERMITIAN:
	  UNARYOP(Hermitian,"ctranspose");
	  break;
	case OP_TRANSPOSE:
	  UNARYOP(Transpose,"transpose");
	  break;
	case OP_INCR:
	  REG1 = Array(double(REG1.asDouble() + 1));
	  break;
	case OP_LHSCOUNT:
	  // FIXME
	  break;
	case OP_SUBSASGNM:
	  // FIXME
	  break;
	case OP_ZERO:
	  REG1 = Array(double(0));
	  break;
	case OP_CELLROWDEF:
	  {
	    ArrayVector x;
	    popVector(x);
	    _stack[_sp++] = CellArrayFromArrayVector(x,x.size());
	    break;
	  }
	case OP_HCAT:
	  {
	    ArrayVector x;
	    popVector(x);
	    _stack[_sp++] = NCat(x,1);
	    break;
	  }
	case OP_VCAT:
	  {
	    ArrayVector x;
	    popVector(x);
	    REG1 = NCat(x,0);
	    break;
	  }
	case OP_LOAD_GLOBAL:
	case OP_LOAD_PERSIST:
	  break;
	case OP_LOAD_DYNAMIC:
	  {
	    FMString name = names_list[constant(insn)].asString();
	    REG1 = closed_frame->getDynamicVar(name);
	    break;
	  }
	case OP_SAVE_GLOBAL:
	  break;
	case OP_SAVE_DYNAMIC:
	  {
	    FMString name = names_list[constant(insn)].asString();
	    closed_frame->getDynamicVarRef(name) = REG1;
	    break;
	  }
	case OP_SAVE_PERSIST:
	case OP_JUMP_ZERO:
	  {
	    if (!RealAllNonZeros(REG1)) ip = constant(insn)-1;
	    break;
	  }
	case OP_JUMP:
	  {
	    ip = constant(insn)-1;
	    break;
	  }
	case OP_TRY_BEGIN:
	case OP_TRY_END:
	case OP_THROW:
	case OP_PRINT:
	  {
	    PrintArrayClassic(REG1,1000,&io);
	    break;
	  }
	case OP_DEREF:
	  {
	    // FIXME - handle function objects
	    REG1 = REG2;
	    break;
	  }
	case OP_SUBSASGN_GLOBAL:
	case OP_SUBSASGN_PERSIST:
	case OP_SUBSASGN_CAPTURED:
	case OP_SUBSASGN_FREE:
	  break;
	case OP_SUBSASGN_DYNAMIC:
 	  {
	    FMString name = names_list[constant(insn)].asString();
	    doSubsasgnOp(closed_frame->getDynamicVarRef(name),REG1);
	    break;
	  }
	case OP_LOOPCOUNT:
	  {
	    double last = _stack[--_sp].asDouble();
	    double step = _stack[--_sp].asDouble();
	    double first = _stack[--_sp].asDouble();
	    REG1 = Array(double(num_for_loop_iter(first,step,last)));
	    break;
	  }
	}
      ip++;
      if (timeit)
	{
	  timer.stop();
	  etimes[save_ip] += timer.elapsed().wall;
	}
    }
  for (int i=0;i<etimes.size();i++)
    {
      std::cout << "OPCode " << i << " time " << etimes[i]/1.0e9 << "\n";
    }
}

void VM::executeBlock(const Array &codeObject, bool singleStep)
{
  /*
  _vars.resize(code->_varlist.size());
  _varlist = code->_varlist;
  _stack.resize(256);
  _regfile.resize(256);
  _ip = 0;
  _sp = 0;
  while (_ip < code->_insnlist.size())
    {
      int32_t insn = code->_insnlist[_ip];
      dump();
      std::cout << "sp = " << _sp << "\n";
      std::cout << "\n";
      printf("%03d   ",_ip);
      int8_t opcode = opcode(insn);
      printf("%-15s",Compiler::getOpCodeName(opcode).c_str());
      printf("%-20s",Compiler::opcodeDecode(opcode,insn).c_str());
      std::cout << "\n";
      if (singleStep) getchar();
      switch (opcode(insn))
	{
	case OP_NOP:
	  break;
	case OP_PUSH:
	  _stack[_sp++] = REG1;
	  break;
	case OP_POP:
	  REG1 = _stack[--_sp];
	  break;
	case OP_CALL:
	  // Finish me
	  break;
	case OP_SAVE:
	  _vars[constant(insn)] = REG1;
	  break;
	case OP_DCOLON:
	  REG1 = DoubleColon(_stack[_sp-2], _stack[_sp-1], _stack[_sp]);
	  _sp -= 3;
	  break;
	case OP_SUBSASGN:
	  doSubsasgnOp(_vars[constant(insn)],REG1);
	  break;
	case OP_LOAD_CONST:
	  REG1 = code->_constlist[constant(insn)];
	  break;
	case OP_LOAD:
	  REG1 = _vars[constant(insn)];
	  break;
	case OP_START_LIST:
	  REG1 = Array(double(_sp));
	  break;
	case OP_END_LIST:
	  _stack[_sp] = Array(double(_sp - REG1.asInteger()));
	  _sp++;
	  break;
	case OP_LOAD_STACK:
	  _sp = REG1.asInteger() + constant(insn);
	  break;
	case OP_SUBSREF:
	  doSubsrefOp(REG1);
	  break;
	case OP_COLON:
	  REG1 = UnitColon(REG2,REG3);
	  break;
	case OP_ADD:
	  BINOP(Add,"plus");
	  break;
	case OP_MINUS:
	  BINOP(Subtract,"minus");
	  break;
	case OP_MTIMES:
	  BINOP(Multiply,"mtimes");
	  break;
	case OP_MRDIVIDE:
	  BINOP(RightDivide,"mrdivide");
	  break;
	case OP_MLDIVIDE:
	  BINOP(LeftDivide,"mldivide");
	  break;
	case OP_OR:
	  BINOP(Or,"or");
	  break;
	case OP_AND:
	  BINOP(And,"and");
	  break;
	case OP_LT:
	  BINOP(LessThan,"lt");
	  break;
	case OP_LE:
	  BINOP(LessEquals,"le");
	  break;
	case OP_GT:
	  BINOP(GreaterThan,"gt");
	  break;
	case OP_GE:
	  BINOP(GreaterEquals,"ge");
	  break;
	case OP_EQ:
	  BINOP(Equals,"eq");
	  break;
	case OP_NE:
	  BINOP(NotEquals,"ne");
	  break;
	case OP_TIMES:
	  BINOP(DotMultiply,"times");
	  break;
	case OP_RDIVIDE:
	  BINOP(DotRightDivide,"rdivide");
	  break;
	case OP_LDIVIDE:
	  BINOP(DotLeftDivide,"ldivide");
	  break;
	case OP_NEG:
	  UNARYOP(Negate,"uminus");
	  break;
	case OP_PLUS:
	  UNARYOP(Plus,"uplus");
	  break;
	case OP_NOT:
	  UNARYOP(Not,"not");
	  break;
	case OP_POWER:
	  BINOP(Power,"mpower");
	  break;
	case OP_DOTPOWER:
	  BINOP(DotPower,"power");
	  break;
	case OP_HERMITIAN:
	  UNARYOP(Hermitian,"ctranspose");
	  break;
	case OP_TRANSPOSE:
	  UNARYOP(Transpose,"transpose");
	  break;
	case OP_INCR:
	  REG1 = Array(double(REG1.asDouble() + 1));
	  break;
	case OP_LHSCOUNT:
	  break;
	case OP_SUBSASGNM:
	  break;
	case OP_ZERO:
	  REG1 = Array(double(0));
	  break;
	case OP_CELLROWDEF:
	  {
	    ArrayVector x;
	    popVector(x);
	    _stack[_sp++] = CellArrayFromArrayVector(x,x.size());
	    break;
	  }
	case OP_HCAT:
	  {
	    ArrayVector x;
	    popVector(x);
	    _stack[_sp++] = NCat(x,1);
	    break;
	  }
	case OP_VCAT:
	  {
	    ArrayVector x;
	    popVector(x);
	    REG1 = NCat(x,0);
	    break;
	  }
	}
      _ip++;
    }
  */
}
