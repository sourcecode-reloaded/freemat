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
#include "LineNumbers.hpp"
#include "FileSystem.hpp"
#include "Debug.hpp"
#include "Globals.hpp"

std::string getOpCodeName(FM::op_t);

const int dbstop_if_error = 1;
const int dbstop_if_catch = 2;
const int dbstop_if_warning = 4;
const int dbstop_if_naninf = 8;
const int dbstop_on_entry = 16;

using namespace FM;

VM::FrameReserver::FrameReserver(VM *vm) : _vm(vm) {
  _vm->_fp++;
}

VM::FrameReserver::~FrameReserver() {
  assert(_vm->_fp > 0);
  _vm->_frames[_vm->_fp]->_sym_names = _vm->_ctxt->_list->empty();
  _vm->_frames[_vm->_fp]->_vars = _vm->_ctxt->_list->empty();
  _vm->_frames[_vm->_fp]->_addrs = _vm->_ctxt->_index->empty();
  _vm->_frames[_vm->_fp]->_defined = _vm->_ctxt->_bool->empty();
  _vm->_frames[_vm->_fp]->_exception_handlers.clear();
  _vm->_frames[_vm->_fp]->_debug_line_nos.clear();
  _vm->_frames[_vm->_fp]->_transient_bp = 0;
  _vm->_rp = _vm->_frames[_vm->_fp]->_reg_offset;
  _vm->_fp--;
}
				 
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
			      _exception(ctxt), _mybps(ctxt->_list->empty())
{
  for (int i=0;i<frame_stack_size;i++)
    _frames.push_back(new Frame(ctxt)); // FIXME - Bad way to do this?
  //  _frames.resize(frame_stack_size);
  _frames[0]->_name = "base";
  _frames[0]->_closed = true;
  _frames[0]->_closedFrame = _frames[0];
  _fp = 0;
  _rp = 0;
  _ctxt = ctxt;
  _try_depth = 0; // FIXME  - check if this is exception safe
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

void VM::updateDebugMode(bool loopEntry)
{
  if (loopEntry && (_frames[_fp]->_prevFrame->_state == FrameRunStateCode::StepIn)) {
    this->_debug_flags = dbstop_on_entry;
    this->_debug_mode = true;
    _frames[_fp]->_prevFrame->_state = FrameRunStateCode::Normal;
    _frames[_fp]->_prevFrame->_transient_bp = 0;
    return;
  }
  this->_debug_mode = _ctxt->_globals->isDefined("_dblist");
  this->_debug_flags = 0;
  // Make a local copy of the breakpoints - avoids the
  // need to lock on each check
  if (this->_debug_mode) {
    this->_mybps = _ctxt->_globals->get("_dblist",_ctxt);
    // Loop through the breakpoints, and set the flags
    const Object *bps = _ctxt->_list->ro(this->_mybps);
    for (int i=0;i<_mybps.count();i++) {
      const BreakpointData *bd = _ctxt->_breakpoint->ro(bps[i]);
      if (bd->bp_type == BreakpointTypeCode::Unconditional) {
	std::cout << "Checking for Entry: " << bd->frame_name << " " <<
	  _frames[_fp]->_debugname << " line " << bd->line_number << "\n";
	if ((bd->frame_name == _frames[_fp]->_debugname) && (bd->line_number == 0))
	  _debug_flags |= dbstop_on_entry;
      }
      if (bd->bp_type == BreakpointTypeCode::When) {
	switch(bd->bp_when) {
	case BreakpointWhen::Always:
	  break;
	case BreakpointWhen::Error:
	  _debug_flags |= dbstop_if_error;
	  break;
	case BreakpointWhen::CaughtError:
	  _debug_flags |= dbstop_if_catch;
	  break;
	case BreakpointWhen::Warning:
	  _debug_flags |= dbstop_if_warning;
	  break;
	case BreakpointWhen::NanInf:
	  _debug_flags |= dbstop_if_naninf;
	  break;
	}
      }
    }
  }
  std::cout << ">update debug mode to " << this->_debug_mode << "\n";
  std::cout << ">update debug flags to " << this->_debug_flags << "\n";
}

// Excuting a module is like executing a function, except that an additional stack is required
Object VM::executeModule(const Object &moduleObject, const Object &parameters)
{
  const ModuleData *cmd = _ctxt->_module->ro(moduleObject);
  std::cout << "Execute module : " << cmd->m_name << "\n";
  switch (cmd->m_modtype) {
  default:
	  throw Exception("Unknown module type!");
  case ScriptModuleType:
    {
      if (parameters.count() != 0)
	throw Exception("Cannot use arguments in call to a script");
      const FunctionData *fd = _ctxt->_function->ro(cmd->m_main);
      executeScript(fd->m_code,cmd->m_name);
      return _ctxt->_list->empty();
    }
  case ClassdefModuleType:
  case FunctionModuleType:
    {
      // Push the module object onto the stack
      _ctxt->_list->push(_modules,moduleObject);
      Object ret = executeFunction(_ctxt->_module->ro(moduleObject)->m_main,parameters,cmd->m_name);
      _modules = _ctxt->_list->pop(_modules);
      return ret;
    }
  case BuiltinModuleType:
    {
      // Check to see if there is a trap on this module
      _ctxt->_list->push(_modules,moduleObject);
      FrameReserver myframe(this);
      const ModuleData *md = _ctxt->_module->ro(moduleObject);
      _frames[_fp]->_name = md->m_name;
      _frames[_fp]->_debugname = md->m_name;
      defineFrame(_ctxt->_list->empty(),0,FrameTypeCode::Builtin);
      updateDebugMode(false);
      if (_debug_mode && (_debug_flags & dbstop_on_entry))
	{
	  _debug_flags ^= dbstop_on_entry;
	  debugCycle();
	}
      Object ret = _ctxt->_module->ro(moduleObject)->m_ptr(parameters,1,_ctxt);
      _modules = _ctxt->_list->pop(_modules);
      return ret;
    }
  }
}

void VM::defineClass(const Object &name, const Object &arguments)
{
  FMString className = _ctxt->_string->str(name);
  if (_ctxt->_globals->isDefined(className)) return;
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
      Object super_meta = _ctxt->_globals->get(_ctxt->_string->str(sp[i]),_ctxt);
      std::cout << "Defining class " << className << " super class " << super_meta.description() << "\n";
      _ctxt->_meta->addSuperClass(fooMeta,super_meta);
    }
  const Object *ep = _ctxt->_list->ro(events);
  for (int i=0;i<events.count();i++)
    _ctxt->_meta->addEvent(fooMeta,ep[i]);
  _ctxt->_globals->set(className,fooMeta);
}


