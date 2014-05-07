#include "Compiler.hpp"
#include <iostream>
#include "BaseTypes.hpp"

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

#define opcode(x) ((x) & 0xFF)
#define reg1(x) (((x) >> 8) & 0xFF)
#define reg2(x) (((x) >> 16) & 0xFF)
#define reg3(x) (((x) >> 24) & 0xFF)
#define constant(x) (((x) >> 16) & 0xFFFF)

std::string Compiler::opcodeDecodeArgs(op_t opcode, int32_t val)
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
    case register_offset:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " ;
    case offset:
      return Stringify(constant(val)) + "  ; " ;
    case register_constant:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " ;
    case register_int:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " ;
	//	+ SummarizeArrayCellEntry(_code->_constlist[constant(val)]);
    case register_variable:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " 
	+ _code->_varlist[constant(val)];
    case register_free:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " 
	+ _code->_freelist[constant(val)];
    case register_captured:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " 
	+ _code->_capturedlist[constant(val)];
    case register_name:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val)) + "  ; " 
	+ _code->_namelist[constant(val)];
    case constant:
      return Stringify(constant(val)) + "  ; " + _code->_constlist[constant(val)].description();
    default:
      return "unknown";
    }
}

std::string Compiler::opcodeDecode(op_t opcode, int32_t val)
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
      return "r" + Stringify(reg1(val)) + ", Const[" + Stringify(constant(val)) +"]";
    case register_int:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val));
    case register_variable:
      return "r" + Stringify(reg1(val)) + ", Var[" + Stringify(constant(val)) + "]";
    case register_captured:
      return "r" + Stringify(reg1(val)) + ", Cap[" + Stringify(constant(val)) + "]";
    case register_free:
      return "r" + Stringify(reg1(val)) + ", Free[" + Stringify(constant(val)) + "]";
    case register_name:
      return "r" + Stringify(reg1(val)) + ", Name[" + Stringify(constant(val)) + "]";
    case register_offset:
      return "r" + Stringify(reg1(val)) + ", " + Stringify(constant(val));
    case offset:
    case constant:
      return Stringify(constant(val));
    default:
      return "unknown";
    }
}


void Compiler::useBlock(BasicBlock *p)
{
  _code->_blocklist.push_back(p);
  _code->_currentBlock = p;
}

void Compiler::emit(int8_t opcode, reg_t reg1, BasicBlock *blk)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8);
  i._target = blk;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, BasicBlock *blk)
{
  Instruction i;
  i._opcode = opcode;
  i._target = blk;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2, reg_t reg3)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (reg2->num() << 16) | (reg3->num() << 24);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2, idx_t reg3)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (reg2->num() << 16) | (reg3 << 24);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (reg2->num() << 16);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1, idx_t reg2)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (reg2 << 16);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, reg_t reg1)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}


void Compiler::emit(int8_t opcode, reg_t reg1, const_t arg)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (arg << 16);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode, const_t arg)
{
  Instruction i;
  i._opcode = opcode | (arg << 16);
  i._target = 0;
  i._position = 0;
  _code->_currentBlock->_insnlist.push_back(i);
}

void Compiler::emit(int8_t opcode)
{
  Instruction i;
  i._opcode = opcode;
  i._target = 0;
  i._position = 0;
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
  emit(OP_CALL,rp,sp,getNameID(funcname));
  return rp;
}

reg_t Compiler::getRegister()
{
  return _regpool->getRegister();
}

