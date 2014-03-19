#include "Compiler.hpp"
#include "Algorithms.hpp"
#include "Print.hpp"
#include "Math.hpp"
#include "Struct.hpp"

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

Register::Register(int num, RegisterBlock *block) : _num(num), _block(block)
{
}

Register::~Register()
{
  _block->returnRegister(_num);
}

RegisterBlock::RegisterBlock(int poolsize)
{
  for (int i=0;i<poolsize;i++)
    _pool.insert(i);
}

reg_t RegisterBlock::getRegister()
{
  int a = *(_pool.begin());
  _pool.erase(_pool.begin());
  return boost::shared_ptr<Register>(new Register(a,this));
}

void RegisterBlock::returnRegister(int num)
{
  _pool.insert(num);
}

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
      return Stringify(constant(val)) + "  ; " + SummarizeArrayCellEntry(_code->_constlist[constant(val)]);
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

void Compiler::emit(int8_t opcode, reg_t reg1, reg_t reg2)
{
  Instruction i;
  i._opcode = opcode | (reg1->num() << 8) | (reg2->num() << 16);
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
void Compiler::doFunctionExpression(const Tree &t, reg_t narg_out) {
  reg_t sp = startStackList();
  FMString funcname = t.first().text();
  if ((t.numChildren() >= 2) && t.second().is(TOK_PARENS)) {
    const Tree &s(t.second());
    for (int p=0;p<s.numChildren();p++)
      multiexpr(s.child(p));
  }
  endStackList(sp);
  push(narg_out);
  emit(OP_CALL,getNameID(funcname));
}

reg_t Compiler::getRegister()
{
  return _regpool->getRegister();
}

void Compiler::push(reg_t x)
{
  emit(OP_PUSH,x);
}

reg_t Compiler::popStack()
{
  reg_t x = getRegister();
  emit(OP_POP,x);
  return x;
}

void Compiler::doPushConstantString(const FMString & t)
{
  push(fetchConstantString(t));
}

void Compiler::doPushConstant(int32_t x)
{
  push(fetchConstant(Array(x)));
}

// Invoke a function, but we expect a scalar argument
// -- expressionStatement, rhs
reg_t Compiler::doScalarFunctionExpression(const Tree &t, bool optional_output) {
  reg_t nargout = fetchConstant(Array(int32(optional_output ? 0 : 1)));
  reg_t sp = getStackRegister();
  doFunctionExpression(t,nargout);
  reg_t x = readFromStack(sp);
  return x;
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
  push(expression(t.first().first()));
  push(expression(t.first().second()));
  push(expression(t.second()));
  reg_t r0 = getRegister();
  emit(OP_DCOLON,r0); // Pops args 0,1,2 off of top of stack
  return r0;
}


//TODO - Collapse into getID(varname,flags)...

const_t Compiler::getVariableID(const FMString & t) {
  return _code->_varlist.indexOf(t);
}

const_t Compiler::getNameID(const FMString & t) {
  return _code->_namelist.indexOf(t);
}

const_t Compiler::getCapturedID(const FMString & t) {
  return _code->_capturedlist.indexOf(t);
}

const_t Compiler::getFreeID(const FMString &t) {
  return _code->_freelist.indexOf(t);
}

const_t Compiler::getConstantID(const FMString & t) {
  return getConstantID(Array(t));
}

const_t Compiler::getConstantID(const Array & t) {
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

void Compiler::assignment(const Tree &t, bool printIt, reg_t b) {
  FMString varname = t.first().text();
  if (t.numChildren() == 1)
    {
      saveRegisterToName(varname,b);
      return;
    }
  flattenDereferenceTreeToStack(t,1);
  int symflags = _code->_syms->syms[varname];
  if (IS_LOCAL(symflags))
    emit(OP_SUBSASGN,b,getVariableID(varname));
  else if (IS_GLOBAL(symflags))
    emit(OP_SUBSASGN_GLOBAL,b,getNameID(varname));
  else if (IS_PERSIST(symflags))
    emit(OP_SUBSASGN_PERSIST,b,getNameID(varname));
  else if (IS_CAPTURED(symflags))
    emit(OP_SUBSASGN_CAPTURED,b,getCapturedID(varname));
  else if (IS_FREE(symflags))
    emit(OP_SUBSASGN_FREE,b,getFreeID(varname));
  else if (IS_DYNAMIC(symflags))
    emit(OP_SUBSASGN_DYNAMIC,b,getNameID(varname));
  else
    throw Exception("Unhandled subsasgn case");
}

// We want to map a dereference tree to the stack.
void Compiler::flattenDereferenceTreeToStack(const Tree &s, int first_child) {
  reg_t x = startStackList();
  for (int index = first_child; index < s.numChildren();index++)
    deref(s.child(index));
  endStackList(x);
}

void Compiler::deref(const Tree &s) {
  switch (s.token())
    {
    case TOK_PARENS:
      {
	doPushConstantString("()");
	reg_t x = startStackList();
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(s.child(p));
	endStackList(x);
	break;
      }
    case TOK_BRACES:
      {
	doPushConstantString("{}");
	reg_t x = startStackList();
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(s.child(p));
	endStackList(x);
	break;
      }
    case '.':
      {
	doPushConstantString(".");
	doPushConstantString(s.first().text());
	doPushConstant(1);
	break;
      }
    case TOK_DYN:
      {
	doPushConstantString(".");
	reg_t p = expression(s.first());
	emit(OP_PUSH,p);
	doPushConstant(1);
      }
    default:
      throw Exception("Unknown expression!");
    }
}

reg_t Compiler::fetchConstant(const Array &constant)
{
  reg_t r0 = getRegister();
  emit(OP_LOAD_CONST,r0,getConstantID(constant));
  return r0;
}

reg_t Compiler::fetchConstantString(const FMString &constant)
{
  return fetchConstant(Array(constant));
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

reg_t Compiler::startStackList()
{
  reg_t x = getRegister();
  emit(OP_START_LIST,x);
  return x;
}

void Compiler::endStackList(reg_t start)
{
  emit(OP_END_LIST,start); // Push sp-start onto stack
}

reg_t Compiler::getStackRegister()
{
  reg_t x = getRegister();
  emit(OP_START_LIST,x);
  return x;
}

void Compiler::restoreStack(reg_t x, const_t offset)
{
  emit(OP_LOAD_STACK,x,offset);
}

reg_t Compiler::readFromStack(reg_t addr)
{
  reg_t sp = getStackRegister();
  emit(OP_LOAD_STACK,addr,const_t(1));
  reg_t x = popStack();
  emit(OP_LOAD_STACK,sp,const_t(0));
  return x;  
}

reg_t Compiler::doShortcutOr(const Tree &t) {
  reg_t ret = getRegister();
  BasicBlock *next = new BasicBlock;
  BasicBlock *fail1 = new BasicBlock;
  BasicBlock *fail2 = new BasicBlock;
  BasicBlock *end = new BasicBlock;
  emit(OP_JUMP_ZERO,expression(t.first()),fail1);
  emit(OP_LOAD_CONST,ret,getConstantID(Array(bool(true))));
  emit(OP_JUMP,end);
  useBlock(fail1);
  emit(OP_JUMP_ZERO,expression(t.second()),fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(Array(bool(true))));
  emit(OP_JUMP,end);
  useBlock(fail2);
  emit(OP_LOAD_CONST,ret,getConstantID(Array(bool(false))));
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
  emit(OP_LOAD_CONST,ret,getConstantID(Array(bool(true))));
  emit(OP_JUMP,end);
  useBlock(fail);
  emit(OP_LOAD_CONST,ret,getConstantID(Array(bool(false))));
  emit(OP_JUMP,end);
  useBlock(end);
  return ret;
}

// Takes an expression like A(...).foo{32} and
// expands it (possibly into multiple values).  They
// are left on the stack.  Caller is responsible
// for removing them.
void Compiler::rhs(const Tree &t) {
  FMString varname = t.first().text();
  int min_children_to_reindex = 1;
  reg_t x;
  int symflags = _code->_syms->syms[varname];
  x = fetchVariable(varname,symflags);
  min_children_to_reindex = 1;
  if (t.numChildren() > min_children_to_reindex)
    {
      flattenDereferenceTreeToStack(t,min_children_to_reindex);
      emit(OP_SUBSREF,x);
    }
  else
    push(x);
}

// TODO - what about "ans"?
void Compiler::expressionStatement(const Tree &t, bool printIt) {
  reg_t p = expression(t.first());
  if (printIt) emit(OP_PRINT,p);
}

reg_t Compiler::cellDefinition(const Tree &t) {
  reg_t y = startStackList();
  for (int i=0;i<t.numChildren();++i) {
    const Tree & s(t.child(i));
    reg_t x = startStackList();
    for (int j=0;j<s.numChildren();j++)
      multiexpr(s.child(j));
    endStackList(x);
    emit(OP_CELLROWDEF);
  }
  endStackList(y);
  reg_t z = getRegister();
  emit(OP_VCAT,z);
  return z;
}

reg_t Compiler::matrixDefinition(const Tree &t) {
  reg_t y = startStackList();
  for (int i=0;i<t.numChildren();++i) {
    const Tree & s(t.child(i));
    reg_t x = startStackList();
    for (int j=0;j<s.numChildren();j++)
      multiexpr(s.child(j));
    endStackList(x);
    emit(OP_HCAT);
  }
  endStackList(y);
  reg_t z = getRegister();
  emit(OP_VCAT,z);
  return z;
}

void Compiler::multiexpr(const Tree &t) {
  if (t.is(TOK_VARIABLE))
    rhs(t);
  else if (!t.is(TOK_KEYWORD))
    push(expression(t));
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
      reg_t sp = getStackRegister();
      rhs(t);
      restoreStack(sp,1);
      return popStack();
    }
  case TOK_REAL:
  case TOK_IMAG:
  case TOK_REALF:
  case TOK_IMAGF:
  case TOK_STRING:
    {
      return fetchConstant(t.array());
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
  reg_t fp = getStackRegister();
  reg_t sp = getStackRegister();
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
	{
	  flattenDereferenceTreeToStack(t,1);
	  emit(OP_LHSCOUNT,lhsCount,getVariableID(varname));
	}
    }
  // Make the function call here
  doFunctionExpression(t.second(),lhsCount);
  // Now we allocate the resulting assignments
  for (int ind=0;ind<s.size();++ind)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (t.numChildren() == 1)
	{
	  reg_t b = readFromStack(sp);
	  incrementRegister(sp);
	  saveRegisterToName(varname,b);
	}
      else
	{
	  flattenDereferenceTreeToStack(t,1);
	  emit(OP_SUBSASGNM,sp,getVariableID(varname));
	}
    }
  restoreStack(fp);
}

Compiler::Compiler() {
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
    reg_t colnum = fetchConstant(Array(index_t(1)));
    // Start a new block
    BasicBlock *loop = new BasicBlock;
    // Need to rebind indexVarName to a local variable here
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

static FMString flagsToString(int32_t flag);


void Compiler::dump() {
  while (!_codestack.empty())
    {
      _code = _codestack.top();
      std::cout << "************************************************************\n";
      std::cout << "Symbols for function: " << _code->_syms->name << "\n";
      for (FMMap<FMString, int>::const_iterator s=_code->_syms->syms.constBegin(); s != _code->_syms->syms.constEnd(); ++s)
	std::cout << "   Symbol: " << s.key() << " flags: " << flagsToString(s.value()) << "\n";
      std::cout << "************************************************************\n";      
      std::cout << "Const: ";
      for (int i=0;i<_code->_constlist.size();i++)
	std::cout << SummarizeArrayCellEntry(_code->_constlist[i]) << " ";
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

static FMString flagsToString(int32_t flag)
{
  FMString ret;
  if (flag & SYM_GLOBAL) ret += " global";
  if (flag & SYM_PERSISTENT) ret += " persist";
  if (flag & SYM_LOCAL_DEF) ret += " localdef";
  if (flag & SYM_PARAMETER) ret += " parameter";
  if (flag & SYM_USED) ret += " used";
  if (flag & SYM_SCALAR) ret += " scalar";
  if (flag & SYM_REFERENCE) ret += " reference";
  if (flag & SYM_RETURN) ret += " return";
  if (flag & SYM_FREE) ret += " free";
  if (flag & SYM_CAPTURED) ret += " captured";
  if (flag & SYM_DYNAMIC) ret += " dynamic";
  return ret;
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


SymbolPass::SymbolPass() {
  _root = new SymbolTable;
  _root->parent = NULL;
  _root->name = "root";
  _current = _root;
}

SymbolTable * SymbolPass::getRoot() {
  return _root;
}

void SymbolPass::newChild(SymbolTable *g) {
  g->parent = _current;
  _current->children.push_back(g);
  _current = g;
}

void SymbolPass::newSibling(SymbolTable *g) {
  if (_current == _root)
    {
      g->parent = _root;
      _root->children.push_back(g);
    }
  else
    {
      g->parent = _current->parent;
      _current->parent->children.push_back(g);
    }
  _current = g;
}

void SymbolPass::popToParent() {
  if (_current == _root)
    throw Exception("Unable to pop to parent");
  _current = _current->parent;
}

void SymbolPass::walkChildren(const Tree &t, int blockDepth) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index),blockDepth);
}

void SymbolPass::beginFunction(const FMString &name, bool nested)
{
  SymbolTable *t = new SymbolTable;
  t->name = name;
  if (!nested)
    newSibling(t);
  else
    newChild(t);
}

void SymbolPass::addSymbol(const FMString &name, int32_t flags, int blockDepth)
{
  if (!_current->syms.contains(name))
    {
      // If the first time a symbol is used, it is undeclared, it is automatically dynamic
      if (flags & SYM_USED) flags |= SYM_DYNAMIC;
      std::cout << "Add new symbol: " << name << " flags [" 
		<< flagsToString(flags) << "] must run: " 
		<< blockDepth << "\n";
      // Do not allow localdef if we aren't at a blockDepth of 2
      if ((flags & SYM_LOCAL_DEF) && (blockDepth > 1))
	  flags = (flags ^ SYM_LOCAL_DEF) | SYM_DYNAMIC;
      _current->syms[name] = flags;
    }
  else
    {
      int32_t oldflags = _current->syms[name];
      if (IS_EXPLICIT_DECLARED(oldflags) && (flags & SYM_LOCAL_DEF)) return;
      if ((oldflags & SYM_USED) && 
	  (!(oldflags & SYM_LOCAL_DEF)) &&
	  (flags & SYM_LOCAL_DEF)) flags = SYM_DYNAMIC;
      if ((oldflags | flags) != oldflags)
	{
	  std::cout << "Update of symbol: " << name 
		    << " new flags [" << flagsToString(flags) 
		    << "] old flags [" << flagsToString(oldflags) 
		    << "] must run: " << blockDepth << "\n";
	  _current->syms[name] = (oldflags | flags);
	}
    }
}

void SymbolPass::walkFunction(const Tree &t, bool nested) {
  const Tree &rets = t.child(0);
  beginFunction(t.child(1).text(),nested);
  const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  for (int index=0;index < rets.numChildren();index++)
    addSymbol(rets.child(index).text(), SYM_RETURN, 0);
  for (int index=0;index < args.numChildren();index++)
    if (args.child(index).is('&'))
      addSymbol(args.child(index).first().text(), SYM_REFERENCE | SYM_PARAMETER, 0);
    else
      addSymbol(args.child(index).text(), SYM_PARAMETER, 0);
  walkCode(code, 0);  
  if (nested) popToParent();
}

void SymbolPass::walkCode(const Tree &t, int blockDepth) {
  switch (t.token())
    {
    case '=':
      {
	walkCode(t.second(),blockDepth);
	const Tree &lhs = t.first();
	addSymbol(lhs.first().text(),SYM_LOCAL_DEF,blockDepth);
	for (int index=1;index < lhs.first().numChildren();++index) 
	  walkCode(lhs.first().child(index),blockDepth);
	break;
      }
    case TOK_GLOBAL:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_GLOBAL,blockDepth);
	    walkChildren(s, blockDepth);
	  }
	break;
      }
    case TOK_VARIABLE:
      {
	addSymbol(t.first().text(),SYM_USED,blockDepth);
	walkChildren(t, blockDepth);
	break;
      }
    case TOK_FOR:
      {
	addSymbol(t.first().first().text(),SYM_LOCAL_DEF,blockDepth);
	walkCode(t.first().second(),blockDepth);
	walkCode(t.second(),false);
	break;
      }
    case TOK_PERSISTENT:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_PERSISTENT,blockDepth);
	    walkChildren(s,blockDepth);
	  }
	break;
      }
    case TOK_MULTI:
      {
	const Tree &lhs = t.first();
	for (int index=0;index < lhs.numChildren();++index)
	  {
	    addSymbol(lhs.child(index).first().text(),SYM_LOCAL_DEF,blockDepth);
	    walkCode(lhs.child(index),blockDepth);
	  }
	const Tree &rhs = t.second();
	walkCode(rhs,blockDepth);
	break;
      }
      //    case TOK_FUNCTION_DEFS:
    case TOK_FUNCTION:
      {
	walkFunction(t);
	break;
      }
    case TOK_NEST_FUNC:
      {
	walkFunction(t,true);
	break;
      }
    case TOK_BLOCK:
      {
	// Blocks (in general) do not have to run
	std::cout << "Block encountered: depth increased to " << blockDepth+1 << "\n";
	walkChildren(t,blockDepth+1);
	break;
      }
    default:
      walkChildren(t,blockDepth);
    }
}

