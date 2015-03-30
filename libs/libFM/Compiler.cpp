#include "Compiler.hpp"
#include "Assembler.hpp"
#include <iostream>
#include "ThreadContext.hpp"
#include "TypeUtils.hpp"
#include "AllTypes.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "EndRemover.hpp"
#include "NestedFunctionMover.hpp"
#include "LineNumbers.hpp"

//#include "Algorithms.hpp"
//#include "Print.hpp"
//#include "Math.hpp"
//#include "Struct.hpp"

using namespace FM;

// Assembler issues.
// 
// It doesn't quite work correctly yet...
//
// If local functions are not visible outside a file scope, why do we bother
// returning them?
// 
// So consider a translation unit foo.m
// with multiple functions in it:
//
// function foo(a,b)
//  blah
//  blah
//  
// function localfoo1
//
// function localfoo2
//
// In this case, localfoo1 and localfoo2 are _not_ visible outside
// the scope of foo.  So they could easily be considered constants
// that were local to foo.  
//
// One option is to dump them all into the parent scope (FM4 approach).
// This is ugly.  A better approach is to create the notion of a module.
//
// The Pythonic approach is to model a module as a code block.  In that
// case, the module itself is a function.  So in the above case, we would
// have foo.m compile into a single code block that contained code like:
//
// load r0,const[0] ; // code object for foo.m
// make_function r0
// save_dynamic r0,name[0]
// load r1,const[1] ; // code object for localfoo1
// make_function r1
// save_dynamic r1,name[1]
//
// This works because in Python, we actively import a code block via
// the "import" statement.  In FreeMat, there is no explicit import.
//
// Can local functions be resolved at compile time?  No - because there is no 
// way to guarantee that those references are actual functions.  Consider
//
// function top
//   load g.mat
//   x = local1
//
// function y = local1
//   y = 32
//
// In this case the reference to local1 is not mapped to a function call, but
// rather (possibly) a local variable inserted into the workspace by the load
// call.
//
// We could have a function carry references to local functions.  So 
// a code_block has a set of local (scoped) functions that are included.
//
// 
//struct CodeBlock
//{
//  SymbolTable *_syms;
//  std::vector<BasicBlock*> _blocklist;
//  BasicBlock* _currentBlock;
//  FMStringList _capturedlist;
//  FMStringList _freelist;
//  FMStringList _varlist;
//  FMStringList _namelist;
//  FMVector<Array> _constlist;
//};
//
//struct CodeObject (stored as an Array)
//{
//  code - the opcode sequence
//  captured - list of names of captured variables
//  free - list of names of free variables
//  varlist - list of local variables
//  namelist - list of names (dynamic variables)
//  constlist - list of constant objects (arrays)
//  localfuncs - map of code objects in our scope
//};


// More trouble with scripts...
// My plan was to use OP_SAVE_DYNAMIC - That will work, except for that
// we need to make sure that OP_SAVE_DYNAMIC doesn't "enable" local variables
// for scripts.  So for example, consider something like:
//
// a = svd(b);
// svd = a;
// c = svd;
//
// In a function scope, we will (assuming svd is not declared prior to the first
// line), declare svd as DYNAMIC.  The SAVE_DYNAMIC means svd is now a local
// variable in the function, and the last operation will read from the local
// variable.  We do not want this to happen.  So when the VM interprets a
// codeblock corresponding to a script, it must have a flag that says 
// "SAVE_DYNAMIC_CREATES_LOCALVARS --> false", or equivalent.
//
// 

// Next step - at this point, things are looking pretty good.  I would like
// to complete the assembler and the generation of a code object.

// Some optimization passes would be nice:
//  

// So far things are looking pretty reasonable.  The register free problem is fixed,
// the dynamic function call mechanism is working reasonably well.  Next step is 
// probably to fix the interpreted mode (so that I can do something with it),
// patch the VM mechanism, etc.
//
// On the other hand, while still in Compiler space, it would be good to deal with
// closures. and try/catch exceptions
//
// Exceptions...
//
// Exceptions are a dynamic function - because there is no way to know at compile 
// time which except block will actually handle the exception - it may be 
// outside the current function.
// 
// Can we have something like:
//  OP_TRY_START  <offset> (like a jump - takes an offset of the catch instruction)
//                          - pushes the offset onto a runtime stack (VM responsibility)
//  OP_TRY_END              - pops the runtime stack
//  OP_RAISE <reg_t>        - raises an exception
//                            at run time, the VM pops the runtime stack and jumps
//                            to that address.  If the runtime stack is empty, 
//                            the function must propogate the exception...
// 
// 

// Assembler.

// Time to write the assembler pass.  To do that, we need to make sure that
// we can walk the CFG (which has loops).  To do this, we have to visit the
// blocks in postorder.  


// nested functions 
// if we create a mechanism to convert a function into an array
//
// Before that, we need to collapse the CFG --> code, this is the assemble step
//
// TODO - Make sure function calls work
// TODO - SAVE_OP does it work for all types (persistant, global, etc...)?

// Understanding cells.

// If a variable is a cell, then we use different opcodes to load and store from it.
// Cells are associated with the parent scope.  So if we have
//  f3
//     f2
//        g
// And the symbol a is used by g, then a will be a cell for f3.

// A variable is "free" if it is bound in a containing scope.  I read
// that a bunch of times, but didn't get it.  Now I do.  So freevar in scope x
// is equivalent to a cellvar in scope y where scope y contains scope x.

// names are for variables that couldn't be bound (determined) or are global
// freevars are for variables that are bound 
// varnames are for local variables (includes parameters)
// consts are for constant values
// code is the actual bytecode to execute
// cellvars are for variables that are captured by nested scopes
// How are parameter handled?

// Need to refactor symbol analysis pass to change "free" to mean bound in containing
// scope.  And "captured" to mean referenced in a contained scope.

// For FreeMat, we have some tweaks.
// - global_vars - list of global variables
// - persistent_vars - list of persistent variables
// - local_vars - list of local variables
// - consts - list of constants
// - captured_vars - list of captured variables
// - imported_vars - list of variables imported from a captured workspace
// - dynamic_names - list of names that must be resolved dynamically (at run time)
// - funclist - list of functions

/*
 Can functions be bound at compile time?
 Consider the simple case of 
  a = fun(b)
 or even
  a = fun
 In both cases, fun will appear as used, with no definition.  This makes fun
 an implicit global symbol.  But implicit global symbols cannot be variables
 inside a function.  So fun must be a function.  The logic is thus:

 In function:
   used (no localdef, or any other flags) --> it's a function or a script.
 In script:
   used --> could be anything.

 The difficulty with scripts is that the opcode to be emitted depends on 
 exactly what the thing is.  So, for example, in the case of a function,
 if we know that foo is a function, then the expression
   a = foo
 triggers an OP_CALL opcode.  But in a script, we do not even know that.
 If the parent scope of the script has defined foo as a variable, this
 operation becomes a simple copy.

 That leads to two possibilities -- one option is to compile the script
 with super-generic op codes, so that even something like
   a = foo
 can dynamically trigger a function call. We could do something like
  OP_LOAD_DYNAMIC rx, name
 Which will perform a runtime lookup of "name", and then if it's a 
 function, will call it.  

 The problem is that M language is very vague and doesn't allow us to
 distinguish between function calls and variable dereferences easily.
 Consider for example, the following:

   a = foo(32)

 If we don't know what foo is, how do we emit opcodes for it?  If we
 use  OP_LOAD_DYNAMIC rx,foo followed by an OP_SUBSREF ry,rx because
 we assume it is a variable, the behavior is incorrect for foo a function.
 Furthermore, we can no longer make OP_LOAD_DYNAMIC invoke functions.

 A more defensive strategy would be to force OP_LOAD_DYNAMIC to load
 a function pointer for functions, and then introduce a second opcode
 to force the result to be a variable.  So something like "a = foo"
 resolves to 
    
  OP_LOAD_DYNAMIC rx, name   <- loads a function pointer 
  OP_DEREF_DYNAMIC rx,ry     <- invokes function pointer with no arguments, does nothing for variables

 Conversely, a call like "a = foo(32)" resolves to

  OP_LOAD_DYNAMIC rx, name   
  OP_PUSH         32
  ...
  OP_SUBSREF      rx

  Which piggy backs on the existing subsref overloading for function pointers.

  What about script invokations?  In a function an isolated statement like

  foo;

  can be assumed to be a function call or a script.  But in a script, it may be
  just the name of a variable in the enclosing scope.  To support this construct
  with the same opcodes, we need to be able to construct a function pointer to
  a script.  This is technically possible, but not supported today.  In fact, it's
  a bug, as it gives a script function scope.  Not the intent.  Yuck.

  The option is to treat scripts like macros and compile them in the scope of the
  function they are being called from.  This is how the JIT works.  In a sense
  that could be a performance problem, as the introduction of a script requires 
  recompilation of the function.  And it doesn't solve the "eval" problem either.

  So the cleanest solution is to press on with the OP_LOAD_DYNAMIC and OP_DEREF_DYNAMIC

  What about dynamic name introduction?  So suppose we have something like

  function foo()
    load data.mat
    p = svd(3)
  end

  Now in this case, we cannot assume that svd is a function.  It may in fact be
  a variable introduced by the "load" statement.  Or, we may have 

  function foo()
    eval("svd = 1:10")
    p = svd(3)
  end

  which means that all unresolved names must map to OP_LOAD_DYNAMIC.  There cannot
  be anything that knows a function is actually referenced at run time.

  There are two exceptions.  The first is multi-valued function calls:

  [u,s,v] = svd(a)

  In this case, svd _must_ be a function.  The second is "special syntax"

  hold on

  in which case "hold" must be a function.


// For FreeMat, we have some tweaks.
// - global_vars - list of global variables
// - persistent_vars - list of persistent variables
// - local_vars - list of local variables
// - consts - list of constants
// - captured_vars - list of captured variables
// - imported_vars - list of variables imported from a captured workspace
// - dynamic_names - list of names that must be resolved dynamically (at run time)

  So we can eliminate funclist completely.  All unresolved names must be resolved at
  run time with dynamic ops.  

  The rules for assessing symbols are as follows:
  GLOBAL:
    if USED | LOCAL, and then GLOBAL --> is this a rebinding of a variable that is
    already in use, we have something like this ( a dynamic rebinding of the
    variable )

  function foo()
    a = 32
    global a
  end

  We can simply fail to support this - It is a warning in M language.  For us, it
  will be a syntax error.

  GLOBAL and PERSISTENT - error - cannot be both

  - TODO - add an opcode to load an integer into a register, so that 0, 1, 2, ... are not stored as unique constants
  - Change idx_t to const_t 
  - Triple opcodes should use indirect references to registers

*/