void VM::defineFrame(const Object &names, int registerCount, const FrameTypeCode &ftype)
{
  auto nameCount = names.count();
  _frames[_fp]->_addrs = _ctxt->_index->makeMatrix(nameCount,1);
  ndx_t *ap = _ctxt->_index->rw(_frames[_fp]->_addrs);
  // FIXME - not right - parameters are defined when the function executes
  for (auto i=0;i<nameCount;i++) ap[i] = -1;
  _frames[_fp]->_defined = _ctxt->_bool->makeMatrix(nameCount,1);
  _frames[_fp]->_sym_names = names;
  _frames[_fp]->_vars = _ctxt->_list->makeMatrix(nameCount,1);
  _frames[_fp]->_exception_handlers.clear();
  _frames[_fp]->_reg_offset = _rp;
  _frames[_fp]->_module = _ctxt->_list->last(_modules);
  _frames[_fp]->_captures = _ctxt->_list->empty();
  _frames[_fp]->_type = ftype;
  _frames[_fp]->_closed = (ftype == FrameTypeCode::Function) ||
    (ftype == FrameTypeCode::Builtin);
  _frames[_fp]->_ip = 0;
  _frames[_fp]->_state = FrameRunStateCode::Normal;
  if (_frames[_fp]->_closed) _frames[_fp]->_closedFrame = _frames[_fp];
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
  _frames[_fp]->_prevFrame = _fp ? _frames[_fp-1] : nullptr;
  if (_frames[_fp]->_prevFrame) _frames[_fp]->_prevFrame->_nextFrame = _frames[_fp];
  _frames[_fp]->_nextFrame = nullptr;
  _rp += registerCount;
}