bool SymbolPass::childNeedsSymbol(SymbolTable *t, const FMString &symname)
{
  std::cout << "Checking if " << t->name << " needs symbol " << symname << "\n";
  if (!t->syms.contains(symname))
    {
      for (int i=0;i<t->children.size();++i)
	if (childNeedsSymbol(t->children[i],symname)) return true;
      return false;
    }
  if (IS_EXPLICIT_DECLARED(t->syms[symname])) return false;
  t->syms[symname] |= SYM_FREE;
  return true;
}

void SymbolPass::markCaptured(SymbolTable *t)
{
  for (FMMap<FMString,int>::iterator i=t->syms.begin();i!=t->syms.end();++i)
    {
      for (int j=0;j<t->children.size();++j)
	if (childNeedsSymbol(t->children[j],i.key()))
	  {
	    std::cout << "Child " << t->name << " needs symbol " << i.key() << "\n";
	    i.value() = (i.value() | SYM_CAPTURED) & (~SYM_LOCAL_DEF);
	  }
    }
  for (int i=0;i<t->children.size();++i)
    markCaptured(t->children[i]);
}

void SymbolPass::findCaptured()
{
  // The root doesn't allow for captured variables.  All other functions do
  for (int i=0;i<_root->children.size();++i)
    {
      markCaptured(_root->children[i]);
    }
}

