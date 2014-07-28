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
  const int16_t SYM_GLOBAL = 1;
  const int16_t SYM_PERSISTENT = (1 << 1);
  const int16_t SYM_PARAMETER = (1 << 2);
  const int16_t SYM_REFERENCE = (1 << 3);
  const int16_t SYM_RETURN = (1 << 4);
  const int16_t SYM_DYNAMIC = (1 << 5);

#define IS_DYNAMIC(x) ((x & SYM_DYNAMIC) != 0)
#define IS_GLOBAL(x) ((x & SYM_GLOBAL) != 0)
#define IS_PERSIST(x) ((x & SYM_PERSISTENT) != 0)
#define IS_PARAMETER(x) ((x & SYM_PARAMETER) != 0)
#define IS_REFERENCE(x) ((x & SYM_REFERENCE) != 0)
#define IS_RETURN(x) ((x & SYM_RETURN) != 0)

  class SymbolTable
  {
  public:
    FMString name;
    FMMap<FMString, int> syms;
    FMVector<SymbolTable*> children;
    SymbolTable* parent;
  };

  // Walk an AST (for a function), and create the symbol table.
  class SymbolPass
  {
    SymbolTable *_root;
    SymbolTable *_current;
    void beginFunction(const FMString &name,bool nested);
    void addSymbol(const FMString &name, int32_t flags);
    void walkFunction(const Tree &t, bool nested = false);
    void newSibling(SymbolTable *t);
    void newChild(SymbolTable *t);
    void popToParent();
    void dump(SymbolTable *t, int);
  public:
    SymbolPass();
    void walkChildren(const Tree &t);
    void walkCode(const Tree &t);
    SymbolTable *getRoot();
    void dump();
  };

  FMString symbolFlagsToString(int32_t flag);
}

#endif