Object VM::executeAnonymousFunction(const Object &codeObject, const Object &parameters, const HashMap<Object> &captures)
{
  FrameReserver myframe(this);
  const CodeData *cp = _ctxt->_code->ro(codeObject);
  _frames[_fp]->_name = _ctxt->_string->str(cp->m_name);
  defineFrame(cp->m_names,cp->m_registers,FrameTypeCode::Function);
  _frames[_fp]->_code = codeObject;
  Object *sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  ndx_t *ap = _ctxt->_index->rw(_frames[_fp]->_addrs);
  // Populate the arguments
  // FIXME - need to only store the number of args and returns
  const Object * args = _ctxt->_list->ro(parameters);
  const ndx_t * param_ndx = _ctxt->_index->ro(cp->m_params);
  auto to_use = std::min<ndx_t>(parameters.count(),cp->m_params.count());
  for (auto i=0;i<to_use;i++)
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
  auto to_return = cp->m_returns.count();
  const ndx_t * return_ndx = _ctxt->_index->ro(cp->m_returns);
  sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  for (auto i=0;i<to_return;i++)
    _ctxt->_list->push(retvec,sp[return_ndx[i]]);
  return retvec;  
}

// Execute a function object, given a list of parameters (params).  Returns a list
// of returns.
Object VM::executeFunction(const Object &functionObject, const Object &parameters,
			   const FMString &debugname)
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
  FrameReserver myframe(this);
  const FunctionData *fd = _ctxt->_function->ro(functionObject);
  const CodeData *cp = _ctxt->_code->ro(fd->m_code);
  _frames[_fp]->_name = _ctxt->_string->str(cp->m_name);
  _frames[_fp]->_debugname = debugname;
  std::cout << "Starting function: " << _frames[_fp]->_name << "\n";
  defineFrame(cp->m_names,cp->m_registers,FrameTypeCode::Function);
  _frames[_fp]->_code = fd->m_code;
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
  ndx_t to_use;
  if (!varargin_case)
    to_use = std::min<ndx_t>(parameters.count(),cp->m_params.count());
  else
    to_use = std::min<ndx_t>(parameters.count(),cp->m_params.count()-1);
  std::cout << "to_use = " << to_use << "\n";
  for (auto i=0;i<to_use;i++)
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
      ndx_t to_push = std::max<ndx_t>(0,parameters.count()-to_use);
      Object varargin = _ctxt->_cell->makeMatrix(to_push,1);
      Object *vip = _ctxt->_cell->rw(varargin);
      for (auto i=0;i<to_push;i++)
	vip[i] = args[to_use+i];
      auto varargin_location = _ctxt->_index->scalarValue(cp->m_varargin);
      sp[varargin_location] = varargin;
      ap[varargin_location] = varargin_location;
    }
  // execute the code
  executeCodeObject(fd->m_code);
  // Collect return values
  Object retvec = _ctxt->_list->empty();
  auto to_return = cp->m_returns.count();
  bool varargout_case = _ctxt->_index->scalarValue(cp->m_varargout) != -1;
  if (varargout_case) to_return -= 1;
  const ndx_t * return_ndx = _ctxt->_index->ro(cp->m_returns);
  sp = _ctxt->_list->rw(_frames[_fp]->_vars);
  for (auto i=0;i<to_return;i++)
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
    auto varargout_location = _ctxt->_index->scalarValue(cp->m_varargout);
    const Object & varargout = sp[varargout_location];
    std::cout << "VARARGOUT " << varargout << "\n";
    const Object * vip = _ctxt->_cell->ro(varargout);
    for (auto i=0;i<varargout.count();i++)
      _ctxt->_list->push(retvec,vip[i]);
  }
  return retvec;
}