// Invoke a function, but we expect a scalar argument
// -- expressionStatement, rhs
reg_t Compiler::doScalarFunctionExpression(const Tree &t, bool optional_output) {
  /*
  // FIXME - broken
  reg_t nargout = fetchConstant(_types->_int32->makeScalar(optional_output ? 0 : 1));
  reg_t sp = getStackRegister();
  doFunctionExpression(t,nargout);
  reg_t x = readFromStack(sp);
  return x;
  */
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


//TODO - Collapse into getID(varname,flags)...

idx_t Compiler::getVariableID(const FMString & t) {
  return _code->_varlist.indexOf(t);
}

idx_t Compiler::getNameID(const FMString & t) {
  return _code->_namelist.indexOf(t);
}

idx_t Compiler::getCapturedID(const FMString & t) {
  return _code->_capturedlist.indexOf(t);
}

idx_t Compiler::getFreeID(const FMString &t) {
  return _code->_freelist.indexOf(t);
}

const_t Compiler::getConstantID(const FMString & t) {
  return getConstantID(_types->_string->makeString(t));
}

const_t Compiler::getConstantID(const Object & t) {
  for (int i=0;i<_code->_constlist.size();++i)
    if (_code->_constlist[i] == t) return i;
  _code->_constlist.push_back(t);
  return _code->_constlist.size()-1;
}

void Compiler::saveRegisterToName(const FMString &varname, reg_t b) {
  int symflags = _code->_syms->syms[varname];
  if (IS_LOCAL(symflags))
    emit(OP_SAVE,b,getVariableID(varname));
  else if (IS_GLOBAL(symflags))
    emit(OP_SAVE_GLOBAL,b,getNameID(varname));
  else if (IS_PERSIST(symflags))
    emit(OP_SAVE_PERSIST,b,getNameID(varname));
  else if (IS_CAPTURED(symflags))
    emit(OP_SAVE_CAPTURED,b,getCapturedID(varname));
  else if (IS_FREE(symflags))
    emit(OP_SAVE_FREE,b,getFreeID(varname));
  else if (IS_DYNAMIC(symflags))
    emit(OP_SAVE_DYNAMIC,b,getNameID(varname));
  else
    throw Exception("Unhandled save case");
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
      return;
    }
  reg_t args = flattenDereferenceTreeToStack(t,1);
  int symflags = _code->_syms->syms[varname];
  if (IS_LOCAL(symflags))
    emit(OP_SUBSASGN,args,b,getVariableID(varname));
  else if (IS_GLOBAL(symflags))
    emit(OP_SUBSASGN_GLOBAL,args,b,getNameID(varname));
  else if (IS_PERSIST(symflags))
    emit(OP_SUBSASGN_PERSIST,args,b,getNameID(varname));
  else if (IS_CAPTURED(symflags))
    emit(OP_SUBSASGN_CAPTURED,args,b,getCapturedID(varname));
  else if (IS_FREE(symflags))
    emit(OP_SUBSASGN_FREE,args,b,getFreeID(varname));
  else if (IS_DYNAMIC(symflags))
    emit(OP_SUBSASGN_DYNAMIC,args,b,getNameID(varname));
  else
    throw Exception("Unhandled subsasgn case");
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
      }
    default:
      throw Exception("Unknown expression!");
    }
}

reg_t Compiler::fetchConstant(const Object &constant)
{
  reg_t r0 = getRegister();
  emit(OP_LOAD_CONST,r0,getConstantID(constant));
  return r0;
}

reg_t Compiler::fetchConstantString(const FMString &constant)
{
  return fetchConstant(_types->_string->makeString(constant));
}