void SymbolPass::dump()
{
  std::cout << "Symbol Tables:\n";
  dump(_root,0);
}

void SymbolPass::dump(SymbolTable *t, int indent)
{
  std::string spacer(indent,' ');
  std::cout << "************************************************************\n";
  std::cout << spacer << "Symbols for function: " << t->name << "\n";
  for (FMMap<FMString, int>::const_iterator s=t->syms.constBegin(); s != t->syms.constEnd(); ++s)
    std::cout << spacer << "   Symbol: " << s.key() << " flags: " << flagsToString(s.value()) << "\n";
  for (int i=0;i<t->children.size();++i)
    dump(t->children[i],indent+3);
}


// The output of the symbol pass will then be:
// 

void VM::dump()
{
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
}

#define REG1 _regfile[reg1(insn)]
#define REG2 _regfile[reg2(insn)]
#define REG3 _regfile[reg3(insn)]

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
  ref = doAssign(ref, assignments, b);
}

void VM::executeBlock(CodeBlock *code, bool singleStep)
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
    std::cout << "   Symbol: " << s.key() << " flags: " << flagsToString(s.value()) << "\n";
  std::cout << "************************************************************\n";      
  std::cout << "Const: ";
  for (int i=0;i<_code->_constlist.size();i++)
    std::cout << SummarizeArrayCellEntry(_code->_constlist[i]) << " ";
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

