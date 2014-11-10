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
  struct symbol_flags_t
  {
    unsigned _global       : 1;
    unsigned _persistent   : 1;
    unsigned _parameter    : 1;
    unsigned _reference    : 1;
    unsigned _return       : 1;
    unsigned _dynamic      : 1;    
    unsigned _free         : 1;
    unsigned _captured     : 1;
    unsigned _nested       : 1;
    unsigned _property     : 1;
    unsigned _method       : 1;
    unsigned _constructor  : 1;
    unsigned param_position  : 8;
    unsigned return_position : 8;

    inline symbol_flags_t() : _global(0), _persistent(0),
			      _parameter(0), _reference(0),
			      _return(0), _dynamic(0),
			      _free(0), _captured(0), _nested(0),
			      _property(0), _method(0), _constructor(0),
			      param_position(0), return_position(0) {}

    inline bool is_global() const {return _global != 0;}
    inline bool is_persistent() const {return _persistent != 0;}
    inline bool is_parameter() const {return _parameter != 0;}
    inline bool is_reference() const {return _reference != 0;}
    inline bool is_return() const {return _return != 0;}
    inline bool is_dynamic() const {return _dynamic != 0;}
    inline bool is_free() const {return _free != 0;}
    inline bool is_captured() const {return _captured != 0;}
    inline bool is_nested() const {return _nested != 0;}
    inline bool is_property() const {return _property != 0;}
    inline bool is_method() const {return _method != 0;}
    inline bool is_constructor() const {return _constructor != 0;}
    inline bool is_local() const {return ((is_dynamic() || is_parameter() || is_return()) && (!is_captured()));}
    inline bool is_cell() const {return is_free() || is_captured();}

    inline FMString str() const {
      FMString ret;
      if (_global) ret += " global";
      if (_persistent) ret += " persistent";
      if (_reference) ret += " reference";
      if (_dynamic) ret += " dynamic";
      if (_parameter) ret += " parameter:" + Stringify(param_position);
      if (_free) ret += " free";
      if (_captured) ret += " captured";
      if (_nested) ret += " nested";
      if (_property) ret += " property";
      if (_method) ret += " method";
      if (_constructor) ret += " constructor";
      if (_return) ret += " return:" + Stringify(return_position);
      return ret;
    }
		       
    static symbol_flags_t GLOBAL() {symbol_flags_t ret; ret._global = 1; return ret;}
    static symbol_flags_t PERSISTENT() {symbol_flags_t ret; ret._persistent = 1; return ret;}
    static symbol_flags_t PARAMETER(unsigned pos) {symbol_flags_t ret; ret._parameter = 1; ret.param_position = pos; return ret;}
    static symbol_flags_t REFERENCE() {symbol_flags_t ret; ret._reference = 1; return ret;}
    static symbol_flags_t RETURN(unsigned pos) {symbol_flags_t ret; ret._return = 1; ret.return_position = pos; return ret;}
    static symbol_flags_t DYNAMIC() {symbol_flags_t ret; ret._dynamic = 1; return ret;}
    static symbol_flags_t FREE() {symbol_flags_t ret; ret._free = 1; return ret;}
    static symbol_flags_t CAPTURED() {symbol_flags_t ret; ret._captured = 1; return ret;}
    static symbol_flags_t NESTED() {symbol_flags_t ret; ret._nested = 1; return ret;}
    static symbol_flags_t PROPERTY() {symbol_flags_t ret; ret._property = 1; return ret;}
    static symbol_flags_t METHOD() {symbol_flags_t ret; ret._method = 1; return ret;}
    static symbol_flags_t CONSTRUCTOR() {symbol_flags_t ret; ret._constructor = 1; return ret;}
  };

  inline bool operator != (const symbol_flags_t &a, const symbol_flags_t &b) {
    return !((a._global == b._global) ||
	     (a._persistent == b._persistent) ||
	     (a._parameter == b._parameter) ||
	     (a._reference == b._reference) ||
	     (a._return == b._return) ||
	     (a._dynamic == b._dynamic) ||
	     (a._free == b._free) ||
	     (a._captured == b._captured) ||
	     (a._nested == b._nested) ||
	     (a._property == b._property) ||
	     (a._method == b._method) ||
	     (a._constructor == b._constructor) ||
	     (a.param_position == b.param_position) ||
	     (a.return_position == b.return_position));
  }

  inline symbol_flags_t operator|(const symbol_flags_t &a, const symbol_flags_t &b) {
    symbol_flags_t ret = a;
    ret._global |= b._global;
    ret._persistent |= b._persistent;
    ret._parameter |= b._parameter;
    ret._reference |= b._reference;
    ret._return |= b._return;
    ret._dynamic |= b._dynamic;
    ret._free |= b._free;
    ret._captured |= b._captured;
    ret._nested |= b._nested;
    ret._property |= b._property;
    ret._method |= b._method;
    ret._constructor |= b._constructor;
    ret.param_position |= b.param_position;
    ret.return_position |= b.return_position;
    return ret;
  }
  
#if 0
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

#endif

  enum FunctionTypeEnum {
    NormalFunction = 0,
    NestedFunction = 1,
    MethodFunction = 2
  };


#if 0
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
#endif

  class SymbolTable
  {
  public:
    FMString name;
    FMMap<FMString, symbol_flags_t> syms;
    FMVector<SymbolTable*> children;
    SymbolTable* parent;
  };

  // Walk an AST (for a function), and create the symbol table.
  class SymbolPass
  {
    SymbolTable *_root;
    SymbolTable *_current;
    void beginFunction(const FMString &name,bool nested);
    void addSymbol(const FMString &name, symbol_flags_t flags);
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

#if 0
  FMString symbolFlagsToString(symbol_flag_t flag);
#endif

}

#endif