// Code object
//    - opcodes
//    - constants
//    - names
//    - local variable names
//    - free variable names
//    - line number table
//    - filename
//    - cell variable list (captured variables)
//    - name

// A frame contains:
//   code - the code segment - which includes
//   builtins - builtin symbol table
//   globals - global symbol table
//   locals  - local symbol table
//   valuestack
//   lineno
//   blockstack

//TODO - 
//   Finish code generation (function calls, flow control, etc.)
//   END handling

#include "OpCodes.cpp"


std::string Compiler::opcodeDecode(op_t opcode, insn_t val)
{
  switch (getOpCodeMode(opcode))
    {
    case no_arguments:
      return "";
    case one_register:
      return "r" + Stringify(reg1(val));
    case two_registers:
      return "r" + Stringify(reg1(val)) + ", r" + Stringify(reg2(val));
    case three_registers:
      return "r" + Stringify(reg1(val)) + ", r" + Stringify(reg2(val)) + ", r" + Stringify(reg3(val));
    case register_constant:
      return "r" + Stringify(reg1(val)) + ", Const[" + Stringify(get_constant(val)) +"]";
    case register_int:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(get_constant(val));
    case register_register_name:
      return "r" + Stringify(reg1(val)) + ", r" + Stringify(reg2(val)) + ", Name[" + Stringify(get_constant(val)) + "]";
    case register_cell:
      return "r"+ Stringify(reg1(val)) + ", Cell[" + Stringify(get_constant(val)) + "]";
    case register_name:
      return "r" + Stringify(reg1(val)) + ", Name[" + Stringify(get_constant(val)) + "]";
    case register_offset:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(get_constant(val));
    case offset:
    case constant:
      return Stringify(get_constant(val));
    default:
      return "unknown";
    }
}

void Compiler::freeRegister(int index)
{
  Instruction i;
  i._opcode = OP_CLEAR | (index << shift_reg1);
  i._target = 0;
  i._position = _currentLineNo;
  if ((_code) && (_code->_currentBlock))
    _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::useBlock(BasicBlock *p)
{
  _code->_blocklist.push_back(p);
  _code->_currentBlock = p;
}


void Compiler::emit(int8_t opcode, reg_t reg1, BasicBlock *blk)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << shift_reg1);
  i._target = blk;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, BasicBlock *blk)
{
  Instruction i;
  i._opcode = opcode;
  i._target = blk;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2, reg_t reg3)
{
  Instruction i;
  i._opcode = opcode | (insn_t(reg1->num()) << shift_reg1) | (insn_t(reg2->num()) << shift_reg2) | (insn_t(reg3->num()) << shift_reg3);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2, const_t ndx)
{
  Instruction i;
  i._opcode = opcode | (insn_t(reg1->num()) << shift_reg1) | (insn_t(reg2->num()) << shift_reg2) | (insn_t(ndx) << shift_constant);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2)
{
  Instruction i;
  i._opcode = opcode | (insn_t(reg1->num()) << shift_reg1) | (insn_t(reg2->num()) << shift_reg2);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1)
{
  Instruction i;
  i._opcode = opcode | (insn_t(reg1->num()) << shift_reg1);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}


void Compiler::emit(int8_t opcode, reg_t reg1, const_t arg)
{
  Instruction i;
  i._opcode = opcode | (insn_t(reg1->num()) << shift_reg1) | (insn_t(arg) << shift_constant);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, const_t arg)
{
  Instruction i;
  i._opcode = opcode | (insn_t(arg) << shift_constant);
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode)
{
  Instruction i;
  i._opcode = opcode;
  i._target = 0;
  i._position = _currentLineNo;
  _code->_currentBlock->_insnlist.push_back(i);
}

// Function call
// Stack:
// nargout
//

// To store a vector to the stack, we need to do the following:
// sp <- current_stack pointer
// push item
// push item
// push item
// push current_stack pointer - sp
// This creates a list of items on the stack followed by a count
// of how many items were written.  I.e., a list of items.

// Remove the end references with AST rewriting...
//
// Calls a function and then returns a pointer to the base
// of the return args on the stack
reg_t Compiler::doFunctionExpression(const Tree &t, reg_t narg_out) {
  reg_t sp = startList();
  pushList(sp,narg_out);
  FMString funcname = t.first().text();
  if ((t.numChildren() >= 2) && t.second().is(TOK_PARENS)) {
    const Tree &s(t.second());
    for (int p=0;p<s.numChildren();p++)
      multiexpr(sp,s.child(p));
  }
  reg_t rp = getRegister();
  //  emit(OP_CALL,rp,sp,getNameID(funcname));
  return rp;
}

reg_t Compiler::getRegister()
{
  return _regpool->getRegister();
}

reg_t Compiler::doBinaryOperator(const Tree &t, op_t opcode) {
  reg_t r1 = expression(t.first());
  reg_t r2 = expression(t.second());
  reg_t r0 = getRegister();
  emit(opcode,r0,r1,r2);
  return r0;
}

reg_t Compiler::doUnaryOperator(const Tree &t, op_t opcode) {
  reg_t r1 = expression(t.first());
  reg_t r0 = getRegister();
  emit(opcode,r0,r1);
  return r0;
}

reg_t Compiler::doubleColon(const Tree &t) {
  reg_t args = startList();
  pushList(args,expression(t.first().first()));
  pushList(args,expression(t.first().second()));
  pushList(args,expression(t.second()));
  reg_t r0 = getRegister();
  emit(OP_DCOLON,r0,args); // Pops args 0,1,2 off of top of stack
  return r0;
}

const_t Compiler::getCellID(const FMString & t) {
  ndx_t nd1 = indexOfStringInList(_ctxt,_code->_freelist,t);
  if (nd1 != -1) return nd1;
  nd1 = indexOfStringInList(_ctxt,_code->_capturedlist,t);
  if (nd1 == -1) throw Exception("Cannot find cell ID for " + t);
  nd1 += _code->_freelist.count();
  return nd1;
}

const_t Compiler::getNameID(const FMString & t) {
  ndx_t n = indexOfStringInList(_ctxt,_code->_namelist,t);
  if (n == -1)
    throw Exception("Internal compiler error - encountered unexpected name '" + t + "'");
  return n;
}

const_t Compiler::getConstantID(const FMString & t) {
  return getConstantID(_ctxt->_string->makeString(t));
}

const_t Compiler::getConstantID(const Object & t) {
  ndx_t ndx = _ctxt->_list->indexOf(_code->_constlist,t);
  if (ndx < 0)
    {
      _ctxt->_list->push(_code->_constlist,t);
      return _code->_constlist.count()-1;
    }
  return ndx;
}