reg_t Compiler::fetchVariable(const FMString &varname, int flags)
{
  reg_t x = getRegister();
  if (IS_LOCAL(flags))
    emit(OP_LOAD,x,getVariableID(varname));
  else if (IS_GLOBAL(flags))
    emit(OP_LOAD_GLOBAL,x,getNameID(varname));
  else if (IS_PERSIST(flags))
    emit(OP_LOAD_PERSIST,x,getNameID(varname));
  else if (IS_CAPTURED(flags))
    emit(OP_LOAD_CAPTURED,x,getCapturedID(varname));
  else if (IS_FREE(flags))
    emit(OP_LOAD_FREE,x,getCapturedID(varname));
  else
    emit(OP_LOAD_DYNAMIC,x,getNameID(varname));
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
  BasicBlock *next = new BasicBlock;
  BasicBlock *fail1 = new BasicBlock;
  BasicBlock *fail2 = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,expression(t.first()),fail1);
  emit(OP_LOAD_CONST,ret,getConstantID(_types->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail1);
  emit(OP_JUMP_ZERO,expression(t.second()),fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(_types->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(_types->_bool->makeScalar(false)));
  emit(OP_JUMP,end);
  useBlock(end);
  return ret;
}


reg_t Compiler::doShortcutAnd(const Tree &t) {
  reg_t ret = getRegister();
  BasicBlock *next = new BasicBlock;
  BasicBlock *fail = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,expression(t.first()),fail);
  emit(OP_JUMP_ZERO,expression(t.second()),fail);
  emit(OP_LOAD_CONST,ret,getConstantID(_types->_bool->makeScalar(true)));
  emit(OP_JUMP,end);
  useBlock(fail);
  emit(OP_LOAD_CONST,ret,getConstantID(_types->_bool->makeScalar(false)));
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
  int min_children_to_reindex = 1;
  reg_t x;
  int symflags = _code->_syms->syms[varname];
  x = fetchVariable(varname,symflags);
  min_children_to_reindex = 1;
  if (t.numChildren() > min_children_to_reindex)
    emit(OP_SUBSREF,list,x,flattenDereferenceTreeToStack(t,min_children_to_reindex));
  else
    pushList(list,x);
}

// TODO - what about "ans"?
void Compiler::expressionStatement(const Tree &t, bool printIt) {
  reg_t p = expression(t.first());
  if (printIt) emit(OP_PRINT,p);
}

reg_t Compiler::cellDefinition(const Tree &t) {
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
  reg_t y = startList();
  for (int i=0;i<t.numChildren();++i) {
    const Tree & s(t.child(i));
    reg_t x = startList();
    for (int j=0;j<s.numChildren();j++)
      multiexpr(x,s.child(j));
    emit(OP_HCAT,y,x);
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

reg_t Compiler::expression(const Tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE:
    {
      const FMString &varname = t.first().text();
      int flags = _code->_syms->syms[varname];
      reg_t q = fetchVariable(varname,flags);
      if (t.numChildren() == 1)
	{
	  if (IS_DYNAMIC(flags))
	    {
	      reg_t p = getRegister();
	      emit(OP_DEREF,p,q);
	      return p;
	    }
	  return q;
	}
      reg_t sp = startList();
      rhs(sp,t);
      reg_t ret = getRegister();
      emit(OP_POP,ret,sp);
      return ret;
    }
  case TOK_REAL:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("D")) mt.chop(1);
      return fetchConstant(_types->_double->makeScalar(mt.toDouble()));
    }
  case TOK_IMAG:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("D")) mt.chop(1);
      return fetchConstant(_types->_double->makeComplex(0,mt.toDouble()));
    }
  case TOK_REALF:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("F")) mt.chop(1);
      return fetchConstant(_types->_single->makeScalar(mt.toFloat()));
    }
  case TOK_IMAGF:
    {
      FMString mt(t.text());
      if (mt.toUpper().endsWith("F")) mt.chop(1);
      return fetchConstant(_types->_single->makeComplex(0,mt.toFloat()));
    }
  case TOK_STRING:
    {
      return fetchConstant(_types->_string->makeString(t.text()));
    }
  case TOK_END:
    // Rewrite the tree to remove TOK_END...
    // if (!endRef.valid())
    //   throw Exception("END keyword not allowed for undefined variables");
    // if (endTotal == 1)
    //   return Array(double(endRef->length()));
    // else
    //   return Array(double(endRef->dimensions()[endCount]));
    break;
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
    //    return FunctionPointer(t);
  default:
    throw Exception("Unrecognized expression!");
  }  
}

void Compiler::incrementRegister(reg_t x) {
  emit(OP_INCR,x);
}

