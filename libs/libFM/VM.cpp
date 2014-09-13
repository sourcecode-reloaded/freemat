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
#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "CodeType.hpp"
#include "config.h"

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


// Oops - Forgot about adding variables to frames that are already
// allocated.  That makes things difficult.
//
// Crap.
// 

VM::VM(ThreadContext *ctxt)
{
  _registers.resize(register_stack_size);
  for (int i=0;i<frame_stack_size;i++)
    _frames.push_back(new Frame(ctxt)); // FIXME - Bad way to do this?
  //  _frames.resize(frame_stack_size);
  _frames[0]->_name = "base";
  _frames[0]->_closed = true;
  _fp = 0;
  _rp = 0;
  _ctxt = ctxt;
}

void VM::defineBaseVariable(const FMString &name, const Object &value)
{
  _frames[0]->setVariableSlow(name,value);
}

#define REG1 regfile[reg1(insn)]
#define REG2 regfile[reg2(insn)]
#define REG3 regfile[reg3(insn)]

#define UNARYOP(fnc,funcname)				\
    REG1 = REG2.type()->fnc(REG2);			

#define BINOP(fnc,funcname) 				\
  REG1 = REG2.type()->fnc(REG2,REG3);			


void VM::dump()
{
  //  _frames[0]->dump();
}

// Execute a function object, given a list of parameters (params).  Returns a list
// of returns.
Object VM::executeFunction(const Object &codeObject, const Object &parameters)
{
  // std::cout << "fp = " << _fp << " rp = " << _rp << "\n";
  // Create a new frame for the function
  _fp++;
  const CodeData *cp = _ctxt->_code->readOnlyData(codeObject);
  _frames[_fp]->_name = _ctxt->_string->getString(cp->m_name);
  // For functions, all addresses are pre-set to point
  // to the first N slots of the variables space
  int N = cp->m_names.elementCount();
  _frames[_fp]->_addrs = _ctxt->_index->makeMatrix(N,1);
  ndx_t *ap = _ctxt->_index->readWriteData(_frames[_fp]->_addrs);
  for (int i=0;i<N;i++) ap[i] = i;
  _frames[_fp]->_sym_names = cp->m_names;
  _frames[_fp]->_vars = _ctxt->_list->makeMatrix(N,1);
  _frames[_fp]->_reg_offset = _rp;
  _rp += 256; // FIXME
  Object *sp = _ctxt->_list->readWriteData(_frames[_fp]->_vars);
  // Function scopes are closed
  _frames[_fp]->_closed = true;
  // Populate the arguments
  // FIXME - need to only store the number of args and returns
  const Object * args = _ctxt->_list->readOnlyData(parameters);
  const Object * param_ndx = _ctxt->_list->readOnlyData(cp->m_params);
  int to_use = std::min<int>(parameters.elementCount(),cp->m_params.elementCount());
  for (int i=0;i<to_use;i++)
    sp[_ctxt->_index->scalarValue(param_ndx[i])] = args[i];
  // execute the code
  executeCodeObject(codeObject);
  // Collect return values
  Object retvec = _ctxt->_list->empty();
  int to_return = cp->m_returns.elementCount();
  const Object * return_ndx = _ctxt->_list->readOnlyData(cp->m_returns);
  sp = _ctxt->_list->readWriteData(_frames[_fp]->_vars);
  for (int i=0;i<to_return;i++)
    _ctxt->_list->push(retvec,sp[_ctxt->_index->scalarValue(return_ndx[i])]);
  _frames[_fp]->_sym_names = Object();
  _frames[_fp]->_vars = Object();
  _frames[_fp]->_addrs = Object();
  // TODO: Clean up the registers
  _rp = _frames[_fp]->_reg_offset;
  _fp--;
  return retvec;
}