void Compiler::saveRegisterToName(const FMString &varname, reg_t b) {
  symbol_flags_t symflags = _code->_syms->syms[varname];
  if (symflags.is_global())
    emit(OP_SAVE_GLOBAL,b,getNameID(varname));
  else if (symflags.is_persistent())
    emit(OP_SAVE_PERSIST,b,getNameID(varname));
  else if (symflags.is_local())
    emit(OP_SAVE,b,getNameID(varname));
  else if (symflags.is_cell())
    {
      emit(OP_SAVE_CELL,b,getCellID(varname));
      std::cout << "CELL " << varname << " has ID " << getCellID(varname) << "\n";
    }
  else
    throw Exception("Unhandled save case for variable " + varname + " flags = " + symflags.str());
}

// FIXME - 
// Requires more arguments than are available.
// SUBSASGN A(list) = b
// means I need to access 3 pieces of info in the op
// namely: A, list, and b
// Proposed solution - let b be at the end of the list
void Compiler::assignment(const Tree &t, bool printIt, reg_t b) {
  FMString varname = t.first().text();
  if (t.numChildren() == 1)
    {
      saveRegisterToName(varname,b);
      if (printIt) emit(OP_PRINT,b);
      return;
    }
  // Check for obj.property 
  reg_t args = flattenDereferenceTreeToStack(t,1);
  symbol_flags_t symflags = _code->_syms->syms[varname];
  bool use_overloads = true;
  if (_code->_isgetset && symflags.is_object() &&
      t.second().is('.') && (t.second().first().text() == _code->_propertyName))
    use_overloads = false;
  if (_code->_issubsasgn && symflags.is_object())
    use_overloads = false;
  if (_code->_issubsasgn) {
    std::cout << "subsasgn case\n";
  }
  if (symflags.is_global())
    emit(OP_SUBSASGN_GLOBAL,args,b,getNameID(varname));
  else if (symflags.is_persistent())
    emit(OP_SUBSASGN_PERSIST,args,b,getNameID(varname));
  else if (symflags.is_local())
    {
      if (use_overloads)
	emit(OP_SUBSASGN,args,b,getNameID(varname));
      else
	emit(OP_SUBSASGN_NOGS,args,b,getNameID(varname));
    }
  else
    throw Exception("Unhandled subsasgn case");
  if (printIt)
    emit(OP_PRINT,fetchVariable(varname,symflags));
}

// We want to map a dereference tree to a list when the first child
// is parens (), and therefore has been precomputed
reg_t Compiler::flattenDereferenceTreeToStackParensPrecomputed(const Tree &s, reg_t paren_children, int first_child)
{
  reg_t x = startList();
  emit(OP_PUSH_INT,x,const_t(0));
  pushList(x,paren_children);
  for (int index = first_child; index < s.numChildren();index++)
    deref(x,s.child(index));
  return x;
}

// We want to map a dereference tree to the stack.
reg_t Compiler::flattenDereferenceTreeToStack(const Tree &s, int first_child) {
  reg_t x = startList();
  for (int index = first_child; index < s.numChildren();index++)
    deref(x,s.child(index));
  return x;
}

void Compiler::deref(reg_t args, const Tree &s) {
  switch (s.token())
    {
    case TOK_PARENS:
      {
	emit(OP_PUSH_INT,args,const_t(0));
	reg_t x = startList();
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(x,s.child(p));
	pushList(args,x);
	break;
      }
    case TOK_BRACES:
      {
	emit(OP_PUSH_INT,args,const_t(1));
	reg_t x = startList();
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(x,s.child(p));
	pushList(args,x);
	break;
      }
    case '.':
      {
	emit(OP_PUSH_INT,args,const_t(2));
	pushList(args,fetchConstantString(s.first().text()));
	break;
      }
    case TOK_DYN:
      {
	emit(OP_PUSH_INT,args,const_t(2));
	pushList(args,expression(s.first()));
	break;
      }
    default:
      throw Exception("Unknown expression!" + s.str());
    }
}

reg_t Compiler::fetchConstant(const Object &constant)
{
  reg_t r0 = getRegister();
  emit(OP_LOAD_CONST,r0,getConstantID(constant));
  return r0;
}

reg_t Compiler::fetchEmpty()
{
  return fetchConstant(_ctxt->_double->empty());
}

reg_t Compiler::fetchConstantBool(bool t)
{
  return fetchConstant(_ctxt->_bool->makeScalar(t));
}

reg_t Compiler::fetchConstantString(const FMString &constant)
{
  return fetchConstant(_ctxt->_string->makeString(constant));
}

reg_t Compiler::fetchCell(const FMString &cellname)
{
  reg_t x = getRegister();
  emit(OP_LOAD_CELL,x,getCellID(cellname));
  return x;
}

reg_t Compiler::makeHandle(reg_t funcobj)
{
  reg_t x = getRegister();
  emit(OP_MAKE_FHANDLE,x,funcobj);
  return x;
}

reg_t Compiler::fetchClosure(const FMString &varname)
{
  // Fetching a closure is complex work.  
  SymbolTable* closure_sym = _currentSym->childNamed(varname);
  if (!closure_sym) throw Exception("Compiler error - unable to find symbol table for closure " + varname);
  reg_t closure = startList();
  for (auto s = closure_sym->syms.constBegin(); s != closure_sym->syms.constEnd(); ++s)
    {
      // For the given closure variable - two things are possible.  It might be 
      // captured and well defined, or it might be undefined.  In the former case,
      // we just pass the cell to the closure.
      if (s.value().is_free())
	emit(OP_PUSH_CELL,closure,getCellID(s.key()));
    }
  reg_t func = fetchConstant(_ctxt->_string->makeString("#"+varname));
  reg_t fptr = getRegister();
  emit(OP_MAKE_CLOSURE,fptr,func,closure);
  return fptr;
}

reg_t Compiler::fetchVariable(const FMString &varname, symbol_flags_t flags)
{
  reg_t x = getRegister();
  std::cout << "Fetch variable: " << varname << " with flags: " << flags.str() << "\n";
  if (flags.is_global())
    emit(OP_LOAD_GLOBAL,x,getNameID(varname));
  else if (flags.is_persistent())
    emit(OP_LOAD_PERSIST,x,getNameID(varname));
  else if (flags.is_local())
    emit(OP_LOAD,x,getNameID(varname));
  else if (flags.is_cell())
    emit(OP_LOAD_CELL,x,getCellID(varname));
  else if (flags.is_nested())
    x = fetchClosure(varname);
  else
    throw Exception("Unhandled load case for variable " + varname + " flags = " + flags.str());
  return x;
}

reg_t Compiler::fetchVariableOrFunction(const FMString & varname, reg_t args)
{
  // We need to find a variable (or possibly a function)
  // To find the right one, we need to know the arguments first
  // This requires a different OPCODE.
  reg_t x = getRegister();
  emit(OP_LOOKUP,x,args,getNameID(varname));
  return x;
}

reg_t Compiler::startList()
{
  reg_t x = getRegister();
  emit(OP_NEW_LIST,x);
  return x;
}

void Compiler::pushList(reg_t list, reg_t x)
{
  emit(OP_PUSH,list,x);
}

