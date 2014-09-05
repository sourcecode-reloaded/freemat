// TODO - LOAD/SAVE Dynamic is about 2.5 x slower than LOAD/SAVE local
// 

#include "VM.hpp"
#include <cmath>
#include <float.h>
//#include <values.h>
//#include "Print.hpp"
//#include "Struct.hpp"
//#include "Math.hpp"
//#include "TermIF.hpp"
#include <boost/timer/timer.hpp>
#include "Compiler.hpp"
#include "NCat.hpp"

std::string getOpCodeName(FM::op_t);


using namespace FM;

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

VM::VM(BaseTypes *types)
{
  _stack.resize(1024);
  _sp = 0;
  Frame *f = new Frame;
  f->_name = "base";
  f->_closed = true;
  _frames.push_back(f);
  _types = types;
}

void VM::defineBaseVariable(const FMString &name, const Object &value)
{
  int addr = _frames[0]->getAddress(name);
  if (addr == 0) addr = _frames[0]->allocateVariable(name);
  _frames[0]->_vars[addr-1] = value;
}

//void VM::dump()
//{
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
//}

#define REG1 f->_regfile[reg1(insn)]
#define REG2 f->_regfile[reg2(insn)]
#define REG3 f->_regfile[reg3(insn)]

#define UNARYOP(fnc,funcname)				\
    REG1 = REG2.type()->fnc(REG2);			

#define BINOP(fnc,funcname) 				\
  REG1 = REG2.type()->fnc(REG2,REG3);			


void VM::dump()
{
  _frames[0]->dump();
}

// Execute a function object, given a list of parameters (params).  Returns a list
// of returns.
Object VM::executeFunction(const Object &codeObject, const Object &parameters)
{
  // Create a new frame for the function
  Frame *f = new Frame;
  f->_name = _types->_string->getString(_types->_struct->getScalar(codeObject,"name"));
  // Space for registers
  f->_regfile.resize(256);
  // Space to store the address maps for variables
  f->_addr.resize(_types->_struct->getScalar(codeObject,"names").elementCount());
  // Function scopes are closed
  f->_closed = true;
  // Populate the arguments
  const Object * args = _types->_list->readOnlyData(parameters);
  const Object * param_names = _types->_list->readOnlyData(_types->_struct->getScalar(codeObject,"params"));
  int to_use = std::min<int>(parameters.elementCount(),_types->_struct->getScalar(codeObject,"params").elementCount());
  for (int i=0;i<to_use;i++)
    {
      int addr = f->allocateVariable(_types->_string->getString(param_names[i]));
      f->_vars[addr-1] = args[i];
    }
  _frames.push_back(f);
  // execute the code
  executeCodeObject(codeObject);
  // Collect return values
  Object retvec = _types->_list->empty();
  int to_return = _types->_struct->getScalar(codeObject,"returns").elementCount();
  const Object * return_names = _types->_list->readOnlyData(_types->_struct->getScalar(codeObject,"returns"));
  for (int i=0;i<to_return;i++)
    {
      FMString name = _types->_string->getString(return_names[i]);
      int addr = f->getAddress(name);
      if (!addr)
	{
	  std::cout << "Warning: not all outputs assigned";
	  _types->_list->push(retvec,_types->_double->empty());
	}
      else
	_types->_list->push(retvec,f->_vars[addr-1]);
    }
  _frames.pop_back();
  delete f;  
  return retvec;
}

void VM::executeScript(const Object &codeObject)
{
  Frame *f = new Frame;
  f->_name = _types->_string->getString(_types->_struct->getScalar(codeObject,"name"));
  // Space for registers
  f->_regfile.resize(256);
  // Space to store the address maps for variables
  f->_addr.resize(_types->_struct->getScalar(codeObject,"names").elementCount());
  f->_closed = false;
  _frames.push_back(f);
  // execute the code
  executeCodeObject(codeObject);
  // No return values.. pop the frame
  _frames.pop_back();
  delete f;
}