Assembler::Assembler(CodeBlock *code) : _code(code)
{
}

void Assembler::run()
{
  depthFirstSearch(_code->_blocklist[0]);
  computeJumpOffsets();
  assemble();
}

void Assembler::computeJumpOffsets()
{
  std::cout << "Jump Offset computation!\n";
  int total_size = 0;
  for (int i=0;i<_postorder.size();++i)
    {
      total_size += _postorder[i]->_insnlist.size();
      printf("Block %x - offset %d\n",_postorder[i],total_size);
      _postorder[i]->_offset = total_size;
    }
  // Update the jump offsets
  for (int i=0;i<_postorder.size();++i)
    {
      BasicBlock *b = _postorder[i];
      for (int j=0;j<b->_insnlist.size();j++)
	{
	  if (b->_insnlist[j]._target)
	    {
	      // Compute the address of the jump target
	      int target_address = total_size-b->_insnlist[j]._target->_offset;
	      b->_insnlist[j]._opcode |= (target_address << 16);
	    }
	}
    }
}

void Assembler::depthFirstSearch(BasicBlock *b) 
{
  if (b->_seen) return;
  b->_seen = 1;
  for (int i=0;i<b->_insnlist.size();++i)
    {
      if (b->_insnlist[i]._target)
	depthFirstSearch(b->_insnlist[i]._target);
    }
  _postorder.push_back(b);
}