reg_t Compiler::doShortcutOr(const Tree &t) {
  reg_t ret = getRegister();
  BasicBlock *fail1 = new BasicBlock;
  BasicBlock *fail2 = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,expression(t.first()),fail1);
  emit(OP_LOAD_CONST,ret,getConstantID(_ctxt->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail1);
  emit(OP_JUMP_ZERO,expression(t.second()),fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(_ctxt->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(_ctxt->_bool->makeScalar(false)));
  emit(OP_JUMP,end);
  useBlock(end);
  return ret;
}


reg_t Compiler::doShortcutAnd(const Tree &t) {
  reg_t ret = getRegister();
  BasicBlock *fail = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,expression(t.first()),fail);
  emit(OP_JUMP_ZERO,expression(t.second()),fail);
  emit(OP_LOAD_CONST,ret,getConstantID(_ctxt->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail);
  emit(OP_LOAD_CONST,ret,getConstantID(_ctxt->_bool->makeScalar(false)));
  emit(OP_JUMP,end);
  useBlock(end);
  return ret;
}

// Takes an expression like A(...).foo{32} and
// expands it (possibly into multiple values).  They
// are left on the stack.  Caller is responsible
// for removing them.
void Compiler::rhs(reg_t list, const Tree &t) {
  FMString varname = t.first().text();
  reg_t x;
  symbol_flags_t symflags = _code->_syms->syms[varname];
  if (t.numChildren() > 1)
    {
      bool use_overloads = true;
      // Check for the case of obj.prop inside a getter
      if (_code->_isgetset && symflags.is_object() &&
	  t.second().is('.') &&
	  (t.second().first().text() == _code->_propertyName))
	use_overloads = false;
      if (_code->_issubsref && symflags.is_object()) use_overloads = false;
      // Check if we have A(...), where A isn't marked as global or persistent
      if (t.second().is(TOK_PARENS) && !symflags.is_global() && !symflags.is_persistent()  && !symflags.is_nested() && use_overloads)
	{
	  // First, get the arguments
	  reg_t argv = startList();
	  const Tree &args = t.second();
	  for (int p=0;p<args.numChildren();p++)
	    multiexpr(argv,args.child(p));
	  // Fetch the function
	  x = fetchVariableOrFunction(varname,argv);
	  emit(OP_SUBSREF,list,x,flattenDereferenceTreeToStackParensPrecomputed(t,argv,2));
	}
      else
	{
	  x = fetchVariable(varname,symflags);
	  if (use_overloads)
	    emit(OP_SUBSREF,list,x,flattenDereferenceTreeToStack(t,1));
	  else
	    emit(OP_SUBSREF_NOGS,list,x,flattenDereferenceTreeToStack(t,1));
	}
    }
  else
    {
      x = fetchVariable(varname,symflags);
      reg_t xv = getRegister();
      emit(OP_DEREF,xv,x);
      pushList(list,xv);
    }
}

void Compiler::expressionStatement(const Tree &t, bool printIt) {
  reg_t p = expression(t.first());
  if (printIt) emit(OP_PRINT,p);
  saveRegisterToName("ans",p);
}

reg_t Compiler::cellDefinition(const Tree &t) {
  if (t.numChildren() == 0)
    return fetchConstant(_ctxt->_cell->empty());
  reg_t y = startList();
  for (int i=0;i<t.numChildren();++i) {
    const Tree & s(t.child(i));
    reg_t x = startList();
    for (int j=0;j<s.numChildren();j++)
      multiexpr(x,s.child(j));
    emit(OP_CELLROWDEF,y,x);
  }
  reg_t z = getRegister();
  emit(OP_VCAT,z,y);
  return z;
}

reg_t Compiler::matrixDefinition(const Tree &t) {
  if (t.numChildren() == 0)
    return fetchConstant(_ctxt->_double->empty());
  reg_t y = startList();
  for (int i=0;i<t.numChildren();++i) {
    const Tree & s(t.child(i));
    reg_t x = startList();
    for (int j=0;j<s.numChildren();j++)
      multiexpr(x,s.child(j));
    reg_t z = getRegister();
    emit(OP_HCAT,z,x);
    emit(OP_PUSH,y,z);
  }
  reg_t z = getRegister();
  emit(OP_VCAT,z,y);
  return z;
}

void Compiler::multiexpr(reg_t list, const Tree &t) {
  if (t.is(TOK_VARIABLE))
    rhs(list,t);
  else if (!t.is(TOK_KEYWORD))
    pushList(list,expression(t));
}

reg_t Compiler::doGetMethod(const Tree &t) {
  reg_t args = startList();
  reg_t obj = fetchVariableOrFunction(t.first().text(),args);
  reg_t func = getRegister();
  emit(OP_GET_METHOD,func,obj,fetchConstantString(t.second().text()));
  reg_t funcHandle = makeHandle(func);
  if (t.numChildren() > 2) {
    reg_t list = startList();
    emit(OP_SUBSREF, list, funcHandle, flattenDereferenceTreeToStack(t,2));
    return listHead(list);
  } 
  return funcHandle;
}

reg_t Compiler::anonymousFunction(const Tree &t) {
  FMString fname = "anon" + Stringify(t.first().context());
  symbol_flags_t symflags = _code->_syms->syms[fname];
  // Compile the function
  CodeBlock *save_code = _code;
  CodeBlock *cp = new CodeBlock(_ctxt);
  save_code->_nested.push_back(cp);
  SymbolTable *_saveSym = _currentSym;
  cp->_syms = _currentSym->childNamed(fname);
  if (!cp->_syms)
    throw Exception("Internal compiler error - unable to find symbol table for anonymous function");
  _currentSym = cp->_syms;
  _module->_locals.insert(fname,cp);
  _code = cp;
  _code->_namelist = _ctxt->_list->empty();
  _code->_constlist = _ctxt->_list->empty();
  _code->_freelist = _ctxt->_list->empty();
  _code->_capturedlist = _ctxt->_list->empty();
  for (auto s = _code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
    {
      addStringToList(_ctxt,_code->_namelist,s.key());
      if (s.value().is_free())
	addStringToList(_ctxt,_code->_freelist,s.key());
      if (s.value().is_captured())
	addStringToList(_ctxt,_code->_capturedlist,s.key());
    }
  std::cout << "Compiling function " << fname << "\n";
  _code->_name = fname;
  std::cout << "Symbol table is " << _currentSym->name << "\n";
  const Tree &code = t.first().child(1);
  // Create a basic block, and push it on the list
  useBlock(new BasicBlock);
  // The body of an anonymous function is an expression.  We assign it to "_"
  {
    reg_t return_value = expression(code);
    saveRegisterToName("_",return_value);
  }
  emit(OP_RETURN);
  _code = save_code;
  reg_t func = fetchConstant(_ctxt->_string->makeString("#"+fname));
  reg_t desc = getRegister(); //TODO Descriptions for anonymous functions
  reg_t fptr = getRegister();
  emit(OP_MAKE_ANONYMOUS,fptr,desc,func);
  return fptr;
}

reg_t Compiler::listHead(reg_t list) {
  reg_t ret = getRegister();
  emit(OP_FIRST,ret,list);
  return ret;
}

reg_t Compiler::expression(const Tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE:
    {
      reg_t sp = startList();
      rhs(sp,t);
      return listHead(sp);
    }
  case TOK_REAL:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("D")) mt.chop(1);
      return fetchConstant(_ctxt->_double->makeScalar(mt.toDouble()));
    }
  case TOK_IMAG:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("D")) mt.chop(1);
      return fetchConstant(_ctxt->_double->makeComplex(0,mt.toDouble()));
    }
  case TOK_REALF:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("F")) mt.chop(1);
      return fetchConstant(_ctxt->_single->makeScalar(mt.toFloat()));
    }
  case TOK_IMAGF:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("F")) mt.chop(1);
      return fetchConstant(_ctxt->_single->makeComplex(0,mt.toFloat()));
    }
  case TOK_STRING:
    {
      return fetchConstant(_ctxt->_string->makeString(t.text()));
    }
  case TOK_END:
    {
      // Get the expression for the main variable
      reg_t variableReferenced = expression(t.first());
      reg_t endValue = getRegister();
      if (t.numChildren() == 1)
	emit(OP_LENGTH,endValue,variableReferenced);
      else
	{
	  reg_t posList = startList();
	  for (int i=0;i<t.second().numChildren();i++)
	    pushList(posList,expression(t.second().child(i)));
	  reg_t posLength = getRegister();
	  emit(OP_LENGTH,posLength,posList);
	  emit(OP_END,endValue,variableReferenced,posLength);
	}
      return endValue;
    }
  case ':':
    if (t.numChildren() == 0) {
      return fetchConstantString(":");
    } else if (t.first().is(':')) {
      return doubleColon(t);
    } else {
      return doBinaryOperator(t,OP_COLON);
    }
  case TOK_MATDEF:
    return matrixDefinition(t);
  case TOK_CELLDEF:
    return cellDefinition(t);
  case '+':
    return doBinaryOperator(t,OP_ADD);
  case '-':
    return doBinaryOperator(t,OP_MINUS);
  case '*':
    return doBinaryOperator(t,OP_MTIMES);
  case '/':
    return doBinaryOperator(t,OP_MRDIVIDE);
  case '\\':
    return doBinaryOperator(t,OP_MLDIVIDE);
  case TOK_SOR:
    return doShortcutOr(t);
  case '|':
    return doBinaryOperator(t,OP_OR);
  case TOK_SAND:
    return doShortcutAnd(t);
  case '&':
    return doBinaryOperator(t,OP_AND);
  case '<':
    return doBinaryOperator(t,OP_LT);
  case TOK_LE:
    return doBinaryOperator(t,OP_LE);
  case '>':
    return doBinaryOperator(t,OP_GT);
  case TOK_GE:
    return doBinaryOperator(t,OP_GE);
  case TOK_EQ:
    return doBinaryOperator(t,OP_EQ);
  case TOK_NE:
    return doBinaryOperator(t,OP_NE);
  case TOK_DOTTIMES:
    return doBinaryOperator(t,OP_TIMES);
  case TOK_DOTRDIV:
    return doBinaryOperator(t,OP_RDIVIDE);
  case TOK_DOTLDIV:
    return doBinaryOperator(t,OP_LDIVIDE);
  case TOK_UNARY_MINUS:
    return doUnaryOperator(t,OP_NEG);
  case TOK_UNARY_PLUS:
    return doUnaryOperator(t,OP_PLUS);
  case '~':
    return doUnaryOperator(t,OP_NOT);
  case '^':
    return doBinaryOperator(t,OP_POWER);
  case TOK_DOTPOWER:
    return doBinaryOperator(t,OP_DOTPOWER);
  case '\'':
    return doUnaryOperator(t,OP_HERMITIAN);
  case TOK_DOTTRANSPOSE:
    return doUnaryOperator(t,OP_TRANSPOSE);
  case '@':
    { // TODO - is this all?
      if (t.first().is(TOK_ANONYMOUS_FUNC))
	return anonymousFunction(t);
      FMString fname = t.first().text();
      symbol_flags_t symflags = _code->_syms->syms[fname];
      if (symflags.is_nested())
	return makeHandle(fetchClosure(fname));
      reg_t args = startList();
      return makeHandle(fetchVariableOrFunction(fname,args)); // FIXME - shouldn't make variables into handles.
    } 
  case TOK_GET_METHOD:
    return doGetMethod(t);
  case '?':
    {
      if (!(t.first().is(TOK_VARIABLE) && t.first().numChildren() == 1))
	throw Exception("The ? operator can only be used with identifiers, i.e. a = ?sclass");
      reg_t x = getRegister();
      emit(OP_LOAD_META,x,fetchConstantString(t.first().first().text()));
      return x;
    }
  default:
    throw Exception("Unrecognized expression!");
  }  
  throw Exception("Unrecognized expression!");
}