bool VM::checkBreakpoints(Frame *frame, Frame *closed_frame, int ip)
{
  int lineno = frame->mapIPToLineNumber(ip);
  int plineno = frame->mapIPToLineNumber(ip-1);
  if (plineno == lineno) return false;
  if ((frame->_state == FrameRunStateCode::StepOut)  ||
      (frame->_state == FrameRunStateCode::StepIn)) {
    frame->_state = FrameRunStateCode::Normal;
    return true;
  }
  // Loop through the transient breakpoints
  if (frame->_transient_bp == lineno) {
    frame->_transient_bp = 0;
    return true;
  }
  frame->_state = FrameRunStateCode::Normal;
  const Object *bpset = _ctxt->_list->ro(this->_mybps);
  std::set<int> todelete;
  for (int i=0;i<this->_mybps.count();i++) {
    const BreakpointData *bd = _ctxt->_breakpoint->ro(bpset[i]);
    std::cout << "Check BP: " << lineno << " name: " << bd->frame_name << " vs: " << frame->_debugname << " line: " << bd->line_number << "\n";
    if ((bd->frame_name == frame->_debugname) &&
	(bd->line_number == lineno)) {
      if (bd->bp_type == BreakpointTypeCode::Unconditional) return true;
      if (bd->bp_type == BreakpointTypeCode::Conditional) {
	std::cout << "Conditional breakpoint check.\n";
	try {
	  this->executeModule(bd->m_condition,_ctxt->_list->empty());
	  Object trap(_ctxt);
	  if (!closed_frame->getLocalVariableSlow("dbstop__",trap)) return false;
	  if ((trap.typeCode() == TypeBool) && (_ctxt->_bool->any(trap))) return true;
	  return false;
	} catch (Exception &) {
	  return false;
	}
      } else {
	return true;
      }
    }
  }
  return false;
}

