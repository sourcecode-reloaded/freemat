#ifndef __JITFunc_hpp__
#define __JITFunc_hpp__

#ifdef HAVE_LLVM

#include <QString>
#include "JIT.hpp"
#include "SymbolTable.hpp"
#include "Context.hpp"
#include "Tree.hpp"

class SymbolInfo {
  bool isScalar;
  int argument_num;
  JITScalar address;
  JITType type;

  SymbolInfo(bool is_scalar, int arg_num, JITScalar base_address, JITType base_type) :
    isScalar(is_scalar), argument_num(arg_num), address(base_address), type(base_type) {}
  friend class JITFunc;

public:
  SymbolInfo(const SymbolInfo &other) { this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->address = other.address; this->type = other.type; }
  SymbolInfo &operator=(const SymbolInfo &other) { this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->address = other.address; this->type = other.type; return *this;}
  SymbolInfo() {} //need default constructor for SymbolTable
};

class JITFunc {
public:
  FM::SymbolTable<SymbolInfo> symbols;
  FM::SymbolTable<JITFunction> double_funcs;
  FM::SymbolTable<JITFunction> float_funcs;
  FM::SymbolTable<JITScalar> constants;
  JIT *jit;
  Interpreter *eval;
  Array** array_inputs;
  void** cache_array_bases;
  double* cache_array_rows;
  double* cache_array_cols;
  int argument_count;
  JITFunction func;
  JITBlock prolog, main_body, epilog;
  JITScalar this_ptr, retcode;
  Exception exception_store;
  QString symbol_prefix;
  int uid;
  void register_std_function(QString name);
  SymbolInfo* add_argument_array(QString name);
  SymbolInfo* add_argument_scalar(QString name, JITScalar val = NULL, bool override = false);
  JITType map_dataclass(DataClass aclass);
  DataClass map_dataclass(JITType type);
  DataClass map_dataclass(JITScalar val);
  void handle_success_code(JITScalar success);
  void initialize();
  SymbolInfo* define_local_symbol(QString name, JITScalar val);
  JITFunc(Interpreter *eval);
  JITScalar compile_expression(Tree* t);
  JITScalar compile_rhs(Tree* t);
  JITScalar compile_function_call(Tree* t);
  JITScalar compile_m_function_call(Tree* t);
  JITScalar compile_built_in_function_call(Tree* t);
  JITScalar compile_constant_function(QString symname);
  JITScalar compile_or_statement(Tree* t);
  JITScalar compile_and_statement(Tree* t);
  void compile_if_statement(Tree* t);
  void compile_for_block(Tree* t);
  void compile_assignment(Tree* t);
  void compile_statement(Tree* t);
  void compile_statement_type(Tree* t);
  void compile_block(Tree* t);
  void compile(Tree* t);
  void prep();
  void run();
};

#endif


#endif