void Compiler::incrementRegister(reg_t x) {
  emit(OP_INCR,x);
}


void Compiler::specialFunctionCall(const Tree &t, bool printIt) {
  FMString funcname = t.first().text();
  reg_t args = startList();
  reg_t x = startList();
  for (int i=1;i<t.numChildren();i++)
    pushList(x,fetchConstantString(t.child(i).text()));
  pushList(args,fetchConstant(_ctxt->_double->makeScalar(0)));
  pushList(args,x);
  reg_t func = fetchVariableOrFunction(funcname,x);
  reg_t returns = getRegister();
  emit(OP_CALL,returns,func,args);
  reg_t toprint = getRegister();
  emit(OP_FIRST,toprint,returns);
  if (printIt) emit(OP_PRINTNE,toprint);
  saveRegisterToName("ans",toprint);
}

void Compiler::multiFunctionCall(const Tree & t, bool printIt) {
  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  t.print();
  TreeList s = t.first().children();
  reg_t lhsCount = getRegister();
  emit(OP_ZERO,lhsCount);
  std::vector<reg_t> lens;
  for (int ind=0;ind<s.size();ind++)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (t.numChildren() == 1 || t.last().is(TOK_PARENS))
	incrementRegister(lhsCount);
      else
	{
	  reg_t junk = startList();
	  rhs(junk,t);
	  reg_t y = listHead(junk);
	  reg_t len_junk = getRegister();
	  emit(OP_LENGTH,len_junk,y);
	  emit(OP_ADD,lhsCount,lhsCount,len_junk);
	  lens.push_back(len_junk);
	}
    }
  const Tree &f = t.second();
  FMString funcname = f.first().text();
  reg_t args = startList();
  emit(OP_PUSH,args,lhsCount);
  reg_t x = startList();
  const Tree &s2 = f.second();
  for (int p=0;p<s2.numChildren();p++)
    multiexpr(x,s2.child(p));
  pushList(args,x);
  reg_t func = fetchVariableOrFunction(funcname,x);  // FIXME - use args? or x?
  reg_t returns = getRegister();
  emit(OP_CALL,returns,func,args);
  // Now we allocate the resulting assignments
  int ret_length_ptr = 0;
  for (int ind=0;ind<s.size();++ind)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (t.numChildren() == 1)
	{
	  saveRegisterToName(varname,listHead(returns));
	}
      else
	{
	  emit(OP_SUBSASGN,flattenDereferenceTreeToStack(t,1),returns,getNameID(varname));
	  emit(OP_POP,returns,lens[ret_length_ptr++]);
	}
    }
}

Compiler::Compiler(ThreadContext *b) {
  assert(b);
  _ctxt = b;
  _regpool = new RegisterBlock(256,this);
  _code = new CodeBlock(b);
  _module = 0;
}

void Compiler::switchStatement(const Tree &t) {
  reg_t switch_val = expression(t.first());
  BasicBlock *end = new BasicBlock;
  BasicBlock *next;
  int n=1;
  while (n<t.numChildren() && t.child(n).is(TOK_CASE)) {
    reg_t case_val = expression(t.child(n).first());
    reg_t cmp = getRegister();
    emit(OP_CASE,cmp,switch_val,case_val);
    next = new BasicBlock;
    emit(OP_JUMP_ZERO,cmp,next);
    block(t.child(n).second());
    emit(OP_JUMP,end);
    useBlock(next);
    n++;
  }
  if (t.last().is(TOK_OTHERWISE))
    {
      block(t.last().first());
      emit(OP_JUMP,end);
    }
  useBlock(end);
}

// TODO - Remove duplication
void Compiler::forStatement(const Tree &t) {
  if (!t.first().is('='))  throw Exception("Incorrect format of for operator");
  FMString indexVarName = t.first().first().text();
  if (t.first().second().is(TOK_MATDEF) ||
      t.first().second().is(TOK_VARIABLE)) {
    // case "for j=[1:10]"
    // case "for j=K"
    // FM4 handled this incorrectly
    // These are handled in the general way
    reg_t indexSet = expression(t.first().second());
    const Tree & codeBlock(t.second());
    // Compute the number of columns
    reg_t iterCount = getRegister();
    emit(OP_NUMCOLS,iterCount,indexSet);
    // Allocate a register to track the column number
    reg_t colnum = fetchConstant(_ctxt->_double->makeScalar(1));
    // Start a new block
    BasicBlock *loop = new BasicBlock;
    emit(OP_JUMP,loop);
    useBlock(loop);
    BasicBlock *end = new BasicBlock;
    reg_t test_res = getRegister();
    emit(OP_LE,test_res,colnum,iterCount);
    emit(OP_JUMP_ZERO,test_res,end);
    // Retrieve the column
    reg_t col_value = getRegister();
    emit(OP_COLUMN,col_value,indexSet,colnum);
    saveRegisterToName(indexVarName,col_value);
    block(codeBlock);
    emit(OP_INCR,colnum);
    emit(OP_JUMP,loop);
    useBlock(end);
  } else if (t.first().second().first().is(':')) {
    reg_t first = expression(t.first().second().first().first());
    reg_t step = expression(t.first().second().first().second());
    reg_t args = startList();
    pushList(args,first);
    pushList(args,step);
    pushList(args,expression(t.first().second().second()));
    reg_t iter_count = getRegister();
    const Tree & codeBlock(t.second());
    emit(OP_LOOPCOUNT,iter_count,args);
    reg_t loop_index = fetchConstant(_ctxt->_double->makeScalar(0));
    BasicBlock *loop = new BasicBlock;
    emit(OP_JUMP,loop);
    useBlock(loop);
    BasicBlock *end = new BasicBlock;
    reg_t test_res = getRegister();
    emit(OP_LT,test_res,loop_index,iter_count);
    emit(OP_JUMP_ZERO,test_res,end);
    reg_t r1 = getRegister();
    emit(OP_TIMES,r1,loop_index,step);
    reg_t r2 = getRegister();
    emit(OP_ADD,r2,r1,first);
    saveRegisterToName(indexVarName,r2);
    block(codeBlock);
    emit(OP_INCR,loop_index);
    emit(OP_JUMP,loop);
    useBlock(end);
  } else {
    reg_t first = expression(t.first().second().first());
    reg_t step = fetchConstant(_ctxt->_double->makeScalar(1));
    reg_t args = startList();
    pushList(args,first);
    pushList(args,step);
    pushList(args,expression(t.first().second().second()));
    reg_t iter_count = getRegister();
    const Tree & codeBlock(t.second());
    emit(OP_LOOPCOUNT,iter_count,args);
    reg_t loop_index = fetchConstant(_ctxt->_double->makeScalar(0));
    BasicBlock *loop = new BasicBlock;
    emit(OP_JUMP,loop);
    useBlock(loop);
    BasicBlock *end = new BasicBlock;
    reg_t test_res = getRegister();
    emit(OP_LT,test_res,loop_index,iter_count);
    emit(OP_JUMP_ZERO,test_res,end);
    reg_t r1 = getRegister();
    emit(OP_TIMES,r1,loop_index,step);
    reg_t r2 = getRegister();
    emit(OP_ADD,r2,r1,first);
    saveRegisterToName(indexVarName,r2);
    block(codeBlock);
    emit(OP_INCR,loop_index);
    emit(OP_JUMP,loop);
    useBlock(end);
  }
}