void VM::debugCycle()
{
  FrameReserver myframe(this);
  activeFrame()->_name = "keyboard";
  defineFrame(_ctxt->_list->empty(),0,FrameTypeCode::Debug);
  // FIXME - this state should be a stack or something
  // FIXME - also need to preserve the debug context.
  // FIXME - keyboard in the base workspace dies
  // Find the frame we want to debug - why does it have to be closed?
  // FIXME - does this not work for scripts?
  Frame *visited_frame = nullptr;
  for (int i=_fp;i>=0;--i)
    if (_frames[i]->_closed) {
      visited_frame = _frames[i];
      break;
    }
  Frame *debug = _frames[_fp];
  while (debug && (debug->_type != FrameTypeCode::Function) && (debug->_type != FrameTypeCode::Script))
    debug = debug->_prevFrame;
  if (!debug) return; // Nothing to debug?
  int lineno = debug->mapIPToLineNumber(debug->_ip);
  char state[20];
  switch (debug->_state) {
  case FrameRunStateCode::Normal:
    sprintf(state,"N");
    break;
  case FrameRunStateCode::Debug:
    sprintf(state,"D");
    break;
  case FrameRunStateCode::StepIn:
    sprintf(state,"I");
    break;
  case FrameRunStateCode::StepOut:
    sprintf(state,"O");
    break;
  }
  char buffer[2048];
  while (1)
    {
      int shift = 0;
      Object dbshift(_ctxt);
      bool dbshiftExists = activeFrame()->getLocalVariableSlow("dbshift__",dbshift);
      if (dbshiftExists) shift = int(dbshift.asDouble());
      Frame *active = activeFrame()->_prevFrame;
      std::cout << "active frame name is " << active->_name << "\n";
      for (int i=0;i<shift;i++) {
	if (!active->_prevFrame) {
	  dbshift = _ctxt->_double->makeScalar(i-1);
	  activeFrame()->setVariableSlow("dbshift__",dbshift);
	  break;
	}
	active = active->_prevFrame;
	std::cout << "dbshift " << i << " to frame " << active->_name << "\n";
      }
      sprintf(buffer,"K [%s,%d (%d) <%d>] %s --> ",active->_name.c_str(),lineno,debug->_ip,shift,state);
      FMString body;
      if (!_ctxt->_io->getInputLine(FMString(buffer),body)) return;
      if (body.size() == 0) return;
      body += "\n\n";
      try {
	_ctxt->_compiler->compile(body);
	Module *mod = _ctxt->_compiler->module();
	if (mod)
	  {
	    Object p = _ctxt->_assembler->run(mod);
	    FrameReserver myframe(this);
	    const ModuleData *cmd = _ctxt->_module->ro(p);
	    const FunctionData *fd = _ctxt->_function->ro(cmd->m_main);
	    const CodeData *cp = _ctxt->_code->ro(fd->m_code);
	    _frames[_fp]->_name = ">>debug";
	    std::cout << "Starting script: " << _frames[_fp]->_name << "\n";
	    defineFrame(cp->m_names,cp->m_registers,FrameTypeCode::Script);
	    _frames[_fp]->_sym_names = _ctxt->_list->empty(); // Our frame has no locally defined symbols
	    //	    _frames[_fp]->_closedFrame = visited_frame;
	    _frames[_fp]->_closedFrame = active->_closedFrame;
	    // execute the code
	    try {
	      executeCodeObject(fd->m_code);
	    } catch (const FM::Exception &e) {
	      std::cout << "Exception: " << e.msg() << "\n";
	    }
	    updateDebugMode(false);
	    //	    visited_frame = _frames[_fp]->_closedFrame;
	    if (_retscrpt_found) return;
	  }
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      } catch (const VMDBQuitException &x) {
	if (x._catchCount == -1) throw;
	if (x._catchCount == 0) throw VMDBQuitException(1);
      }
    }
}

Frame* VM::findPreviousClosedFrame(Frame *p) {
  // TODO - this will not persist across statements issued in the debugger
  // Find the current frame
  int cfp = 0;
  for (int i=0;i<_fp;i++)
    if (_frames[i] == p) {
      cfp = i;
      break;
    }
  for (int i=cfp-1;i>=0;--i)
    if (_frames[i]->_closed) return _frames[i];
  throw Exception("Cannot go up");
}

Frame* VM::findNextClosedFrame(Frame *p) {
  int cfp = 0;
  for (int i=0;i<_fp;i++)
    if (_frames[i] == p) {
      cfp = i;
      break;
    }
  for (int i=cfp+1;i<=_fp;i++)
    if (_frames[i]->_closed) return _frames[i];
  throw Exception("Cannot go down");
}