double num_for_loop_iter( double first, double step, double last )
{
    int signum = (step > 0) - (step < 0);
    int nsteps = (int) floor( ( last - first ) / step ) + 1;
    if( nsteps < 0 )
	return 0;

    double mismatch = signum*(first + nsteps*step - last);
    if( (mismatch > 0) && ( mismatch < 3.*feps(last) ) && ( step != rint(step) ) ) //allow overshoot by 3 eps in some cases
	nsteps++;

    return nsteps;  
}

// Dynamic load/stores - these behave differently 
// depending on if we are a script or not.  If we are
// a script, a dynamic load/store can see the calling
// scopes - up until we encounter a closed scope.

// Benchmark : A = 0; for i=1:1:1e7; A = A + i; end - 4.256 seconds
// Benchmark : function foo; A = 0; for i=1:1:1e7; A = A + i; end - 1.447 seconds
// Bnechmark : A = 0; for i=1:1:10000000; A(1) = A(1) + i; end
// function foo; a = 32; for i=1:1:100000000; a = a + i; end;a, end - 15 seconds or 19 seconds??  And then vector-> Object* reduces to 1
// function foo; a = 0; a(512,512) = 0; for k=1:1:20; for i=1:1:512; for j=1:1:512; a(j,i) = i+j; end; end; end