void Compiler::whileStatement(const Tree &t) {
  BasicBlock *end = new BasicBlock;
  BasicBlock *test = new BasicBlock;
  emit(OP_JUMP, test);
  useBlock(test);
  reg_t condtest = expression(t.first()); 
  _continueblock.push(test);
  _breakblock.push(end);
  emit(OP_JUMP_ZERO,condtest, end);
  block(t.second());
  emit(OP_JUMP, test);
  useBlock(end);
  _continueblock.pop();
  _breakblock.pop();
}

void Compiler::ifStatement(const Tree &t) {
  reg_t condtest = expression(t.first());
  BasicBlock *next = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,condtest,next);
  block(t.second());
  emit(OP_JUMP,end);
  useBlock(next);
  int n=2;
  while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
    reg_t eltest = expression(t.child(n).first());
    next = new BasicBlock;
    emit(OP_JUMP_ZERO,eltest,next);
    block(t.child(n).second());
    emit(OP_JUMP,end);
    useBlock(next);
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    block(t.last().first());
  }
  emit(OP_JUMP,end);
  useBlock(end);
}

void Compiler::throwStatement(const Tree &t)
{
  reg_t r1 = expression(t.first());
  emit(OP_THROW,r1);
}

void Compiler::tryStatement(const Tree &t)
{
  BasicBlock *ctch = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_TRY_BEGIN,ctch);
  block(t.first());
  emit(OP_TRY_END);
  emit(OP_JUMP,end);
  useBlock(ctch);
  if (t.numChildren() > 1)
    block(t.second().first());
  emit(OP_JUMP,end);
  useBlock(end);
}

void Compiler::statementType(const Tree &t, bool printIt) {
  switch(t.token()) {
  case '=':
    assignment(t.first(),printIt,expression(t.second()));
    break;
  case TOK_MULTI:
    multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    forStatement(t);
    break;
  case TOK_WHILE:
    whileStatement(t);
    break;
  case TOK_IF:
    ifStatement(t);
    break;
  case TOK_BREAK:
    if (_breakblock.empty()) throw Exception("BREAK outside loop");
    emit(OP_JUMP,_breakblock.top());
    break;
  case TOK_CONTINUE:
    if (_continueblock.empty()) throw Exception("CONTINUE outside loop");
    emit(OP_JUMP,_continueblock.top());
    break;
  case TOK_RETURN:
    // An explicit return inside a script is unusual
    // We map it to a unique opcode to distinguish it from
    // a normal return.
    if (_module->_modtype == ScriptModuleType)
      emit(OP_RETSCRPT);
    else
      emit(OP_RETURN);
    break;
  case TOK_SWITCH:
    switchStatement(t);
    break;
  case TOK_TRY:
    tryStatement(t);
    break;
  case TOK_THROW:
    throwStatement(t);
    break;
  case TOK_QUIT:
    //quitStatement();
    break;
  case TOK_RETALL:
    //retallStatement();
    break;
  case TOK_KEYBOARD:
    emit(OP_STOP);
    break;
  case TOK_GLOBAL:
    break;
  case TOK_PERSISTENT:
    break;
  case TOK_EXPR:
    expressionStatement(t,printIt);
    break;
  default:
    throw Exception("Unrecognized statement type");
  }  
}

// 

void Compiler::reset() {
  _code = new CodeBlock(_ctxt);
  delete _regpool;
  _regpool = new RegisterBlock(256,this);
  delete _module;
  _module = new Module(_ctxt);
  _module->_modtype = FunctionModuleType;
  // while (!_codestack.empty()) {
  //   CodeBlock *p = _codestack.top();
  //   delete p;
  //   _codestack.pop();
  // }
  while (!_continueblock.empty()) {
    BasicBlock *b = _continueblock.top();
    delete b;
    _continueblock.pop();
  }
  while (!_breakblock.empty()) {
    BasicBlock *b = _breakblock.top();
    delete b;
    _breakblock.pop();
  }
}

void PrintModule(Module* mod);

void Compiler::compile(const FMString &code, const FMString &name) {
  _currentLineNo = 0;
  Scanner S(code,"");
  Parser P(S);
  Tree t(P.process());
  EndRemoverPass e;
  t.print();
  t = e.walkCode(t);
  NestedFunctionMoverPass n;
  t = n.walkCode(t);
  std::cout << "********************************************************************************\n";
  t.print();
  std::cout << "********************************************************************************\n";
  SymbolPass p;
  reset();
  _module->_name = name;
  // Start the code walk at a depth of 1 - if it's a function
  // definition instead of a script, the internal blocks will
  // be processed at the correct depth.
  p.walkCode(t);
  p.dump();
  _symsRoot = p.getRoot();
  _currentSym = _symsRoot;
  walkCode(t);
  PrintModule(_module);
}

void Compiler::insertPrefix(FMString funcName, FunctionTypeEnum funcType) {
  switch (funcType) {
  case ConstructorFunction:
    {
      // Iterate over symbols - find all symbols marked as super classes
      // without an explicit invokation - also find the name of the object
      FMString objname;
      FMStringList toInvoke;
      for (auto s = _currentSym->parent->syms.constBegin(); s != _currentSym->parent->syms.constEnd(); ++s)
	if (s.value().is_super() && (!s.value().is_explicit())) toInvoke.push_back(s.key());
      for (auto s = _currentSym->syms.constBegin(); s != _currentSym->syms.constEnd(); ++s)
	  if (s.value().is_object()) objname = s.key();
      reg_t args = startList();
      for (int i=0;i<toInvoke.size();i++)
	{
	  std::cout << "Adding prefix call for base class " << toInvoke[i] << "\n";
	  reg_t obj = getRegister();
	  emit(OP_LOAD,obj,getNameID(objname));
	  reg_t func = getRegister();
	  emit(OP_GET_METHOD,func,obj,fetchConstantString(toInvoke[i]));
	  emit(OP_DEREF,obj,func);
	  emit(OP_SAVE,obj,getNameID(objname));
	}
      break;
    }
  case SubsrefFunction:
    {
      _code->_issubsref = true;
      break;
    }
  case SubsasgnFunction:
    {
      _code->_issubsasgn = true;
      break;
    }
  case GetterFunction:
    {
      if (_currentSym->countParameterValues() != 1)
	throw Exception("Getter for class " + _currentSym->name + " named " + funcName + " cannot have more than one parameter (object)");
      if (_currentSym->countReturnValues() != 1)
	throw Exception("Getter for class " + _currentSym->name + " named " + funcName + " must have one return value");
      _code->_isgetset = true;
      std::cout << "Set Get flag set to true\n";
      _code->_propertyName = funcName.mid(4);
      std::cout << "Property name = " << _code->_propertyName << "\n";
      break;
    }
  case SetterFunction:
    {
      if (_currentSym->countParameterValues() < 2)
	throw Exception("Setter for class " + _currentSym->name + " named " + funcName + " must take at least two parameters (object, value)");
      if (_currentSym->countReturnValues() != 1)
	throw Exception("Setter for class " + _currentSym->name + " named " + funcName + " must have one return value");
      _code->_isgetset = true;
      std::cout << "Set Get flag set to true\n";
      _code->_propertyName = funcName.mid(4);
      std::cout << "Property name = " << _code->_propertyName << "\n";
      break;
    }
  default:
    {
      // No prefix for default function types
    }
  }  
}

