#ifndef __Symbol_hpp__
#define __Symbol_hpp__

#include "Tree.hpp"

namespace FM
{

  // A simplified symbol pass - we now realize that variables only 
  // come in the following flavors:
  //  Global, persistent, parameters (value and reference), return, dynamic
  // A dynamic variable is one that is bound at run time.  It can be locally
  // defined (i.e., in the current code's closed scope), or it can be a 
  // captured variable. 

  // TODO - turn this into a struct... it's getting too ugly
  // struct symbol_flag_t
  // {
  //   unsigned is_global : 1;
  //   unsigned is_persistent : 1;
  //   unsigned is_parameter : 1;
  //   unsigned is_reference : 1;
  //   unsigned is_return : 1;
  //   unsigned is_dynamic : 1;    
  // };

  typedef int32_t symbol_flag_t;

  const symbol_flag_t SYM_GLOBAL = 1;
  const symbol_flag_t SYM_PERSISTENT = (1 << 1);
  const symbol_flag_t SYM_PARAMETER = (1 << 2);
  const symbol_flag_t SYM_REFERENCE = (1 << 3);
  const symbol_flag_t SYM_RETURN = (1 << 4);
  const symbol_flag_t SYM_DYNAMIC = (1 << 5);
  const symbol_flag_t SYM_FREE = (1 << 6);
  const symbol_flag_t SYM_CAPTURED = (1 << 7);
  const symbol_flag_t SYM_NESTED = (1 << 8);
  const symbol_flag_t SYM_PROPERTY = (1 << 9);
  const symbol_flag_t SYM_METHOD = (1 << 10);
  const symbol_flag_t SYM_CONSTRUCTOR = (1 << 11);


  enum FunctionTypeEnum {
    NormalFunction = 0,
    NestedFunction = 1,
    MethodFunction = 2
  };

#define IS_DYNAMIC(x) (((x) & SYM_DYNAMIC) != 0)
#define IS_GLOBAL(x) (((x) & SYM_GLOBAL) != 0)
#define IS_PERSIST(x) (((x) & SYM_PERSISTENT) != 0)
#define IS_PARAMETER(x) (((x) & SYM_PARAMETER) != 0)
#define IS_REFERENCE(x) (((x) & SYM_REFERENCE) != 0)
#define IS_RETURN(x) (((x) & SYM_RETURN) != 0)
#define IS_FREE(x) (((x) & SYM_FREE) != 0)
#define IS_CAPTURED(x) (((x) & SYM_CAPTURED) != 0) 
#define IS_CELL(x) (IS_FREE(x) || IS_CAPTURED(x))
#define IS_NESTED(x) (((x) & SYM_NESTED) != 0)
#define IS_LOCAL(x) ((IS_DYNAMIC(x) || IS_PARAMETER(x) || IS_RETURN(x)) && (!IS_CAPTURED(x)))
#define IS_PROPERTY(x) (((x) & SYM_PROPERTY) != 0)
#define IS_METHOD(x) (((x) & SYM_METHOD) != 0)
#define IS_CONSTRUCTOR(x) (((x) & SYM_METHOD) != 0)
#define SYM_PARAM_POSITION(x) (((x) >> 12) & 0xFF)
#define SYM_RETURN_POSITION(x) (((x) >> 20) & 0xFF)

  class SymbolTable
  {
  public:
    FMString name;
    FMMap<FMString, symbol_flag_t> syms;
    FMVector<SymbolTable*> children;
    SymbolTable* parent;
  };

  // Walk an AST (for a function), and create the symbol table.
  class SymbolPass
  {
    SymbolTable *_root;
    SymbolTable *_current;
    void beginFunction(const FMString &name,bool nested);
    void addSymbol(const FMString &name, symbol_flag_t flags);
    void walkFunction(const Tree &t, FunctionTypeEnum funcType = NormalFunction);
    void walkClassDef(const Tree &t);
    void walkProperty(const Tree &t);
    void newSibling(SymbolTable *t);
    void newChild(SymbolTable *t);
    void popToParent();
    void dump(SymbolTable *t, int);
    void walkChildren(const Tree &t, bool nested);
    bool parentScopeDefines(const FMString &name);
    void markParentSymbolCaptured(const FMString &name);
  public:
    SymbolPass();
    void walkCode(const Tree &t, bool nested = false);
    SymbolTable *getRoot();
    void dump();
  };

  FMString symbolFlagsToString(symbol_flag_t flag);
}

#endif