void Compiler::multiFunctionCall(const Tree & t, bool printIt) {
  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  TreeList s = t.first().children();
  reg_t lhsCount = getRegister();
  emit(OP_ZERO,lhsCount);
  for (int ind=0;ind<s.size();ind++)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (t.numChildren() == 1 || t.last().is(TOK_PARENS))
	incrementRegister(lhsCount);
      else
	emit(OP_LHSCOUNT,lhsCount,flattenDereferenceTreeToStack(t,1),getVariableID(varname));
    }
  // Make the function call here
  reg_t ans = doFunctionExpression(t.second(),lhsCount);
  // Now we allocate the resulting assignments
  for (int ind=0;ind<s.size();++ind)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (t.numChildren() == 1)
	{
	  reg_t b = getRegister();
	  emit(OP_POP,b,ans);
	  saveRegisterToName(varname,b);
	}
      else
	emit(OP_SUBSASGNM,ans,flattenDereferenceTreeToStack(t,1),getVariableID(varname));
    }
}

Compiler::Compiler(BaseTypes *b) {
  assert(b);
  _types = b;
  _regpool = new RegisterBlock(256);
  _code = new CodeBlock;
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
    reg_t colnum = fetchConstant(_types->_index->makeScalar(1));
    // Start a new block
    BasicBlock *loop = new BasicBlock;
    // Need to rebind indexVarName to a local variable here
    // FIXME - Don't think this is correct - 
    // Consider case of a script
    // with a for loop.  After the for loop completes,
    // the value of the index needs to be restored to the
    // parent scope.
    _code->_syms->syms[indexVarName] = SYM_LOCAL_DEF | SYM_USED;
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
    reg_t loop_index = fetchConstant(_types->_double->makeScalar(0));
    BasicBlock *loop = new BasicBlock;
    emit(OP_JUMP,loop);
    useBlock(loop);
    BasicBlock *end = new BasicBlock;
    reg_t test_res = getRegister();
    emit(OP_LE,test_res,loop_index,iter_count);
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
  } else
    throw Exception("Unhandled for case...");
}

void Compiler::whileStatement(const Tree &t) {
  BasicBlock *body = new BasicBlock;
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
    //    specialFunctionCall(t,printIt);
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
  case TOK_DBSTEP:
    //    dbstepStatement(t);
    break;
  case TOK_DBTRACE:
    //    dbtraceStatement(t);
    break;
  case TOK_DBUP:
    // dbupStatement(t);
    break;
  case TOK_DBDOWN:
    //dbdown(t);
    break;
  case TOK_RETURN:
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
    //doDebugCycle();
    break;
  case TOK_GLOBAL:
    break;
  case TOK_PERSISTENT:
    break;
  case TOK_EXPR:
    expressionStatement(t,printIt);
    break;
  case TOK_NEST_FUNC:
    _currentSym = _currentSym->children[0];
    break;
  default:
    throw Exception("Unrecognized statement type");
  }  
}


// 

void Compiler::compile(const Tree &t) {
  SymbolPass p;
  // Start the code walk at a depth of 1 - if it's a function
  // definition instead of a script, the internal blocks will
  // be processed at the correct depth.
  p.walkCode(t,1);
  p.findCaptured();
  p.dump();
  _symsRoot = p.getRoot();
  _currentSym = _symsRoot;
  walkCode(t);
}