void Compiler::walkFunction(const Tree &t, FunctionTypeEnum funcType) {
  CodeBlock *save_code = _code;
  CodeBlock *cp = new CodeBlock(_ctxt);
  FMString funcName = t.child(1).text();
  cp->_syms = _currentSym;
  switch (funcType) {
  case NormalFunction:
    _module->_main = cp;
    break;
  case NestedFunction:
    save_code->_nested.push_back(cp);
    break;
  case LocalFunction:
  case MethodFunction:
  case SubsrefFunction:
  case SubsasgnFunction:
  case GetterFunction:
  case SetterFunction:
  case ConstructorFunction:
    _module->_locals.insert(funcName,cp); // Methods are stored as local functions
    break;
  default:
    throw Exception("Unhandled function type");
  }
  _code = cp;
  // Build up the variable list
  // By convention, the arguments are first
  _code->_namelist = _ctxt->_list->empty();
  _code->_constlist = _ctxt->_list->empty();
  _code->_freelist = _ctxt->_list->empty();
  _code->_capturedlist = _ctxt->_list->empty();

  for (auto s = _code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
    {
      addStringToList(_ctxt,_code->_namelist,s.key());
      if (s.value().is_free())
	addStringToList(_ctxt,_code->_freelist,s.key());
      if (s.value().is_captured())
	addStringToList(_ctxt,_code->_capturedlist,s.key());
    }
  std::cout << "Compiling function " << funcName << "\n";
  _code->_name = funcName;
  std::cout << "Symbol table is " << _currentSym->name << "\n";
  //const Tree &rets = t.child(0);
  //  beginFunction(funcName,nested);
  //const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  // Create a basic block, and push it on the list
  useBlock(new BasicBlock);
  // TODO - clean this up
  insertPrefix(funcName, funcType);
  block(code);
  emit(OP_RETURN);
  _code = save_code;
}


void Compiler::walkFunctionCollection(const Tree &t) {

  for (int i=0;i<t.numChildren();++i)
    switch (t.child(i).token())
      {
      case TOK_FUNCTION:
	{
	  _currentSym = _symsRoot->children[i];
	  if (i == 0)
	    walkFunction(t.child(i),NormalFunction);
	  else
	    walkFunction(t.child(i),LocalFunction);
	  break;
	}
      default:
	throw Exception("Unexpected child of function collection");
      }
}

void Compiler::walkScript(const Tree &t) {
  CodeBlock *cp = new CodeBlock(_ctxt);
  cp->_syms = _currentSym;
  _module->_main = cp;
  _code = cp;
  // Build up the variable list
  _code->_namelist = _ctxt->_list->empty();
  _code->_constlist = _ctxt->_list->empty();
  for (auto s = _code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
    addStringToList(_ctxt,_code->_namelist,s.key());
  useBlock(new BasicBlock);
  block(t.first());
  emit(OP_RETURN);
}

void Compiler::walkMethods(const Tree &t) {
  assert(t.is(TOK_METHODS));
  int start = 0;
  SymbolTable *saveSym = _currentSym;
  if (t.numChildren() > 1 && t.first().is(TOK_ATTRIBUTES)) start++;
  for (int i=start;i<t.numChildren();i++)
    {
      const Tree &p = t.child(i);
      FMString methodName = p.child(1).text();
      std::cout << "Compiling method named: " << methodName << "\n";
      symbol_flags_t flags = _symsRoot->syms[methodName];
      std::cout << "Symbol flags for this method are: " << flags.str() << "\n";
      // Find symbol table for this method
      _currentSym = _symsRoot->childNamed(p.child(1).text());
      if (!_currentSym) throw Exception("Internal compiler error: unable to find symbol table for method " + p.child(1).text());
      if (flags.is_constructor())
	walkFunction(p,ConstructorFunction);
      else if (flags.is_getter())
	walkFunction(p,GetterFunction);
      else if (flags.is_setter())
	walkFunction(p,SetterFunction);
      else if (flags.is_subsref())
	walkFunction(p,SubsrefFunction);
      else if (flags.is_subsasgn())
	walkFunction(p,SubsasgnFunction);
      else
	walkFunction(p,MethodFunction);
    }
  _currentSym = saveSym;
}

void Compiler::walkProperties(reg_t list, const Tree &t) {
  assert(t.is(TOK_PROPERTIES));
  int start = 0;
  if (t.numChildren() > 1 && t.first().is(TOK_ATTRIBUTES)) start++;
  for (int i=start;i<t.numChildren();i++)
    {
      const Tree &p = t.child(i);
      FMString propname = p.text();
      // Create a list of arguments to OP_PROPERTY
      // Name, isConstant, isDependent, defaultValue, getter, setter
      reg_t prop_args = startList();
      symbol_flags_t flags = _symsRoot->syms[propname];
      pushList(prop_args,fetchConstantString(propname));
      pushList(prop_args,fetchConstantBool(flags.is_constant()));
      pushList(prop_args,fetchConstantBool(flags.is_dependent()));
      if (p.hasChildren())
	pushList(prop_args,expression(p.first().first()));
      else
	pushList(prop_args,fetchEmpty());
      FMString gettername = "get." + propname;
      if (_symsRoot->syms.contains(gettername))
	pushList(prop_args,fetchConstantString("#"+gettername));
      else
	pushList(prop_args,fetchEmpty());
      FMString settername = "set." + propname;
      if (_symsRoot->syms.contains(settername))
	pushList(prop_args,fetchConstantString("#"+settername));
      else
	pushList(prop_args,fetchEmpty());
      pushList(list,prop_args);
    }
}

void Compiler::walkClassDef(const Tree &t) {
  CodeBlock *cp = new CodeBlock(_ctxt);
  cp->_syms = _currentSym;
  _module->_main = cp;
  _module->_modtype = ClassdefModuleType;
  _module->_dependencies = _ctxt->_list->empty();
  _code = cp;
  _code->_namelist = _ctxt->_list->empty();
  _code->_constlist = _ctxt->_list->empty();
  for (auto s = _currentSym->syms.constBegin(); s != _currentSym->syms.constEnd(); ++s)
    addStringToList(_ctxt,_code->_namelist,s.key());
  // Build up the property list
  useBlock(new BasicBlock);
  {
    reg_t cd_args = startList();
    // Walk the superclasses (if they exist)
    reg_t superclasses = startList();
    if (t.child(1).is('<'))
      {
	const Tree &s = t.child(1);
	for (int j=0;j<s.numChildren();j++)
	  {
	    pushList(superclasses,fetchConstantString(s.child(j).text()));
	    _ctxt->_list->push(_module->_dependencies,_ctxt->_string->makeString(s.child(j).text()));
	  }
      }
    reg_t props = startList();
    // Walk the properties
    for (int i=1;i<t.numChildren();i++)
      if (t.child(i).is(TOK_PROPERTIES))
	walkProperties(props,t.child(i));
    reg_t events = startList();
    // Walk the events
    for (auto s = _currentSym->syms.constBegin(); s != _currentSym->syms.constEnd(); ++s)
      if (s.value().is_event())
	pushList(events,fetchConstantString(s.key()));
    reg_t methods = startList();
    // Note - setters and getters are ignored here because they are 
    // handled in the property construction (walkProperty).
    // Constructors are special cased.
    for (auto s = _currentSym->syms.constBegin(); s != _currentSym->syms.constEnd(); ++s)
      if (s.value().is_method() && !s.value().is_getter() && !s.value().is_setter() && !s.value().is_constructor())
	{
	  reg_t method_args = startList();
	  pushList(method_args,fetchConstantString(s.key()));
	  pushList(method_args,fetchConstantString("#"+s.key()));
	  pushList(method_args,fetchConstantBool(s.value().is_static()));
	  pushList(methods,method_args);
	}
    // Walk the methods
    for (int i=1;i<t.numChildren();i++)
      if (t.child(i).is(TOK_METHODS))
	walkMethods(t.child(i));
    // Find the constructor
    bool explicit_constructor = false;
    reg_t constructor;
    for (auto s = _currentSym->syms.constBegin(); s != _currentSym->syms.constEnd(); ++s)
      if (s.value().is_constructor())
	{
	  explicit_constructor = true;
	  constructor = fetchConstantString("#" + s.key());
	  break;
	}
    if (!explicit_constructor)
      {
	std::cout << "Explicit constructor not found!\n";
	constructor = fetchConstant(_ctxt->_module->pass());
      }
    reg_t name = fetchConstantString(cp->_syms->name);
    pushList(cd_args,superclasses);
    pushList(cd_args,props);
    pushList(cd_args,events);
    pushList(cd_args,methods);
    pushList(cd_args,constructor);
    emit(OP_CLASSDEF,name,cd_args);
  }
  emit(OP_RETURN);
}

void Compiler::walkCode(const Tree &t) {
  switch (t.token())
    {
    case TOK_FUNCTION_DEFS:
      {
	walkFunctionCollection(t);
	break;
      }
    case TOK_SCRIPT:
      {
	_module->_modtype = ScriptModuleType;
	walkScript(t);
	break;
      }
    case TOK_CLASSDEF:
      {
	walkClassDef(t);
	break;
      }
    default:
      throw Exception("Unexpected code type for compilation");
    }
}

void Compiler::statement(const Tree &t) {
  _currentLineNo = t.node().position() & 0xFFFF;
  if (t.first().is(TOK_FOR) || t.first().is(TOK_WHILE) || t.first().is(TOK_IF))
    _currentLineNo = t.first().node().position() & 0xFFFF;
  if (t.is(TOK_QSTATEMENT))
    statementType(t.first(),false);
  else if (t.is(TOK_STATEMENT))
    statementType(t.first(),true);
  else if (t.is(TOK_NEST_FUNC))
    {
      SymbolTable *_saveSym = _currentSym;
      _currentSym = _currentSym->children[0];
      walkFunction(t,NestedFunction);
      _currentSym = _saveSym;
    }
}

void Compiler::block(const Tree &t) {
  const TreeList statements(t.children());
  for (auto i : statements) statement(i);
}

Module* Compiler::module() {
  return _module;
  /*
  // The first code block on the stack is the "main" function
  // The rest of the code blocks are local functions.  Reverse
  // the stack and move all the local functions into the local
  // function pool.
  std::stack<CodeBlock *> codelist;
  while (!_codestack.empty())
    {
      codelist.push(_codestack.top());
      _codestack.pop();
    }
  if (codelist.size() == 0) return NULL;
  CodeBlock *main = codelist.top();
  codelist.pop();
  Module *mod = new Module;
  mod->_name = main->_name; // FIXME - not true - module name comes from the filename
  mod->_main = main;
  while (!codelist.empty())
    {
      CodeBlock *local = codelist.top();
      codelist.pop();
      mod->_locals[local->_name] = local;
    }
  return mod;
  */
}


// To add peer functions to the current symboltable,
// newSibling() {
//  SymbolTable *g = new SymbolTable;
//   g->_parent = _current->_parent;
//   _current->_parent->_children.push_back(g);
//   _current = _current->_parent->_children.back();
// }
// newChild() {
//  SymbolTable *g;
//   g->_parent = _current;
//   _current->_children.push_back(g);
//   _current = _current->children.back();
// }
// popToParent() {
//   _current = _current->_parent;
// }



// The output of the symbol pass will then be:
// 



void PrintInsn(int ip, const Instruction &i)
{
  printf("%03d (%03d)  ",ip,i._position);
  int8_t op = opcode(i._opcode);
  printf("%-20s",getOpCodeName(op).c_str());
  printf("%-20s",Compiler::opcodeDecode(op,i._opcode).c_str());
  if (i._target)	
    printf("%-15p",i._target);
  printf("\n");
}

void PrintBasicBlock(BasicBlock *b)
{
  printf("  Block %p\n",b);
  for (int i=0;i<b->_insnlist.size();i++)
    PrintInsn(i,b->_insnlist[i]);
}

void PrintCodeBlock(CodeBlock *_code)
{
  std::cout << "************************************************************\n";
  std::cout << "Symbols for function: " << _code->_syms->name << "\n";
  for (auto s=_code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
    std::cout << "   Symbol: " << s.key() << " flags: " << s.value().str() << "\n";
  std::cout << "************************************************************\n";      
  std::cout << "Const: ";
  ListType *lt = _code->_constlist.asType<ListType>();
  for (int i=0;i<_code->_constlist.count();i++)
    std::cout << lt->fetch(_code->_constlist,i).description() << " ";
  std::cout << "\n";
  std::cout << "Names:  " << _code->_namelist << "\n";
  std::cout << "Free:  " << _code->_freelist << "\n";
  std::cout << "Captured:  " << _code->_capturedlist << "\n";
  std::cout << "Code:\n";
  for (int j=0;j<_code->_blocklist.size();j++)
    PrintBasicBlock(_code->_blocklist[j]);
}

void PrintModule(Module* mod)
{
  std::cout << "************************************************************\n";
  std::cout << "* Module: " << mod->_name << "\n";
  std::cout << "************************************************************\n";
  std::cout << " MAIN\n";
  std::cout << "************************************************************\n";
  PrintCodeBlock(mod->_main);
  std::cout << "************************************************************\n";
  std::cout << " LOCALS\n";
  std::cout << "************************************************************\n";
  for (FMMap<FMString,CodeBlock*>::const_iterator i=mod->_locals.constBegin();
       i!=mod->_locals.constEnd();++i)
      PrintCodeBlock(i.value());
}


void FM::DumpBasicBlock(BasicBlock *b, int offset)
{
  printf("Basic Block : %p\n",b);
  printf("  offset    : %d\n",b->_offset);
  printf("  seen      : %d\n",b->_seen);
  printf("  insn count: %lu\n",b->_insnlist.size());
  for (int i=0;i<b->_insnlist.size();++i)
    PrintInsn(i+offset,b->_insnlist[i]);
}

static void PrintRawInsn(int ip, insn_t insn)
{
  printf("%03d   %016llx %03x %03x %03x %06x ",ip,insn,reg1(insn),reg2(insn),reg3(insn),get_constant(insn));
  int8_t opcode = insn & 0xFF;
  printf("%-15s",getOpCodeName(opcode).c_str());
  printf("%-25s",Compiler::opcodeDecode(opcode,insn).c_str());
  printf("\n");
}

static void PrintInsn(uint16_t lineno, int ip, insn_t insn)
{
  printf("%03d (%03d) ",ip,lineno);
  int8_t opcode = insn & 0xFF;
  printf("%-15s",getOpCodeName(opcode).c_str());
  printf("%-25s",Compiler::opcodeDecode(opcode,insn).c_str());
  printf("\n");
}

void FM::Disassemble(ThreadContext *_ctxt, const Object &p)
{
  //  if (!p.isUserClass() || p.className() != "code_object")
  //    throw Exception("argument to disassemble is not a code_object");
  if (p.isEmpty()) {
    std::cout << "Empty object of type " << p.type()->name() << "\n";
    return;
  }
  if (p.type()->code() == TypeModule)
    {
      const ModuleData *md = _ctxt->_module->ro(p);
      std::cout << "Module: " << md->m_name << "\n";
      std::cout << "Type: " << ModuleTypeCodeToString(md->m_modtype) << "\n";
      std::cout << "Dependencies: " << md->m_dependencies.description() << "\n";
      std::cout << "  ** main routine **\n";
      Disassemble(_ctxt,_ctxt->_function->ro(md->m_main)->m_code);
      for (auto i : md->m_locals) {
	std::cout << "  ** local routine " << i.first << " **\n";
	Disassemble(_ctxt,_ctxt->_function->ro(i.second)->m_code);
      }
      return;
    }
  if (p.type()->code() == TypeFunction)
    {
      const FunctionData *fd = _ctxt->_function->ro(p);
      std::cout << "Function: " << fd->m_name << "\n";
      std::cout << "Closure: " << fd->m_closure << "\n";
      Disassemble(_ctxt,fd->m_code);
      return;
    }
  assert(p.type()->code() == TypeCode);
  const CodeData *dp = _ctxt->_code->ro(p);
  std::cout << "Name      : " << dp->m_name.description() << "\n";
  std::cout << "Module    : " << dp->m_module.description() << "\n";
  std::cout << "Names     : " << dp->m_names << "\n";
  std::cout << "Captured  : " << dp->m_captured << "\n";
  std::cout << "Free      : " << dp->m_free << "\n";
  std::cout << "Parameters: [";
  for (int i=0;i<dp->m_params.count();i++)
    std::cout << _ctxt->_index->ro(dp->m_params)[i] << " ";
  std::cout << "]\n";
  std::cout << "Returns   : [";
  for (int i=0;i<dp->m_returns.count();i++)
    std::cout << _ctxt->_index->ro(dp->m_returns)[i] << " ";
  std::cout << "]\n";
  std::cout << "Varargin  : " << dp->m_varargin << "\n";
  std::cout << "Varargout : " << dp->m_varargout << "\n";
  const Object* cp = _ctxt->_list->ro(dp->m_consts);
  std::cout << "Consts    : ";
  for (dim_t i=0;i<dp->m_consts.count();i++)
    std::cout << cp[i].brief() << ", ";
  std::cout << "\n";
  std::cout << "Line No   : " << dp->m_lineno << "\n";
  std::vector<uint16_t> line_nos;
  rle_decode_line_nos(_ctxt->_uint32->ro(dp->m_lineno),
		      dp->m_lineno.count(),line_nos);
  Object code = dp->m_code;
  const insn_t *opcodes = _ctxt->_uint64->ro(code);
  std::cout << "Code: " << code.dims().count() << " length\n";
  //  assert(code.count() == line_nos.size());
  for (dim_t i=0;i<code.dims().count();++i)
    PrintInsn(line_nos[i],i,opcodes[i]);
  for (dim_t i=0;i<dp->m_consts.count();i++)
    if (cp[i].is(TypeCode) || cp[i].is(TypeFunction))
      {
	std::cout << "     ***** Nested Routine ****\n";
	Disassemble(_ctxt,cp[i]);
      }
}
