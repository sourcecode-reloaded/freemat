#ifndef __Compiler_hpp__
#define __Compiler_hpp__

#include <stdint.h>
#include <vector>
#include "Tree.hpp"
#include "Symbol.hpp"
#include "Register.hpp"
#include "Code.hpp"

// The python approach is to capture via analysis.  The MLAB approach (based on the documentation) is to capture all variables.  That means
// the problem is kicked to runtime. I like the python approach.  It makes the VM simpler (and faster).

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
namespace FM
{
  struct ThreadContext;

  class Compiler
  {
    RegisterBlock *_regpool;
    std::stack<CodeBlock *> _codestack;
    CodeBlock *_code;
    SymbolTable *_symsRoot;
    SymbolTable *_currentSym;
    std::stack<BasicBlock *> _continueblock;
    std::stack<BasicBlock *> _breakblock;
    ThreadContext *_ctxt;
  private:
    void reset();
    void useBlock(BasicBlock *b);
    void emit(int8_t opcode, reg_t reg1);
    void emit(int8_t opcode, reg_t reg1, reg_t reg2);
    void emit(int8_t opcode, reg_t reg1, reg_t reg2, reg_t reg3);
    void emit(int8_t opcode, reg_t reg1, reg_t reg2, const_t arg);
    void emit(int8_t opcode, reg_t reg1, const_t arg);
    void emit(int8_t opcode, const_t arg);
    void emit(int8_t opcode);
    void emit(int8_t opcode, reg_t reg1, BasicBlock *blk);
    void emit(int8_t opcode, BasicBlock *blk);
    reg_t startList();
    void pushList(reg_t list, reg_t x);
    void multiexpr(reg_t list, const Tree &t);
    reg_t doBinaryOperator(const Tree&, op_t);
    reg_t doUnaryOperator(const Tree&, op_t);
    reg_t doShortcutOr(const Tree& t);
    reg_t doShortcutAnd(const Tree& t);
    reg_t doubleColon(const Tree&);
    reg_t getRegister();
    const_t getConstantID(const Object &);
    const_t getConstantID(const FMString &);
    const_t getNameID(const FMString &);
    const_t getCellID(const FMString &);
    SymbolTable* getSymbolTableForClosure(const FMString &varname);
    reg_t cellDefinition(const Tree &t);
    reg_t matrixDefinition(const Tree &t);
    reg_t fetchConstant(const Object &);
    reg_t fetchClosure(const FMString &);
    reg_t fetchCell(const FMString &);
    reg_t fetchConstantString(const FMString &);
    reg_t fetchVariable(const FMString &, symbol_flags_t flags);
    reg_t fetchVariableOrFunction(const FMString &, reg_t args);
    void saveRegisterToName(const FMString &varname, reg_t b);
    reg_t flattenDereferenceTreeToStack(const Tree&, int);
    reg_t flattenDereferenceTreeToStackParensPrecomputed(const Tree &s, reg_t paren_children, int first_child);
    void deref(reg_t args, const Tree&);
    void rhs(reg_t list, const Tree&);
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
    reg_t doFunctionExpression(const Tree &t, reg_t narg_out);
    void walkFunctionCollection(const Tree &t);
    void walkScript(const Tree &t);
    void walkCode(const Tree &t);
    void walkFunction(const Tree &t, bool nested = false);
    void walkProperties(reg_t list, const Tree &t);
    void walkMethods(const Tree &t, Object &metaClass);
    void walkClassDef(const Tree &t);
  public:
    static std::string opcodeDecode(op_t opcode, insn_t val);
    Compiler(ThreadContext *b);
    void compile(const FMString &code);
    Module* module();
  };

  void Disassemble(ThreadContext *_ctxt, const Object &p);

  void DumpBasicBlock(BasicBlock *k, int offset);
}

#endif