void Compiler::walkFunction(const Tree &t, bool nested) {
  CodeBlock *cp = new CodeBlock;
  cp->_syms = _currentSym;
  _codestack.push(cp);
  _code = cp;
  // Build up the variable list
  // By convention, the arguments are first

  for (FMMap<FMString, int>::const_iterator s = _code->_syms->syms.constBegin();
       s != _code->_syms->syms.constEnd(); ++s)
    {
      if (IS_LOCAL(s.value()))
	_code->_varlist += s.key();
      else if (IS_CAPTURED(s.value()))
	_code->_capturedlist += s.key();
      else if (IS_FREE(s.value()))
	_code->_freelist += s.key();
      else if (IS_DYNAMIC(s.value()))
	{
	  _code->_namelist += s.key();
	  _code->_varlist += s.key();
	}
      else
	_code->_namelist += s.key();
    }
  std::cout << "Compiling function " << t.child(1).text() << "\n";
  _code->_name = t.child(1).text();
  std::cout << "Symbol table is " << _currentSym->name << "\n";
  const Tree &rets = t.child(0);
  //  beginFunction(t.child(1).text(),nested);
  const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  // Create a basic block, and push it on the list
  useBlock(new BasicBlock);
  block(code);
  emit(OP_RETURN);
}


void Compiler::walkFunctionCollection(const Tree &t) {

  for (int i=0;i<t.numChildren();++i)
    switch (t.child(i).token())
      {
      case TOK_FUNCTION:
	{
	  _currentSym = _symsRoot->children[i];
	  walkFunction(t.child(i));
	  break;
	}
      default:
	throw Exception("Unexpected child of function collection");
      }
}

void Compiler::walkScript(const Tree &t) {
  CodeBlock *cp = new CodeBlock;
  cp->_syms = _currentSym;
  _codestack.push(cp);
  _code = cp;
  // Build up the variable list
  for (FMMap<FMString, int>::const_iterator s = _code->_syms->syms.constBegin();
       s != _code->_syms->syms.constEnd(); ++s)
    {
      if (IS_LOCAL(s.value()))
	_code->_varlist += s.key();
      else if (IS_CAPTURED(s.value()))
	_code->_capturedlist += s.key();
      else if (IS_FREE(s.value()))
	_code->_freelist += s.key();
      else if (IS_DYNAMIC(s.value()))
	{
	  _code->_namelist += s.key();
	  _code->_varlist += s.key();
	}
      else
	_code->_namelist += s.key();
    }
  useBlock(new BasicBlock);
  block(t.first());
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
	walkScript(t);
	break;
      }
    default:
      throw Exception("Unexpected code type for compilation");
    }
}

void Compiler::statement(const Tree &t) {
  if (t.is(TOK_QSTATEMENT))
    statementType(t.first(),false);
  else if (t.is(TOK_STATEMENT))
    statementType(t.first(),true);
}

void Compiler::block(const Tree &t) {
  const TreeList statements(t.children());
  for (TreeList::const_iterator i=statements.begin(); i!= statements.end(); ++i)
    statement(*i);
}

Module* Compiler::module() {
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
}