void Assembler::assemble()
{
  int ip = 0;
  for (int i=0;i<_postorder.size();i++)
    {
      BasicBlock *b = _postorder[_postorder.size()-1-i];
      for (int j=0;j<b->_insnlist.size();j++)
	_vm_codes.push_back(b->_insnlist[j]._opcode);
    }
}

Array Assembler::codeObject()
{
  // Compute the codeObject for the code block
  StructArray qp;
  qp.insert("name",Array(_code->_name));
  BasicArray<uint32_t> opcodes(NTuple(_vm_codes.size(),1));
  memcpy(opcodes.data(),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint32_t));
  qp.insert("code",Array(opcodes));
  qp.insert("captured",CellArrayFromStringVector(_code->_capturedlist));
  qp.insert("free",CellArrayFromStringVector(_code->_freelist));
  qp.insert("vars",CellArrayFromStringVector(_code->_varlist));
  qp.insert("names",CellArrayFromStringVector(_code->_namelist));
  Array consts(CellArray,NTuple(1,_code->_constlist.size()));
  BasicArray<Array> &rp(consts.real<Array>());
  for (index_t i=1;i<=_code->_constlist.size();i++)
    rp.set(i,_code->_constlist[i-1]);
  qp.insert("consts",consts);
  qp.insert("locals",EmptyConstructor());
  qp.updateDims();
  Array ret(qp);
  ret.structPtr().setClassName("code_object");
  return ret;
}