void VM::executeScript(const Object &codeObject, const FMString &debugname)
{
  FrameReserver myframe(this);
  const CodeData *cp = _ctxt->_code->ro(codeObject);
  _frames[_fp]->_name = get_basename_from_full_path(debugname);
  _frames[_fp]->_debugname = debugname;
  std::cout << "Starting script: " << _frames[_fp]->_name << " " << _frames[_fp]->_debugname << "\n";
  defineFrame(cp->m_names,cp->m_registers,FrameTypeCode::Script);
  _frames[_fp]->_code = codeObject;
  _frames[_fp]->_sym_names = _ctxt->_list->empty(); // Our frame has no locally defined symbols
  // execute the code
  executeCodeObject(codeObject);
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

  int &ip = _frames[_fp]->_ip;
  bool returnFound = false;
  _retscrpt_found = false;

  Frame* &closed_frame = _frames[_fp]->_closedFrame;
  Object *regfile = _ctxt->_list->rw(_registers) + _frames[_fp]->_reg_offset;
  ndx_t *addrfile = _ctxt->_index->rw(_frames[_fp]->_addrs); 
  Object *varfile = _ctxt->_list->rw(closed_frame->_vars);
  Object *capturesfile = _ctxt->_list->rw(_frames[_fp]->_captures);
  std::vector<int> *eh = &_frames[_fp]->_exception_handlers;

  updateDebugMode(true);
  
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
	    if (_debug_mode && checkBreakpoints(_frames[_fp],closed_frame,ip)) {
	      debugCycle();
	    }
	    if (_debug_mode && (_debug_flags & dbstop_on_entry)) {
	      _debug_flags = _debug_flags ^ dbstop_on_entry;
	      debugCycle();
	    }

	    switch (opcode(insn))
	      {
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
		REG1 = REG2.type()->call(REG2,_ctxt->_list->second(REG3),ndx_t(_ctxt->_double->scalarValue(_ctxt->_list->first(REG3))));
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
		  if (_ctxt->_globals->isDefined(classname))
		    throw Exception("No metaclass defined with name:" + classname);
		  REG1 = _ctxt->_globals->get(classname,_ctxt);
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
		REG1 = _ctxt->_double->makeScalar(double(REG2.count()/REG2.rows()));
		break;
	      case OP_CASE:
		REG1 = TestForCaseMatch(_ctxt,REG2,REG3);
		break;
	      case OP_COLUMN:
		REG1 = REG2.type()->sliceColumn(REG2,ndx_t(_ctxt->_double->scalarValue(REG3)-1));
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
		REG1 = _ctxt->_double->makeScalar(_ctxt->_double->scalarValue(REG1)+1);
		break;
	      case OP_LENGTH:
		REG1 = _ctxt->_double->makeScalar(double(REG2.count()));
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
		REG1 = _ctxt->_double->makeScalar(double(REG2.rodims().dimension(ndx_t(_ctxt->_double->scalarValue(REG3)))));
		break;
		/*
		  case OP_LOAD_GLOBAL:
		  case OP_LOAD_PERSIST:
		  break;
		*/
	      case OP_LOAD_CELL:
		{
		  int ndx = get_constant(insn);
		  REG1 = _ctxt->_captured->get(capturesfile[ndx]);
		  std::cout << "    CELL LOAD: " << REG1.description() << "\n";
		  break;
		}
	      case OP_PUSH_CELL:
		{
		  int ndx = get_constant(insn);
		  _ctxt->_list->push(REG1,capturesfile[ndx]);
		  std::cout << "    CELL PUSH: " << capturesfile[ndx].description() << "\n";
		  break;
		}
	      case OP_SAVE_CELL:
		{
		  int ndx = get_constant(insn);
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
		  int ndx = get_constant(insn);
		  ndx_t addr = addrfile[ndx];
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
		  int ndx = get_constant(insn);
		  ndx_t addr = addrfile[ndx];
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
		  int ndx = get_constant(insn);
		  ndx_t addr = addrfile[ndx];
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
		  _try_depth++;
		  break;
		}
	      case OP_TRY_END:
		{
		  eh->pop_back();
		  _try_depth--;
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
		      _try_depth--;
		      if ((_debug_flags & dbstop_if_catch) != 0) {
			debugCycle();
		      }
		    }
		  else
		    {
		      if (((_debug_flags & dbstop_if_error) != 0)  && (_try_depth == 0)) {
			debugCycle();
		      }
		      throw Exception(_exception.description()); // FIXME
		    }
		  break;
		}
	      case OP_PRINT:
		{
		  REG1.type()->print(REG1);
		  break;
		}
	      case OP_PRINTNE:
		{
		  if (!REG1.isEmpty())
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
		  int ndx = get_constant(insn);
		  ndx_t addr = addrfile[ndx];
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
	    _try_depth--;
	    if ((_debug_flags & dbstop_if_catch) != 0) {
	      debugCycle();
	    }
	  }
	else
	  throw;
      }
    }
}

