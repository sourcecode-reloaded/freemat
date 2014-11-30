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
    unsigned _getter       : 1;
    unsigned _setter       : 1;
    unsigned _static       : 1;
    unsigned _constant     : 1;
    unsigned _dependent    : 1;
    unsigned param_position    : 8;
    unsigned return_position   : 8;
    unsigned property_position : 16;

    inline symbol_flags_t() : _global(0), _persistent(0),
			      _parameter(0), _reference(0),
			      _return(0), _dynamic(0),
			      _free(0), _captured(0), _nested(0),
			      _property(0), _method(0), _constructor(0),
			      _getter(0), _setter(0), param_position(0), 
			      return_position(0), property_position(0),
			      _static(0), _constant(0), _dependent(0) {}

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
    inline bool is_getter() const {return _getter != 0;}
    inline bool is_setter() const {return _setter != 0;}
    inline bool is_local() const {return ((is_dynamic() || is_parameter() || is_return()) && (!is_captured()));}
    inline bool is_cell() const {return is_free() || is_captured();}
    inline bool is_static() const {return _static != 0;}
    inline bool is_constant() const {return _constant != 0;}
    inline bool is_dependent() const {return _dependent != 0;}

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
      if (_property) ret += " property:" + Stringify(property_position);
      if (_method) ret += " method";
      if (_constructor) ret += " constructor";
      if (_getter) ret += " getter";
      if (_setter) ret += " setter";
      if (_return) ret += " return:" + Stringify(return_position);
      if (_static) ret += " static";
      if (_constant) ret += " constant";
      if (_dependent) ret += " dependent";
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
    static symbol_flags_t PROPERTY(unsigned pos) {symbol_flags_t ret; ret._property = 1; ret.property_position = pos; return ret;}
    static symbol_flags_t METHOD() {symbol_flags_t ret; ret._method = 1; return ret;}
    static symbol_flags_t CONSTRUCTOR() {symbol_flags_t ret; ret._method = 1; ret._constructor = 1; return ret;}
    static symbol_flags_t GETTER() {symbol_flags_t ret; ret._method = 1; ret._getter = 1; return ret;}
    static symbol_flags_t SETTER() {symbol_flags_t ret; ret._method = 1; ret._setter = 1; return ret;}
    static symbol_flags_t STATIC() {symbol_flags_t ret; ret._static = 1; return ret;}
    static symbol_flags_t CONSTANT() {symbol_flags_t ret; ret._constant = 1; return ret;}
    static symbol_flags_t DEPENDENT() {symbol_flags_t ret; ret._dependent = 1; return ret;}
  };

  inline bool operator != (const symbol_flags_t &a, const symbol_flags_t &b) {
    return !((a._global == b._global) &&
	     (a._persistent == b._persistent) &&
	     (a._parameter == b._parameter) &&
	     (a._reference == b._reference) &&
	     (a._return == b._return) &&
	     (a._dynamic == b._dynamic) &&
	     (a._free == b._free) &&
	     (a._captured == b._captured) &&
	     (a._nested == b._nested) &&
	     (a._property == b._property) &&
	     (a._method == b._method) &&
	     (a._constructor == b._constructor) &&
	     (a._getter == b._getter) &&
	     (a._setter == b._setter) &&
	     (a.param_position == b.param_position) &&
	     (a.return_position == b.return_position) &&
	     (a.property_position == b.property_position) &&
	     (a._static == b._static) &&
	     (a._constant == b._constant) &&
	     (a._dependent == b._dependent));
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
    ret._getter |= b._getter;
    ret._setter |= b._setter;
    ret.param_position |= b.param_position;
    ret.return_position |= b.return_position;
    ret._static |= b._static;
    ret._constant |= b._constant;
    ret._dependent |= b._dependent;
    return ret;
  }

  enum FunctionTypeEnum {
    NormalFunction = 0,
    NestedFunction = 1,
    MethodFunction = 2,
    LocalFunction = 3,
    ConstructorFunction = 4,
    SetterFunction = 5,
    GetterFunction = 6
  };


  class SymbolTable
  {
  public:
    FMString name;
    FMMap<FMString, symbol_flags_t> syms;
    FMVector<SymbolTable*> children;
    SymbolTable* parent;
    int property_count;
    //
    int countReturnValues() {
      int return_count = 0;
      for (auto s = syms.constBegin(); s!= syms.constEnd(); ++s)
	if (s.value().is_return()) return_count++;
      return return_count;
    }
    int countParameterValues() {
      int param_count = 0;
      for (auto s = syms.constBegin(); s!= syms.constEnd(); ++s)
	if (s.value().is_parameter()) param_count++;
      return param_count;
    }
    FMString returnName(int ndx) {
      int ret_count = 0;
      for (auto s = syms.constBegin(); s!= syms.constEnd(); ++s)
	if (s.value().is_return()) {
	  if (ret_count == ndx)
	    return s.key();
	  else
	    ret_count++;
	}
      return FMString();
    }
    FMString parameterName(int ndx) {
      int param_count = 0;
      for (auto s = syms.constBegin(); s!= syms.constEnd(); ++s)
	if (s.value().is_parameter()) {
	  if (param_count == ndx)
	    return s.key();
	  else
	    param_count++;
	}
      return FMString();
    }
  };

  // Walk an AST (for a function), and create the symbol table.
  class SymbolPass
  {
    SymbolTable *_root;
    SymbolTable *_current;
    void beginFunction(const FMString &name,bool nested);
    void addSymbol(const FMString &name, symbol_flags_t flags);
    void walkFunction(const Tree &t, FunctionTypeEnum funcType = NormalFunction, symbol_flags_t flags = symbol_flags_t());
    void walkClassDef(const Tree &t);
    void walkProperty(const Tree &t, symbol_flags_t flags);
    void walkProperties(const Tree &t);
    void walkMethods(const Tree &t);
    symbol_flags_t walkAttributes(const Tree &t);
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

}

#endif