void PrintRawInsn(int ip, uint32_t insn)
{
  printf("%03d   ",ip);
  int8_t opcode = insn & 0xFF;
  printf("%-15s",getOpCodeName(opcode).c_str());
  printf("%-25s",Compiler::opcodeDecode(opcode,insn).c_str());
  printf("\n");
}

void Disassemble(const Array &p)
{
  if (!p.isUserClass() || p.className() != "code_object")
    throw Exception("argument to disassemble is not a code_object");
  const StructArray &qp = p.constStructPtr();
  std::cout << "Name: " << qp["name"][1].asString() << "\n";
  std::cout << "Captured: " << StringVectorFromArray(qp["captured"][1]) << "\n";
  std::cout << "Free: " << StringVectorFromArray(qp["free"][1]) << "\n";
  std::cout << "Vars: " << StringVectorFromArray(qp["vars"][1]) << "\n";
  std::cout << "Names: " << StringVectorFromArray(qp["names"][1]) << "\n";
  const BasicArray<Array>& cp(qp["consts"][1].constReal<Array>());
  std::cout << "Consts: ";
  for (index_t i=1;i<=cp.length();++i)
    std::cout << SummarizeArrayCellEntry(cp[i]) << " ";
  std::cout << "\n";
  const BasicArray<uint32_t>& opcodes(qp["code"][1].constReal<uint32_t>());
  std::cout << "Code: " << opcodes.length() << " length\n";
  for (index_t i=1;i<=opcodes.length();++i)
    PrintRawInsn(i-1,opcodes[i]);
}
