#ifndef __Compiler_hpp__
#define __Compiler_hpp__

#include <stdint.h>
#include <vector>
#include "Tree.hpp"

// The python approach is to capture via analysis.  The MLAB approach (based on the documentation) is to capture all variables.  That means
// the problem is kicked to runtime. I like the python approach.  It makes the VM simpler (and faster).

const int16_t SYM_GLOBAL = 1;
const int16_t SYM_PERSISTENT = (1 << 1);
const int16_t SYM_LOCAL_DEF = (1 << 2);
const int16_t SYM_PARAMETER = (1 << 3);
const int16_t SYM_USED = (1 << 4);
const int16_t SYM_SCALAR = (1 << 5);
const int16_t SYM_REFERENCE = (1 << 6);
const int16_t SYM_RETURN = (1 << 7);
const int16_t SYM_FREE = (1 << 8);
const int16_t SYM_CAPTURED = (1 << 9);
const int16_t SYM_DYNAMIC = (1 << 10);

#define IS_DYNAMIC(x) ((x & SYM_DYNAMIC) != 0)
#define IS_LOCAL(x) (((x & (SYM_LOCAL_DEF | SYM_PARAMETER | SYM_RETURN)) != 0) && ((x & SYM_DYNAMIC) == 0))
#define IS_GLOBAL(x) ((x & SYM_GLOBAL) != 0)
#define IS_PERSIST(x) ((x & SYM_PERSISTENT) != 0)
#define IS_EXPLICIT_DECLARED(x) ((x & (SYM_GLOBAL | SYM_PERSISTENT | SYM_PARAMETER | SYM_RETURN)) != 0)
#define IS_CAPTURED(x) ((x & SYM_CAPTURED) != 0)
#define IS_FREE(x) ((x & SYM_FREE) != 0)

/*

SymTable: - used by the compiler

Symbols can have several flags - including: global, persistent, local, parameter, nonlocal, used, def_free (used, but not defined), scalar (future use)


Codeblock:
VariableNames : string list
Constants : array list
Locals: array list
Arguments:

TOK_REAL:
TOK_IMAG:
TOK_REALF:
TOK_IMAGF:
TOK_STRING:
LOAD_CONST <const_no> -->
   PUSH(GET_CONST(oparg))
TOK_VARIABLE:
// if global
   PUSH(GET_GLOBAL(oparg))
// if local
   PUSH(GET_LOCAL(oparg))
// 

BINARY_GET --> equivalent to X(Y)
  x = pop()
  y = pop()
  push x.get(y)
  
NARY_GET --> equivalent to 
  x = pop()
  n = pop()
  for i=1:n
    vec[i] = pop()
  push x.get(vec)

FIELD_GET
  x = pop()
  y = pop()
  push x.get(y.asString())

STORE_LOCAL id
  x = pop()
  local[id] = x

STORE_GLOBAL id
  x = pop()
  global[id] = x

CELL_UNPACK
  x = pop()
  y = array_from_cell_array(x)
  push(x)

END
  


binary op:

push A
push B
binop name

incr_prefix:
*/

struct OpCode
{
  int8_t opcode;
  int8_t opreg0;
  int8_t opreg1;
  int8_t opreg2;
};

typedef uint8_t op_t;
typedef int16_t const_t;

#include "OpCodes.h"

enum opcodemode
  {
    no_arguments,
    one_register,
    two_registers,
    three_registers,
    register_constant,
    register_variable,
    register_name,
    register_offset,
    register_captured,
    register_free,
    constant,
    offset
  };

// We want a tree of symbol tables... sort of
// S1 S2 S3
//        |
//        N1 N2 N3
//           |
//           N5

class SymbolTable
{
public:
  FMString name;
  FMMap<FMString, int> syms;
  FMVector<SymbolTable*> children;
  SymbolTable* parent;
};

struct Instruction
{
  int32_t _opcode;
  struct BasicBlock *_target;
  int32_t _position;
};

class BasicBlock
{
public:
  std::vector<Instruction> _insnlist;
  int32_t _offset;
  bool _seen;
  BasicBlock() : _insnlist(), _offset(0), _seen(0) {}
};


struct CodeBlock
{
  FMString _name;
  SymbolTable *_syms;
  std::vector<BasicBlock*> _blocklist;
  BasicBlock* _currentBlock;
  FMStringList _capturedlist;
  FMStringList _freelist;
  FMStringList _varlist;
  FMStringList _namelist;
  FMVector<Array> _constlist;
};

struct Module
{
  FMString _name;
  CodeBlock *_main;
  FMMap<FMString,CodeBlock*> _locals;
};

typedef std::pair<FMString,ArrayVector> assignment;

// This can be split out later...
class VM
{
  std::vector<Array> _stack;
  std::vector<Array> _regfile;
  std::vector<Array> _vars;
  FMStringList _varlist;
  int _sp;
  int _ip;
public:
  void executeBlock(CodeBlock *, bool singleStep = false);
  void doSubsasgnOp( Array &ref, const Array &b);
  void doSubsrefOp( Array & ref);
  void popVector(ArrayVector &x);
  void decodeAssignments(std::vector<assignment> & assignments);
  void dump();
  Array deref(const assignment&, const Array &);
  Array assign(Array A, const assignment & p, const Array &b);
  Array doAssign(Array A, const std::vector<assignment> & assignments, const Array &b);
};