void VM::executeScript(const Object &codeObject)
{
  _fp++;
  const CodeData *cp = _ctxt->_code->readOnlyData(codeObject);
  _frames[_fp]->_name = _ctxt->_string->getString(cp->m_name);
  _frames[_fp]->_sym_names = _ctxt->_list->empty();
  int N = cp->m_names.elementCount();
  _frames[_fp]->_addrs = _ctxt->_index->makeMatrix(N,1);
  _frames[_fp]->_reg_offset = _rp;
  _frames[_fp]->_ctxt = _ctxt;
  _frames[_fp]->_closed = false;
  _rp += 256; // FIXME
  ndx_t *ap = _ctxt->_index->readWriteData(_frames[_fp]->_addrs);
  // Initialize the addresses to be unknown
  for (int i=0;i<N;i++) ap[i] = -1;
  _frames[_fp]->_closed = false;
  // execute the code
  executeCodeObject(codeObject);
  // No return values.. pop the frame
  _rp = _frames[_fp]->_reg_offset;
  _frames[_fp]->_sym_names = Object();
  _frames[_fp]->_vars = Object();
  _frames[_fp]->_addrs = Object();
  _fp--;
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

// Idea:
// When a function starts, each variable is allocated based on the name
// The address of a variable is then base+ndx, where ndx is the offset of the variable name.
// When a script starts, the variables must be mapped from a closed scope.  These cannot
// be pre-allocated, as they may already be defined in the parent scope.  Furthermore,
// given the dynamic nature, the lookup for the address needs a dictionary to find the
// address.
//
// 


void VM::executeCodeObject(const Object &codeObject)
{
  const CodeData *cp = _ctxt->_code->readOnlyData(codeObject);

  const Object *const_list = _ctxt->_list->readOnlyData(cp->m_consts);
  const uint64_t *code = _ctxt->_uint64->readOnlyData(cp->m_code);
  const Object *names_list = _ctxt->_list->readOnlyData(cp->m_names);

  int ip = 0;
  bool returnFound = false;

  Frame *closed_frame = NULL;
  for (int i=_fp;i>=0;--i)
    if (_frames[i]->_closed)
      {
	closed_frame = _frames[i];
	break;
      }
  if (!closed_frame)  throw Exception("Closed frame not found!  Should never happen!");

  Object *regfile = &(_registers[0]) + _frames[_fp]->_reg_offset;
  ndx_t *addrfile = _ctxt->_index->readWriteData(_frames[_fp]->_addrs);
  Object *varfile = _ctxt->_list->readWriteData(closed_frame->_vars);

  int save_ip;

  while (!returnFound)
    {
      insn_t insn = code[ip];

      switch (opcode(insn))
	{
	case OP_NOP:
	  break;
	case OP_RETURN:
	  returnFound = true;
	  break;
	case OP_PUSH:
	  _ctxt->_list->push(REG1,REG2);
	  break;
 	case OP_FIRST:
	  REG1 = _ctxt->_list->first(REG2);
	  break;
	// case OP_CALL:
	//   // Finish me
	//   break;
	case OP_DCOLON:
	  {
	    const Object *ap = _ctxt->_list->readOnlyData(REG2);
	    REG1 = ap[0].type()->DoubleColon(ap[0],ap[1],ap[2]);
	    break;
	  }
	case OP_LOAD_CONST:
	  REG1 = const_list[get_constant(insn)];
	  break;
	case OP_NEW_LIST:
	  REG1 = _ctxt->_list->empty();
	  break;
	case OP_SUBSREF:
	  _ctxt->_list->merge(REG1,REG2.type()->get(REG2,REG3));
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
	  BINOP(RightDivide,"mrdivide");
	  break;
	case OP_MLDIVIDE:
	  BINOP(LeftDivide,"mldivide");
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
	case  OP_NUMCOLS:
	  REG1 = _ctxt->_index->makeScalar(REG2.elementCount()/REG2.rows());
	  break;
	  /*
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
	  REG1 = _ctxt->_double->makeScalar(_ctxt->_double->scalarValue(REG1)+1);
	  break;
	  /*	case OP_LHSCOUNT:
	  // FIXME
	  break;
	  case OP_SUBSASGNM:
	  // FIXME
	  break;
	  */
	case OP_ZERO:
	  REG1 = _ctxt->_double->zeroScalar();
	  break;
	case OP_HCAT:
	  REG1 = NCat(_ctxt,REG2,1);
	  break;
	case OP_VCAT:
	  REG1 = NCat(_ctxt,REG2,0);
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
	    register int addr = addrfile[ndx];
	    if (addr == -1)
	      {
		FMString name = _ctxt->_string->getString(names_list[ndx]);
		addrfile[ndx] = closed_frame->getAddress(name);
		if (addrfile[ndx] == -1)
		  throw Exception("Reference to undefined variable " + name);
		addr = addrfile[ndx];
	      }
	    REG1 = varfile[addr];
	    break;
	  }
	case OP_SAVE_GLOBAL:
	  break;
	case OP_SAVE:
	  {
	    register int ndx = get_constant(insn);
	    register int addr = addrfile[ndx];
	    if (addr == -1)
	      {
		FMString name = _ctxt->_string->getString(names_list[get_constant(insn)]);
		addrfile[ndx] = closed_frame->getAddress(name);
		if (addrfile[ndx] == -1)
		  {
		    addrfile[ndx] = closed_frame->allocateVariable(name);
		    varfile = _ctxt->_list->readWriteData(closed_frame->_vars);
		  }
		addr = addrfile[ndx];
	      }
	    varfile[addr] = REG1;
	    break;
	  }
	case OP_SAVE_PERSIST:
	case OP_JUMP_ZERO:
	  {
	    if (_ctxt->_double->scalarValue(REG1) == 0)
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
	    REG1.type()->print(REG1);
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
	    register int addr = addrfile[ndx];
	    if (addr == -1)
	      {
		FMString name = _ctxt->_string->getString(names_list[get_constant(insn)]);
		addrfile[ndx] = closed_frame->getAddress(name);
		if (addrfile[ndx] == -1)
		  {
		    addrfile[ndx] = closed_frame->allocateVariable(name);
		    varfile = _ctxt->_list->readWriteData(closed_frame->_vars);
		  }
		addr = addrfile[ndx];
	      }
	    varfile[addr].type()->set(varfile[addr],REG1,REG2);
	    break;
	  }
	case OP_LOOPCOUNT:
	  {
	    const Object *ap = _ctxt->_list->readOnlyData(REG2);
	    double last = _ctxt->_double->scalarValue(ap[2]);
	    double step = _ctxt->_double->scalarValue(ap[1]);
	    double first = _ctxt->_double->scalarValue(ap[0]);
	    REG1 = _ctxt->_double->makeScalar(num_for_loop_iter(first,step,last));
	    break;
	  }
	case OP_LOAD_INT:
	  {
	    REG1 = _ctxt->_double->makeScalar(get_constant(insn));
	    break;
	  }
	case OP_PUSH_INT:
	  {
	    _ctxt->_list->push(REG1,_ctxt->_int32->makeScalar(get_constant(insn)));
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
    }
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