void VM::executeCodeObject(const Object &codeObject)
{
  Object consts = _types->_struct->getScalar(codeObject,"consts");
  Object opcodes = _types->_struct->getScalar(codeObject,"code");
  Object names = _types->_struct->getScalar(codeObject,"names");

  const Object *const_list = _types->_list->readOnlyData(consts);
  const uint64_t *code = _types->_uint64->readOnlyData(opcodes);
  const Object *names_list = _types->_list->readOnlyData(names);

  std::vector<double> etimes;
  etimes.resize(opcodes.elementCount());

  int ip = 0;
  
  //  int nargout = _stack[--_sp].asInteger();
  //  ObjectVector args;
  //  popVector(args);
  
  // FIXME - do something with the argument vector
  bool returnFound = false;

  Frame *f = _frames.back();
  Frame *closed_frame = NULL;
  for (int i=_frames.size()-1;i>=0;--i)
    if (_frames[i]->_closed)
      {
	closed_frame = _frames[i];
	break;
      }
  if (!closed_frame)  throw Exception("Closed frame not found!  Should never happen!");

  std::cout << "Current frame: " << f->_name << "\n";
  std::cout << "Closed frame: " << closed_frame->_name << "\n";

  StdIOTermIF io;

  // const bool timeit = true;

  int save_ip;
  boost::timer::cpu_timer timer;
  
  while (!returnFound)
    {
      // if (timeit) 
      // 	{
      // 	  timer.start();
      // 	  save_ip = ip;
      // 	}
      insn_t insn = code[ip];

      /*
      printf(">>%03d   ",ip);
      int8_t opcode_num = opcode(insn);
      printf("%-15s",getOpCodeName(opcode_num).c_str());
      printf("%-20s",Compiler::opcodeDecode(opcode_num,insn).c_str());
      std::cout << "\n";
      */

      switch (opcode(insn))
	{
	case OP_NOP:
	  break;
	case OP_RETURN:
	  returnFound = true;
	  break;
	case OP_PUSH:
	  _types->_list->push(REG1,REG2);
	  break;
 	case OP_FIRST:
	  REG1 = _types->_list->first(REG2);
	  break;
	case OP_CALL:
	  // Finish me
	  break;
	case OP_DCOLON:
	  {
	    const Object *ap = _types->_list->readOnlyData(REG2);
	    REG1 = ap[0].type()->DoubleColon(ap[0],ap[1],ap[2]);
	    break;
	  }
	case OP_LOAD_CONST:
	  REG1 = const_list[get_constant(insn)];
	  break;
	case OP_NEW_LIST:
	  REG1 = _types->_list->empty();
	  break;
	case OP_SUBSREF:
	  _types->_list->merge(REG1,REG2.type()->get(REG2,REG3));
	  break;
	case OP_COLON:
	  BINOP(Colon,"colon");
	  break;
	case OP_ADD:
	  BINOP(Add,"plus");
	  break;
	case OP_MINUS:
	  BINOP(Subtract,"minus");
	  break;
	case OP_LE:
	  BINOP(LessEquals,"le");
	  break;
	case OP_LT:
	  BINOP(LessThan,"lt");
	  break;
	case OP_TIMES:
	  BINOP(DotMultiply,"times");
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
	case OP_OR:
	  BINOP(Or,"or");
	  break;
	case OP_AND:
	  BINOP(And,"and");
	  break;
	case OP_MTIMES:
	  BINOP(Multiply,"mtimes");
	  break;
	case OP_MRDIVIDE:
	  REG1 = REG2.type()->RightDivide(REG2,REG3,&io);
	  break;
	case OP_MLDIVIDE:
	  REG1 = REG2.type()->LeftDivide(REG2,REG3,&io);
	  break;
	case OP_RDIVIDE:
	  BINOP(DotRightDivide,"rdivide");
	  break;
	case OP_LDIVIDE:
	  BINOP(DotLeftDivide,"ldivide");
	  break;
	case OP_PLUS:
	  UNARYOP(Plus,"uplus");
	  break;
	case OP_NEG:
	  UNARYOP(Neg,"negate");
	  break;
	  /*
	case  OP_NUMCOLS:
	  REG1 = IterationColumns(REG2);
	  break;
	case OP_CASE:
	  // FIXME
	  break;
	case OP_COLUMN:
	  REG1 = REG2.column(REG3.asDouble());
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
	  */
	case OP_HERMITIAN:
	  UNARYOP(Hermitian,"ctranspose");
	  break;
	case OP_TRANSPOSE:
	  UNARYOP(Transpose,"transpose");
	  break;
	case OP_INCR:
	  REG1 = _types->_double->makeScalar(_types->_double->scalarValue(REG1)+1);
	  break;
	  /*	case OP_LHSCOUNT:
	  // FIXME
	  break;
	  case OP_SUBSASGNM:
	  // FIXME
	  break;
	  */
	case OP_ZERO:
	  REG1 = _types->_double->zeroScalar();
	  break;
	case OP_HCAT:
	  REG1 = NCat(REG2,1);
	  break;
	case OP_VCAT:
	  REG1 = NCat(REG2,0);
	  break;
	  /*
	case OP_CELLROWDEF:
	  {
	    // ObjectVector x;
	    // popVector(x);
	    // _stack[_sp++] = CellObjectFromObjectVector(x,x.size());
	    break;
	  }
	case OP_LOAD_GLOBAL:
	case OP_LOAD_PERSIST:
	  break;
	  */
	case OP_LOAD:
	  {
	    register int ndx = get_constant(insn);
	    register int addr = f->_addr[ndx];
	    if (!addr)
	      {
		FMString name = _types->_string->getString(names_list[ndx]);
		f->_addr[ndx] = closed_frame->getAddress(name);
		if (!f->_addr[ndx])
		  throw Exception("Reference to undefined variable " + name);
		addr = f->_addr[ndx];
	      }
	    REG1 = closed_frame->_vars[addr-1];
	    break;
	  }
	case OP_SAVE_GLOBAL:
	  break;
	case OP_SAVE:
	  {
	    register int ndx = get_constant(insn);
	    register int addr = f->_addr[ndx];
	    if (!addr)
	      {
		FMString name = _types->_string->getString(names_list[get_constant(insn)]);
		f->_addr[get_constant(insn)] = closed_frame->getAddress(name);
		if (!f->_addr[get_constant(insn)])
		  f->_addr[get_constant(insn)] = closed_frame->allocateVariable(name);
		addr = f->_addr[ndx];
	      }
	    closed_frame->_vars[addr-1] = REG1;
	    break;
	  }
	case OP_SAVE_PERSIST:
	case OP_JUMP_ZERO:
	  {
	    if (_types->_double->scalarValue(REG1) == 0)
	      ip = get_constant(insn)-1;
	    break;
	  }
	case OP_JUMP:
	  {
	    ip = get_constant(insn)-1;
	    break;
	  }
	case OP_TRY_BEGIN:
	case OP_TRY_END:
	case OP_THROW:
	case OP_PRINT:
	  {
	    REG1.type()->print(REG1,io);
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
	case OP_SUBSASGN:
 	  {
	    register int ndx = get_constant(insn);
	    register int addr = f->_addr[ndx];
	    if (!addr)
	      {
		FMString name = _types->_string->getString(names_list[get_constant(insn)]);
		f->_addr[get_constant(insn)] = closed_frame->getAddress(name);
		if (!f->_addr[get_constant(insn)])
		  f->_addr[get_constant(insn)] = closed_frame->allocateVariable(name);
		addr = f->_addr[ndx];
	      }
	    Object &var = closed_frame->_vars[addr-1];
	    var.type()->set(var,REG1,REG2);
	    break;
	  }
	case OP_LOOPCOUNT:
	  {
	    const Object *ap = _types->_list->readOnlyData(REG2);
	    double last = _types->_double->scalarValue(ap[2]);
	    double step = _types->_double->scalarValue(ap[1]);
	    double first = _types->_double->scalarValue(ap[0]);
	    REG1 = _types->_double->makeScalar(num_for_loop_iter(first,step,last));
	    break;
	  }
	case OP_LOAD_INT:
	  {
	    REG1 = _types->_double->makeScalar(get_constant(insn));
	    break;
	  }
	case OP_PUSH_INT:
	  {
	    _types->_list->push(REG1,_types->_int32->makeScalar(get_constant(insn)));
	    break;
	  }
	default:
	  {
	    std::cerr << "Unknown opcode " << opcode(insn) << "\n";
	    printf("%03d   ",ip);
	    int8_t op = opcode(insn);
	    printf("%-15s",getOpCodeName(op).c_str());
	    printf("%-20s",Compiler::opcodeDecode(op,insn).c_str());
	    std::cout << "\n";
	    exit(1);
	  }
	}
      ip++;
      // if (timeit)
      // 	{
      // 	  timer.stop();
      // 	  etimes[save_ip] += timer.elapsed().wall;
      // 	}
    }
  // if (timeit)
  //   for (int i=0;i<etimes.size();i++)
  //     {
  // 	std::cout << "OPCode " << i << " time " << etimes[i]/1.0e9 << "\n";
  //     }
}

void VM::executeBlock(const Object &codeObject, bool singleStep)
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
	  _vars[get_constant(insn)] = REG1;
	  break;
	case OP_DCOLON:
	  REG1 = DoubleColon(_stack[_sp-2], _stack[_sp-1], _stack[_sp]);
	  _sp -= 3;
	  break;
	case OP_SUBSASGN:
	  doSubsasgnOp(_vars[get_constant(insn)],REG1);
	  break;
	case OP_LOAD_CONST:
	  REG1 = code->_constlist[get_constant(insn)];
	  break;
	case OP_LOAD:
	  REG1 = _vars[get_constant(insn)];
	  break;
	case OP_START_LIST:
	  REG1 = Object(double(_sp));
	  break;
	case OP_END_LIST:
	  _stack[_sp] = Object(double(_sp - REG1.asInteger()));
	  _sp++;
	  break;
	case OP_LOAD_STACK:
	  _sp = REG1.asInteger() + get_constant(insn);
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
	  REG1 = Object(double(REG1.asDouble() + 1));
	  break;
	case OP_LHSCOUNT:
	  break;
	case OP_SUBSASGNM:
	  break;
	case OP_ZERO:
	  REG1 = Object(double(0));
	  break;
	case OP_CELLROWDEF:
	  {
	    ObjectVector x;
	    popVector(x);
	    _stack[_sp++] = CellObjectFromObjectVector(x,x.size());
	    break;
	  }
	case OP_HCAT:
	  {
	    ObjectVector x;
	    popVector(x);
	    _stack[_sp++] = NCat(x,1);
	    break;
	  }
	case OP_VCAT:
	  {
	    ObjectVector x;
	    popVector(x);
	    REG1 = NCat(x,0);
	    break;
	  }
	}
      _ip++;
    }
  */
}
