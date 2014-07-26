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

  FMString symbolFlagsToString(int32_t flag);
}

#endif
