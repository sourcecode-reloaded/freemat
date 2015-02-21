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
#include "Assembler.hpp"
#include <boost/timer/timer.hpp>
#include "Compiler.hpp"
#include "NCat.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "CodeType.hpp"
#include "config.h"
#include "TypeUtils.hpp"
#include <readline/readline.h>
#include <readline/history.h>

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

VM::VM(ThreadContext *ctxt) : _registers(ctxt->_list->makeMatrix(register_stack_size,1)),
			      _modules(ctxt->_list->makeMatrix(frame_stack_size,1)),
			      _exception(ctxt)
{
  for (int i=0;i<frame_stack_size;i++)
    _frames.push_back(new Frame(ctxt)); // FIXME - Bad way to do this?
  //  _frames.resize(frame_stack_size);
  _frames[0]->_name = "base";
  _frames[0]->_closed = true;
  _fp = 0;
  _rp = 0;
  _ctxt = ctxt;
}

void VM::dbshift(int n) {
  
}

Object VM::backtrace() {
  Object list(_ctxt->_cell->makeMatrix(_fp,1));
  Object *lp = _ctxt->_cell->rw(list);
  for (int i=0;i<_fp;i++) {
    lp[i] = _ctxt->_string->makeString(_frames[i]->_name);
  }
  return list;
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

// Excuting a module is like executing a function, except that an additional stack is required
Object VM::executeModule(const Object &moduleObject, const Object &parameters)
{
  const ModuleData *cmd = _ctxt->_module->ro(moduleObject);
  if (cmd->m_modtype == ScriptModuleType) {
    if (parameters.count() != 0)
      throw Exception("Cannot use arguments in call to a script");
    const FunctionData *fd = _ctxt->_function->ro(cmd->m_main);
    executeScript(fd->m_code);
    return _ctxt->_list->empty();
  } else {
    // Push the module object onto the stack
    _ctxt->_list->push(_modules,moduleObject);
    Object ret = executeFunction(_ctxt->_module->ro(moduleObject)->m_main,parameters);
    _modules = _ctxt->_list->pop(_modules);
    return ret;
  }
}

void VM::defineClass(const Object &name, const Object &arguments)
{
  FMString className = _ctxt->_string->str(name);
  if (_ctxt->_globals->count(className) > 0) return;
  Object fooMeta = _ctxt->_meta->makeScalar();
  ClassMetaData *cmd = _ctxt->_meta->rw(fooMeta);
  cmd->m_name = name;
  const Object *ap = _ctxt->_list->ro(arguments);
  // ap = [superclasses, properties, methods]
  const Object &superclasses = ap[0];
  const Object &parameters = ap[1];
  const Object &events = ap[2];
  const Object &methods = ap[3];
  const Object &constructor = ap[4];
  cmd->m_constructor = constructor;
  const Object *pp = _ctxt->_list->ro(parameters);
  for (int i=0;i<parameters.count();i++)
    {
      const Object *pl = _ctxt->_list->ro(pp[i]);
      _ctxt->_meta->addProperty(fooMeta,
				pl[0], // name
				_ctxt->_bool->scalarValue(pl[1]), // isconstant
				_ctxt->_bool->scalarValue(pl[2]), // isdependent
				pl[3], // default value
				pl[4], // getter
				pl[5]); // setter
    }
  const Object *mp = _ctxt->_list->ro(methods);
  for (int i=0;i<methods.count();i++)
    {
      const Object *ml = _ctxt->_list->ro(mp[i]);
      _ctxt->_meta->addMethod(fooMeta,
			      ml[0], // name
			      ml[1], // code
			      _ctxt->_bool->scalarValue(ml[2])); // is constant
    }
  // We add superclasses last to get name resolution correct
  const Object *sp = _ctxt->_list->ro(superclasses);
  for (int i=0;i<superclasses.count();i++) 
    {
      Object super_meta = _ctxt->_globals->at(_ctxt->_string->str(sp[i]));
      std::cout << "Defining class " << className << " super class " << super_meta.description() << "\n";
      _ctxt->_meta->addSuperClass(fooMeta,super_meta);
    }
  const Object *ep = _ctxt->_list->ro(events);
  for (int i=0;i<events.count();i++)
    _ctxt->_meta->addEvent(fooMeta,ep[i]);
  _ctxt->_globals->insert(std::make_pair(className,fooMeta));
}


void VM::defineFrame(const Object &names, int registerCount, bool closed)
{
  int nameCount = names.count();
  _frames[_fp]->_addrs = _ctxt->_index->makeMatrix(nameCount,1);
  ndx_t *ap = _ctxt->_index->rw(_frames[_fp]->_addrs);
  // FIXME - not right - parameters are defined when the function executes
  for (int i=0;i<nameCount;i++) ap[i] = -1;
  _frames[_fp]->_defined = _ctxt->_bool->makeMatrix(nameCount,1);
  _frames[_fp]->_sym_names = names;
  _frames[_fp]->_vars = _ctxt->_list->makeMatrix(nameCount,1);
  _frames[_fp]->_exception_handlers.clear();
  _frames[_fp]->_reg_offset = _rp;
  _frames[_fp]->_module = _ctxt->_list->last(_modules);
  _frames[_fp]->_captures = _ctxt->_list->empty();
  _frames[_fp]->_closed = closed;
  if (closed)
    _frames[_fp]->_closedFrame = _frames[_fp];
  else
    {
      // Open frames must be closed by searching backwards
      Frame *closed_frame = NULL;
      for (int i=_fp;i>=0;--i)
	if (_frames[i]->_closed)
	  {
	    closed_frame = _frames[i];
	    break;
	  }
      if (!closed_frame)  throw Exception("Closed frame not found!  Should never happen!");
      _frames[_fp]->_closedFrame = closed_frame;
    }  
  _rp += registerCount;
}

Object VM::executeAnonymousFunction(const Object &codeObject, const Object &parameters, const HashMap<Object> &captures)
{
  _fp++;
  const CodeData *cp = _ctxt->_code->ro(codeObject);
  _frames[_fp]->_name = _ctxt->_string->str(cp->m_name);
  defineFrame(cp->m_names,cp->m_registers,true);
  Object *sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  ndx_t *ap = _ctxt->_index->rw(_frames[_fp]->_addrs);
  // Populate the arguments
  // FIXME - need to only store the number of args and returns
  const Object * args = _ctxt->_list->ro(parameters);
  const ndx_t * param_ndx = _ctxt->_index->ro(cp->m_params);
  int to_use = std::min<int>(parameters.count(),cp->m_params.count());
  for (int i=0;i<to_use;i++)
    {
      sp[param_ndx[i]] = args[i];
      ap[param_ndx[i]] = param_ndx[i];
    }
  Object *vars = _ctxt->_list->rw(_frames[_fp]->_vars);
  ndx_t *addrfile = _ctxt->_index->rw(_frames[_fp]->_addrs);  
  for (auto n : captures) {
    int addr = _frames[_fp]->mapNameToVariableIndex(n.first);
    if (addr != -1) {
      vars[addr] = n.second;
      addrfile[addr] = addr;
    }
  }
  // execute the code
  executeCodeObject(codeObject);
  // Collect return values
  Object retvec = _ctxt->_list->empty();
  int to_return = cp->m_returns.count();
  const ndx_t * return_ndx = _ctxt->_index->ro(cp->m_returns);
  sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  for (int i=0;i<to_return;i++)
    _ctxt->_list->push(retvec,sp[return_ndx[i]]);
  _frames[_fp]->_sym_names = _ctxt->_list->empty();
  _frames[_fp]->_vars = _ctxt->_list->empty();
  _frames[_fp]->_addrs = _ctxt->_index->empty();
  _frames[_fp]->_defined = _ctxt->_bool->empty();
  _frames[_fp]->_exception_handlers.clear();
  {
    Object *regfile = _ctxt->_list->rw(_registers);
    for (int i=_frames[_fp]->_reg_offset;i<_rp;i++)
      regfile[i] = _ctxt->_double->empty();
  }
  _rp = _frames[_fp]->_reg_offset;
  _fp--;
  return retvec;  
}

// Execute a function object, given a list of parameters (params).  Returns a list
// of returns.
Object VM::executeFunction(const Object &functionObject, const Object &parameters)
{
  //
  // The structure of a frame needs a little explanation.
  // The _vars array holds the values for each name in the frame
  // The _sym_names array holds the names defined in the frame
  // The _addrs array contains the addresses of each name into the _vars array
  // 
  // Initially, when a frame is created, only variables that are known
  // defined in the frame have addresses (i.e., the parameters to the 
  // function call).  For a script, none of the variables are known at
  // creation time.
  //
  //  Now consider a OP_LOAD
  //
  //   If the address is -1, we look up the variable in the closed scope.
  //   If the variable is defined in the closed scope, we return it's address.
  //
  // The implication is that we need an additional flag per variable that
  // indicated that the variable has been defined.  A fetch on a variable
  // that has not been defined should trigger a search of the global scope for
  // a function of the given name. 
  //
  // Create a new frame for the function
  _fp++;
  const FunctionData *fd = _ctxt->_function->ro(functionObject);
  const CodeData *cp = _ctxt->_code->ro(fd->m_code);
  _frames[_fp]->_name = _ctxt->_string->str(cp->m_name);
  std::cout << "Starting function: " << _frames[_fp]->_name << "\n";
  defineFrame(cp->m_names,cp->m_registers,true);
  ndx_t *ap = _ctxt->_index->rw(_frames[_fp]->_addrs);
  _frames[_fp]->_captures = fd->m_closure;
  // Allocate space for the captured variables (free ones come through the closure)
  for (int i=0;i<cp->m_captured.count();i++)
    _ctxt->_list->push(_frames[_fp]->_captures,_ctxt->_captured->empty());
  Object *sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  // Populate the arguments
  // FIXME - need to only store the number of args and returns
  const Object * args = _ctxt->_list->ro(parameters);
  const ndx_t * param_ndx = _ctxt->_index->ro(cp->m_params);
  bool varargin_case = _ctxt->_index->scalarValue(cp->m_varargin) != -1;
  int to_use;
  if (!varargin_case)
    to_use = std::min<int>(parameters.count(),cp->m_params.count());
  else
    to_use = std::min<int>(parameters.count(),cp->m_params.count()-1);
  std::cout << "to_use = " << to_use << "\n";
  for (int i=0;i<to_use;i++)
    {
      // Two cases here - one is that the parameter is normal, the other is that it is
      // captured.  If it is normal:
      if (param_ndx[i] < 1000) 
	{
	  sp[param_ndx[i]] = args[i];
	  ap[param_ndx[i]] = param_ndx[i];
	}
      else
	{
	  ndx_t capture_slot = param_ndx[i] % 1000;
	  std::cout << "Parameter " << i << " put into cell " << capture_slot << "\n";
	  _ctxt->_captured->set(_ctxt->_list->rw(_frames[_fp]->_captures)[capture_slot],args[i]);
	}
    }
  if (varargin_case)
    {
      int to_push = std::max<int>(0,parameters.count()-to_use);
      Object varargin = _ctxt->_cell->makeMatrix(to_push,1);
      Object *vip = _ctxt->_cell->rw(varargin);
      for (int i=0;i<to_push;i++)
	vip[i] = args[to_use+i];
      int varargin_location = _ctxt->_index->scalarValue(cp->m_varargin);
      sp[varargin_location] = varargin;
      ap[varargin_location] = varargin_location;
    }
  // execute the code
  executeCodeObject(fd->m_code);
  // Collect return values
  Object retvec = _ctxt->_list->empty();
  int to_return = cp->m_returns.count();
  bool varargout_case = _ctxt->_index->scalarValue(cp->m_varargout) != -1;
  if (varargout_case) to_return -= 1;
  const ndx_t * return_ndx = _ctxt->_index->ro(cp->m_returns);
  sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  for (int i=0;i<to_return;i++)
    {
      if (return_ndx[i] < 1000)
	_ctxt->_list->push(retvec,sp[return_ndx[i]]);
      else
	{
	  ndx_t capture_slot = return_ndx[i] % 1000;
	  std::cout << "Return " << i << " taken from cell " << capture_slot << "\n";
	  _ctxt->_list->push(retvec,_ctxt->_captured->get(_ctxt->_list->ro(_frames[_fp]->_captures)[capture_slot]));
	}
    }
  if (varargout_case) {
    std::cout << "to_return = " << to_return << "\n";
    std::cout << "returns = " << cp->m_returns << "\n";
    int varargout_location = _ctxt->_index->scalarValue(cp->m_varargout);
    const Object & varargout = sp[varargout_location];
    std::cout << "VARARGOUT " << varargout << "\n";
    const Object * vip = _ctxt->_cell->ro(varargout);
    for (int i=0;i<varargout.count();i++)
      _ctxt->_list->push(retvec,vip[i]);
  }
  _frames[_fp]->_sym_names = _ctxt->_list->empty();
  _frames[_fp]->_vars = _ctxt->_list->empty();
  _frames[_fp]->_addrs = _ctxt->_index->empty();
  _frames[_fp]->_defined = _ctxt->_bool->empty();
  _frames[_fp]->_exception_handlers.clear();
  /* - TODO - Do we still need this?
  {
    Object *regfile = _ctxt->_list->rw(_registers);
    for (int i=_frames[_fp]->_reg_offset;i<_rp;i++)
      regfile[i] = _ctxt->_double->empty();
      }
  */
  _rp = _frames[_fp]->_reg_offset;
  _fp--;
  return retvec;
}

void VM::debugCycle()
{
  // FIXME - this state should be a stack or something
  // FIXME - also need to preserve the debug context.
  Debug state;
  state._fp_when_called = _fp;
  for (int i=_fp;i>=0;--i)
    if (_frames[_fp]->_closed)
      state._activeFrame = _frames[_fp];
  debugs.push(state);
  while (1)
    {
      char *p = readline("K--> ");
      if (p && *p)
	add_history(p);
      if (!p)
	return;
      FMString body(p);
      body += "\n\n";
      try {
	_ctxt->_compiler->compile(body);
	Module *mod = _ctxt->_compiler->module();
	if (mod)
	  {
	    Object p = _ctxt->_asm->run(mod->_main);
	    this->executeScript(p);
	    std::cout << "Execute script complete\n";
	    if (_retscrpt_found) return;
	  }
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      }
    }
  debugs.pop();
}

void VM::executeScript(const Object &codeObject)
{
  _fp++;
  const CodeData *cp = _ctxt->_code->ro(codeObject);
  _frames[_fp]->_name = _ctxt->_string->str(cp->m_name);
  std::cout << "Starting script: " << _frames[_fp]->_name << "\n";
  defineFrame(cp->m_names,cp->m_registers,false);
  _frames[_fp]->_sym_names = _ctxt->_list->empty(); // Our frame has no locally defined symbols
  _frames[_fp]->_fp = _fp; // Capture the frame pointer
  
  // execute the code
  executeCodeObject(codeObject);
  // No return values.. pop the frame
  _frames[_fp]->_sym_names = _ctxt->_list->empty();
  _frames[_fp]->_vars = _ctxt->_list->empty();
  _frames[_fp]->_addrs = _ctxt->_index->empty();
  _frames[_fp]->_defined = _ctxt->_bool->empty();
  _frames[_fp]->_exception_handlers.clear();
  /*
  {
    Object *regfile = _ctxt->_list->rw(_registers);
    for (int i=_frames[_fp]->_reg_offset;i<_rp;i++)
      {
	std::cout << "Clearing register " << i << "\n";
	regfile[i] = _ctxt->_double->empty();
      }
  }
  */
  _rp = _frames[_fp]->_reg_offset;
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

// s is the switch value to test, x is the values to test against
// copied from Algorithms.cpp
static inline Object TestForCaseMatch(ThreadContext *ctxt, const Object &s, const Object &x) {
  if (!(s.isScalar() || s.isString()))
    throw Exception("Switch argument must be a scalar or a string");
  // If x is a scalar, we just need to call the scalar version
  if (((x.typeCode() != TypeCellArray) && x.isScalar()) || x.isString())
    return ctxt->_bool->makeScalar(s == x);
  return ctxt->_bool->makeScalar(ctxt->_cell->indexOf(x,s) != -1);
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
  const CodeData *cp = _ctxt->_code->ro(codeObject);

  const Object *const_list = _ctxt->_list->ro(cp->m_consts);
  const uint64_t *code = _ctxt->_uint64->ro(cp->m_code);
  const Object *names_list = _ctxt->_list->ro(cp->m_names);

  int ip = 0;
  bool returnFound = false;
  _retscrpt_found = false;

  Frame *closed_frame = _frames[_fp]->_closedFrame;
  Object *regfile = _ctxt->_list->rw(_registers) + _frames[_fp]->_reg_offset;
  ndx_t *addrfile = _ctxt->_index->rw(_frames[_fp]->_addrs); // FIXME - Isn't this illegal? Do scripts have space allocated for an address file!!
  Object *varfile = _ctxt->_list->rw(closed_frame->_vars);
  Object *capturesfile = _ctxt->_list->rw(_frames[_fp]->_captures);
  std::vector<int> *eh = &_frames[_fp]->_exception_handlers;

  int save_ip;

  while (!returnFound)
    {
      try {
	while (!returnFound)
	  {
	    insn_t insn = code[ip];
	    
	    {
	      int8_t op = opcode(insn);
	      printf("%-15s",getOpCodeName(op).c_str());
	      printf("%-20s",Compiler::opcodeDecode(op,insn).c_str());
	      std::cout << "\n";
	    }

	    switch (opcode(insn))
	      {
	      case OP_DBUP:
		{
		  // TODO - this will not persist across statements issued in the debugger
		  for (int i=closed_frame->_fp-1;i>=0;--i)
		    if (_frames[i]->_closed)
		      {
			closed_frame = _frames[i];
			
			break;
		      }
		}
		break;
	      case OP_DBDOWN:
		break;
	      case OP_NOP:
		break;
	      case OP_RETURN:
		returnFound = true;
		break;
	      case OP_RETSCRPT:
		returnFound = true;
		_retscrpt_found = true;
		break;
	      case OP_PUSH:
		_ctxt->_list->push(REG1,REG2);
		break;
	      case OP_FIRST:
		REG1 = _ctxt->_list->first(REG2);
		REG2 = _ctxt->_list->pop(REG2);
		break;
	      case OP_CALL:
		REG1 = REG2.type()->call(REG2,_ctxt->_list->second(REG3),_ctxt->_double->scalarValue(_ctxt->_list->first(REG3)));
		break;
	      case OP_DCOLON:
		{
		  const Object *ap = _ctxt->_list->ro(REG2);
		  REG1 = ap[0].type()->DoubleColon(ap[0],ap[1],ap[2]);
		  break;
		}
	      case OP_LOAD_META:
		{
		  FMString classname = _ctxt->_string->str(REG2);
		  if (_ctxt->_globals->count(classname) == 0)
		    throw Exception("No metaclass defined with name:" + classname);
		  REG1 = _ctxt->_globals->at(classname);
		  if (!REG1.is(TypeMeta))
		    throw Exception("Name: " + classname + " is not associated with a class");
		  // FIXME - return a proxy object, or convert to a regular class at this point.
		  break;
		}
	      case OP_LOAD_CONST:
		REG1 = const_list[get_constant(insn)];
		break;
	      case OP_NEW_LIST:
		REG1 = _ctxt->_list->empty();
		break;
	      case OP_SUBSREF:
		_ctxt->_list->merge(REG1,REG2.type()->get(REG2,REG3,true));
		break;
	      case OP_SUBSREF_NOGS:
		_ctxt->_list->merge(REG1,REG2.type()->get(REG2,REG3,false));
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
	      case OP_NUMCOLS:
		REG1 = _ctxt->_double->makeScalar(REG2.count()/REG2.rows());
		break;
	      case OP_CASE:
		REG1 = TestForCaseMatch(_ctxt,REG2,REG3);
		break;
	      case OP_COLUMN:
		REG1 = REG2.type()->sliceColumn(REG2,_ctxt->_double->scalarValue(REG3));
		break;
	      case OP_NOT:
		UNARYOP(Not,"not");
		break;
		/*
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
	      case OP_LENGTH:
		REG1 = _ctxt->_double->makeScalar(REG2.count());
		break;
		/*
		  case OP_SUBSASGNM:
		  // FIXME
		  break;
		*/
	      case OP_ZERO:
		REG1 = _ctxt->_double->zeroScalar();
		break;
	      case OP_CLEAR:
		REG1 = _ctxt->_double->empty();
		break;
	      case OP_HCAT:
		REG1 = NCat(_ctxt,REG2,1);
		break;
	      case OP_VCAT:
		REG1 = NCat(_ctxt,REG2,0);
		break;
	      case OP_CELLROWDEF:
		REG1 = _ctxt->_list->makeScalar(makeCellFromList(_ctxt,REG2));
		break;
	      case OP_END:
		REG1 = _ctxt->_double->makeScalar(REG2.dims().dimension(_ctxt->_double->scalarValue(REG3)));
		break;
		/*
		  case OP_LOAD_GLOBAL:
		  case OP_LOAD_PERSIST:
		  break;
		*/
	      case OP_LOAD_CELL:
		{
		  register int ndx = get_constant(insn);
		  REG1 = _ctxt->_captured->get(capturesfile[ndx]);
		  std::cout << "    CELL LOAD: " << REG1.description() << "\n";
		  break;
		}
	      case OP_PUSH_CELL:
		{
		  register int ndx = get_constant(insn);
		  _ctxt->_list->push(REG1,capturesfile[ndx]);
		  std::cout << "    CELL PUSH: " << capturesfile[ndx].description() << "\n";
		  break;
		}
	      case OP_SAVE_CELL:
		{
		  register int ndx = get_constant(insn);
		  _ctxt->_captured->set(capturesfile[ndx],REG1);
		  std::cout << "    CELL SAVE: " << REG1.description() << "\n";
		  break;
		}
	      case OP_MAKE_CLOSURE:
		{
		  REG1 = _ctxt->_function->fromCode(REG2,REG3);
		  break;
		}
	      case OP_MAKE_ANONYMOUS:
		{
		  REG1 = _ctxt->_anonymous->build(REG2,REG3,closed_frame);
		  break;
		}
	      case OP_MAKE_FHANDLE:
		{
		  REG1 = _ctxt->_functionHandle->fromFunction(REG2);
		  break;
		}
	      case OP_GET_METHOD:
		{
		  if (REG2.is(TypeMeta)) 
		    // Case of a scoped pointer to a static method, i.e., Class::foo
		    REG1 = REG2.type()->getField(REG2,REG3);
		  else if (REG2.is(TypeClass))
		    REG1 = _ctxt->_class->getMethod(REG2,REG3);
		  break;
		}
	      case OP_LOAD:
		{
		  // We start by looking for the name in our address file
		  register int ndx = get_constant(insn);
		  register int addr = addrfile[ndx];
		  if (addr == -1)
		    {
		      std::cout << "OP_LOAD for " << _ctxt->_string->str(names_list[ndx]) << "\n";
		      // The address for this index has not been defined yet in the current scope.
		      // First, see if the closed frame has the address for it.  In the process, the 
		      // closed frame will search the global namespace for the symbol.
		      addr = closed_frame->lookupAddressForName(names_list[ndx],true);
		      if (addr == -1)
			throw Exception("Reference to undefined variable " + _ctxt->_string->str(names_list[ndx]));
		      addrfile[ndx] = addr;
		    }
		  REG1 = varfile[addr];
		  break;
		}
	      case OP_LOOKUP:
		{
		  register int ndx = get_constant(insn);
		  register int addr = addrfile[ndx];
		  if ((addr == -1) || _debug_mode)
		    {
		      std::cout << "OP_LOOKUP for " << _ctxt->_string->str(names_list[ndx]) << "\n";
		      // check for user classes
		      bool anyUserClasses = false;
		      const Object *regs = _ctxt->_list->ro(REG2);
		      for (int i=0;i<REG2.count();i++)
			anyUserClasses |= regs[i].isClass();
		      if (anyUserClasses) 
			{
			  std::cout << "Searching for " << names_list[ndx].description() << "\n";
			  for (int i=0;i<REG2.count();i++)
			    if (regs[i].isClass() && _ctxt->_class->hasMethod(regs[i],names_list[ndx],REG1)) 
			      {
				std::cout << "User method found\n";
				goto cont_lookup;
			      }
			}
		      std::cout << "No user defined classes\n";
		      // The address for this index has not been defined yet in the current scope.
		      // First, see if the closed frame has the address for it.  In the process, the 
		      // closed frame will search the global namespace for the symbol.
		      addr = closed_frame->lookupAddressForName(names_list[ndx],true);
		      if (addr == -1)
			throw Exception("Reference to undefined variable " + _ctxt->_string->str(names_list[ndx]));
		      addrfile[ndx] = addr;
		      // FIXME - if varfile[addr] is a variable, we can cache the address, otherwise, we can't
		    }
		  REG1 = varfile[addr];
		cont_lookup:
		  break;		  
		}
	      case OP_SAVE_GLOBAL:
		break;
	      case OP_SAVE:
		{
		  register int ndx = get_constant(insn);
		  register int addr = addrfile[ndx];
		  if ((addr == -1) || _debug_mode)
		    {
		      addr = closed_frame->lookupAddressForName(names_list[ndx],false);
		      if (addr == -1)
			{
			  addr = closed_frame->defineNewSymbol(names_list[ndx]);
			  varfile = _ctxt->_list->rw(closed_frame->_vars);
			}
		      addrfile[ndx] = addr;
		    }
		  std::cout << "Writing " << REG1.brief() << " to address " << addr << " at machine " << addrfile + ndx << "\n";
		  varfile[addr] = REG1;
		  break;
		}
	      case OP_SAVE_PERSIST:
	      case OP_JUMP_ZERO:
		{
		  if (REG1.asDouble() == 0)
		    ip = get_constant(insn)-1;
		  break;
		}
	      case OP_JUMP:
		{
		  ip = get_constant(insn)-1;
		  break;
		}
	      case OP_TRY_BEGIN:
		{
		  eh->push_back(get_constant(insn));
		  break;
		}
	      case OP_TRY_END:
		{
		  eh->pop_back();
		  break;
		}
	      case OP_STOP:
		{
		  debugCycle();
		  break;
		}
	      case OP_THROW:
		{
		  _exception = REG1;
		  std::cout << "Exception caught : " << _exception.description() << "\n";
		  if (!eh->empty())
		    {
		      ip = eh->back()-1;
		      eh->pop_back();
		    }
		  else
		    throw Exception(_exception.description()); // FIXME
		  break;
		}
	      case OP_PRINT:
		{
		  REG1.type()->print(REG1);
		  break;
		}
	      case OP_DEREF:
		{
		  REG1 = REG2.type()->deref(REG2);
		  break;
		}
	      case OP_SUBSASGN_GLOBAL:
	      case OP_SUBSASGN_PERSIST:
	      case OP_SUBSASGN_NOGS:
	      case OP_SUBSASGN:
		{
		  register int ndx = get_constant(insn);
		  register int addr = addrfile[ndx];
		  if ((addr == -1) || _debug_mode)
		    {
		      FMString name = _ctxt->_string->str(names_list[get_constant(insn)]);
		      addrfile[ndx] = closed_frame->getAddress(name);
		      if (addrfile[ndx] == -1)
			{
			  addrfile[ndx] = closed_frame->allocateVariable(name);
			  varfile = _ctxt->_list->rw(closed_frame->_vars);
			}
		      addr = addrfile[ndx];
		    }
		  varfile[addr].type()->set(varfile[addr],REG1,REG2,(opcode(insn) != OP_SUBSASGN_NOGS));
		  break;
		}
	      case OP_LOOPCOUNT:
		{
		  const Object *ap = _ctxt->_list->ro(REG2);
		  double last = _ctxt->_double->scalarValue(ap[2]);
		  double step = _ctxt->_double->scalarValue(ap[1]);
		  double first = _ctxt->_double->scalarValue(ap[0]);
		  REG1 = _ctxt->_double->makeScalar(num_for_loop_iter(first,step,last));
		  break;
		}
	      case OP_PUSH_INT:
		{
		  _ctxt->_list->push(REG1,_ctxt->_double->makeScalar(get_constant(insn)));
		  break;
		}
	      case OP_POP:
		{
		  std::cout << "POP for " << _ctxt->_double->scalarValue(REG2) << "\n";
		  for (int i=0;i<_ctxt->_double->scalarValue(REG2);i++)
		    REG1 = _ctxt->_list->pop(REG1);
		  break;
		}
	      case OP_CLASSDEF:
		{
		  defineClass(REG1,REG2);
		  break;
		}
	      default:
		{
		  std::cerr << "Unknown opcode " << int(opcode(insn)) << "\n";
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
      } catch (const Exception &e) {
	_exception = _ctxt->_string->makeString(e.msg());
	if (!eh->empty())
	  {
	    ip = eh->back();
	    eh->pop_back();
	  }
	else
	  throw;
      }
    }
}