void Compiler::dump() {
  while (!_codestack.empty())
    {
      _code = _codestack.top();
      std::cout << "************************************************************\n";
      std::cout << "Symbols for function: " << _code->_syms->name << "\n";
      for (FMMap<FMString, int>::const_iterator s=_code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
	std::cout << "   Symbol: " << s.key() << " flags: " << symbolFlagsToString(s.value()) << "\n";
      std::cout << "************************************************************\n";      
      std::cout << "Const: ";
      for (int i=0;i<_code->_constlist.size();i++)
	std::cout << _code->_constlist[i].description() << " ";
      std::cout << "\n";
      std::cout << "Vars:  " << _code->_varlist << "\n";
      std::cout << "Captured:  " << _code->_capturedlist << "\n";
      std::cout << "Free:  " << _code->_freelist << "\n";
      std::cout << "Names:  " << _code->_namelist << "\n";
      std::cout << "Code:\n";
      for (int j=0;j<_code->_blocklist.size();j++)
	{
	  printf("  Block %0x\n",_code->_blocklist[j]);
	  BasicBlock *b = _code->_blocklist[j];
	  for (int i=0;i<b->_insnlist.size();i++)
	    {
	      printf("%03d   ",i);
	      Instruction insn = b->_insnlist[i];
	      int8_t opcode = opcode(insn._opcode);
	      printf("%-15s",getOpCodeName(opcode).c_str());
	      printf("%-25s",opcodeDecodeArgs(opcode,insn._opcode).c_str());
	      if (insn._target)	printf("%-15x",insn._target);
	      printf("\n");
	    }
	}
      _codestack.pop();
    }
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
  printf("%03d   ",ip);
  int8_t op = opcode(i._opcode);
  printf("%-20s",getOpCodeName(op).c_str());
  printf("%-20s",Compiler::opcodeDecode(op,i._opcode).c_str());
  if (i._target)	
    printf("%-15x",i._target);
  printf("\n");
}

void PrintBasicBlock(BasicBlock *b)
{
  printf("  Block %0x\n",b);
  for (int i=0;i<b->_insnlist.size();i++)
    PrintInsn(i,b->_insnlist[i]);
}

void PrintCodeBlock(CodeBlock *_code)
{
  std::cout << "************************************************************\n";
  std::cout << "Symbols for function: " << _code->_syms->name << "\n";
  for (FMMap<FMString, int>::const_iterator s=_code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
    std::cout << "   Symbol: " << s.key() << " flags: " << symbolFlagsToString(s.value()) << "\n";
  std::cout << "************************************************************\n";      
  std::cout << "Const: ";
  for (int i=0;i<_code->_constlist.size();i++)
    std::cout << _code->_constlist[i].description() << " ";
  std::cout << "\n";
  std::cout << "Vars:  " << _code->_varlist << "\n";
  std::cout << "Captured:  " << _code->_capturedlist << "\n";
  std::cout << "Free:  " << _code->_freelist << "\n";
  std::cout << "Names:  " << _code->_namelist << "\n";
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


void DumpBasicBlock(BasicBlock *b, int offset)
{
  printf("Basic Block : %x\n",b);
  printf("  offset    : %d\n",b->_offset);
  printf("  seen      : %d\n",b->_seen);
  printf("  insn count: %d\n",b->_insnlist.size());
  for (int i=0;i<b->_insnlist.size();++i)
    PrintInsn(i+offset,b->_insnlist[i]);
}

void PrintRawInsn(int ip, uint32_t insn)
{
  printf("%03d   ",ip);
  int8_t opcode = insn & 0xFF;
  printf("%-15s",getOpCodeName(opcode).c_str());
  printf("%-25s",Compiler::opcodeDecode(opcode,insn).c_str());
  printf("\n");
}

void FM::Disassemble(BaseTypes *_types, const Object &p)
{
  //  if (!p.isUserClass() || p.className() != "code_object")
  //    throw Exception("argument to disassemble is not a code_object");
  assert(p.type()->code() == TypeStruct);
  std::cout << "Name: " << _types->_struct->getScalar(p,"name").description() << "\n";
  std::cout << "Captured: " << _types->makeStringsFromCell(_types->_struct->getScalar(p,"captured")) << "\n";
  std::cout << "Free: " << _types->makeStringsFromCell(_types->_struct->getScalar(p,"free")) << "\n";
  std::cout << "Vars: " << _types->makeStringsFromCell(_types->_struct->getScalar(p,"vars")) << "\n";
  std::cout << "Names: " << _types->makeStringsFromCell(_types->_struct->getScalar(p,"names")) << "\n";
  Object consts = _types->_struct->getScalar(p,"consts");
  const Object* cp = _types->_cell->readOnlyData(consts);
  std::cout << "Consts: ";
  for (dim_t i=0;i<consts.dims().elementCount();i++)
    std::cout << cp[i].description() << " ";
  std::cout << "\n";
  Object code = _types->_struct->getScalar(p,"code");
  const uint32_t *opcodes = _types->_uint32->readOnlyData(code);
  std::cout << "Code: " << code.dims().elementCount() << " length\n";
  for (dim_t i=0;i<code.dims().elementCount();++i)
    PrintRawInsn(i,opcodes[i]);
}