// Walk an AST (for a function), and create the symbol table.
class SymbolPass
{
  SymbolTable *_root;
  SymbolTable *_current;
  void beginFunction(const FMString &name,bool nested);
  void addSymbol(const FMString &name, int32_t flags, int blockMustRun);
  void walkFunction(const Tree &t, bool nested = false);
  void newSibling(SymbolTable *t);
  void newChild(SymbolTable *t);
  void popToParent();
  void dump(SymbolTable *t, int);
  void markCaptured(SymbolTable *t);
  bool childNeedsSymbol(SymbolTable *t, const FMString &);
public:
  SymbolPass();
  void walkChildren(const Tree &t, int blockDepth);
  void walkCode(const Tree &t, int blockDepth);
  void findCaptured();
  SymbolTable *getRoot();
  void dump();
};

class RegisterBlock;

class Register
{
  RegisterBlock *_block;
  int _num;
public:
  Register(int num, RegisterBlock *block);
  ~Register();
  int num() const {return _num;}
};

typedef boost::shared_ptr<Register> reg_t;

class RegisterBlock
{
  std::set<int> _pool;
public:
  RegisterBlock(int poolsize);
  reg_t getRegister();
  void returnRegister(int num);
};

class Assembler
{
  std::vector<BasicBlock*> _postorder;
  CodeBlock *_code;
  std::vector<uint32_t> _vm_codes;
  void depthFirstSearch(BasicBlock* p);
  void computeJumpOffsets();
public:
  Assembler(CodeBlock *code);
  void run();
  void assemble();
  Array codeObject();
};

class Compiler
{
  RegisterBlock *_regpool;
  std::stack<CodeBlock *> _codestack;
  CodeBlock *_code;
  SymbolTable *_symsRoot;
  SymbolTable *_currentSym;
  std::stack<BasicBlock *> _continueblock;
  std::stack<BasicBlock *> _breakblock;
private:
  std::string opcodeDecodeArgs(op_t opcode, int32_t val);
  void useBlock(BasicBlock *b);
  void emit(int8_t opcode, reg_t reg1);
  void emit(int8_t opcode, reg_t reg1, reg_t reg2);
  void emit(int8_t opcode, reg_t reg1, reg_t reg2, reg_t reg3);
  void emit(int8_t opcode, reg_t reg1, const_t arg);
  void emit(int8_t opcode, const_t arg);
  void emit(int8_t opcode);
  void emit(int8_t opcode, reg_t reg1, BasicBlock *blk);
  void emit(int8_t opcode, BasicBlock *blk);
  reg_t startStackList();
  void endStackList(reg_t start);
  void multiexpr(const Tree &t);
  void push(reg_t x);
  reg_t popStack();
  reg_t doScalarFunctionExpression(const Tree&, bool);
  reg_t doBinaryOperator(const Tree&, op_t);
  reg_t doUnaryOperator(const Tree&, op_t);
  reg_t doShortcutOr(const Tree& t);
  reg_t doShortcutAnd(const Tree& t);
  void doPushConstantString(const FMString &);
  void doPushConstant(int32_t x);
  reg_t doubleColon(const Tree&);
  reg_t getRegister();
  const_t getConstantID(const Array &);
  const_t getConstantID(const FMString &);
  const_t getVariableID(const FMString &);
  const_t getFreeID(const FMString &);
  const_t getCapturedID(const FMString &);
  const_t getNameID(const FMString &);
  reg_t cellDefinition(const Tree &t);
  reg_t matrixDefinition(const Tree &t);
  reg_t fetchConstant(const Array &);
  reg_t fetchConstantString(const FMString &);
  reg_t fetchVariable(const FMString &, int flags);
  reg_t getStackRegister();
  reg_t readFromStack(reg_t addr);
  void saveRegisterToName(const FMString &varname, reg_t b);
  void flattenDereferenceTreeToStack(const Tree&, int);
  void deref(const Tree&);
  void restoreStack(reg_t x, const_t offset = 0);
  void rhs(const Tree&);
  void expressionStatement(const Tree&, bool);
  void incrementRegister(reg_t);
  void multiFunctionCall(const Tree&, bool);
  void assignment(const Tree &var, bool printIt, reg_t b);
  reg_t expression(const Tree &t);
  void block(const Tree & t);
  void statement(const Tree & t);
  void tryStatement(const Tree & t);
  void throwStatement(const Tree & t);
  void ifStatement(const Tree & t);
  void whileStatement(const Tree & t);
  void switchStatement(const Tree & t);
  void forStatement(const Tree & t);
  void statementType(const Tree &t, bool printIt);
  void doFunctionExpression(const Tree &t, reg_t narg_out);
  void walkFunctionCollection(const Tree &t);
  void walkScript(const Tree &t);
  void walkCode(const Tree &t);
  void walkFunction(const Tree &t, bool nested = false);
public:
  static std::string opcodeDecode(op_t opcode, int32_t val);
  Compiler();
  void compile(const Tree &t);
  void dump();
  Module* module();
};

void Disassemble(const Array &p);

#endif
