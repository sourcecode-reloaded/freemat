#include "Symbol.hpp"
#include <iostream>
#include "Exception.hpp"

using namespace FM;

SymbolPass::SymbolPass() {
  _root = new SymbolTable;
  _root->parent = NULL;
  _root->name = "root";
  _root->property_count = 0;
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

void SymbolPass::walkChildren(const Tree &t, ScopeTypeEnum scopeType) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index),scopeType);
}

bool SymbolPass::parentScopeDefines(const FMString &name) {
  if (_current == _root) return false;
  SymbolTable *sp = _current->parent;
  while (1) 
    {
      if (sp->syms.contains(name)) return true;
      sp = sp->parent;
      if (!sp || (sp == _root)) return false;
    }
  return false;
}

void SymbolPass::markParentSymbolCaptured(const FMString &name) {
  if (_current == _root) return;
  SymbolTable *sp = _current->parent;
  while (1)
    {
      if (sp->syms.contains(name) && !sp->syms[name].is_free())
	{
	  symbol_flags_t old_flag = sp->syms[name];
	  if (old_flag.is_captured()) return;
	  old_flag._captured = 1;
	  old_flag._dynamic = 0;
	  std::cout << "CAPTURE " << name << "\n";
	  std::cout << "  FLAG: " << old_flag.str() << "\n";
	  sp->syms[name] = old_flag;
	  return;
	}
      else
	{
	  sp->syms[name] = symbol_flags_t::FREE();
	  std::cout << "FLOW THROUGH: " << name << "\n";
	}
      sp = sp->parent;
      if (sp == _root) return;
    }
}

void SymbolPass::beginFunction(const FMString &name, bool nested)
{
  SymbolTable *t = new SymbolTable;
  t->name = name;
  t->property_count = 0;
  if (!nested)
    newSibling(t);
  else
    newChild(t);
}

void SymbolPass::addSymbol(const FMString &name, symbol_flags_t flags)
{
  if (!_current->syms.contains(name))
    {
      _current->syms[name] = flags;
      std::cout << "Insert of symbol: " << name << " with flags " << flags.str() << "\n";
    }
  else
    {
      symbol_flags_t oldflags = _current->syms[name];
      if ((oldflags | flags) != oldflags)
	{
	  std::cout << "Update of symbol: " << name 
		    << " new flags [" << flags.str() 
		    << "] old flags [" << oldflags.str()
		    << "]\n";
	  _current->syms[name] = (oldflags | flags);
	}
    }
}

void SymbolPass::walkProperty(const Tree &t, symbol_flags_t attr) {
  if (_current->syms.contains(t.text()))
    throw Exception("Property " + t.text() + " cannot be defined more than once in definition of class " + _current->name);
  addSymbol(t.text(),symbol_flags_t::PROPERTY(_current->property_count++) | attr);
  if (t.hasChildren())
    walkCode(t.first());
}


static bool activeAttribute(const Tree &t, const FMString &name) {
  if (t.numChildren() == 0) 
    return (t.text().toLower() == name);
  else
    return ((t.text().toLower() == name) &&
	    (t.first().text().toLower() == "true"));
}


symbol_flags_t SymbolPass::walkAttributes(const Tree &t) {
  std::cout << "ATTRIBUTES\n";
  symbol_flags_t p;
  for (int i=0;i<t.numChildren();i++)
    {
      std::cout << "ATTRIBUTE:" << t.child(i).text() << "\n";
      if (activeAttribute(t.child(i),"static"))
	p._static = 1;
      if (activeAttribute(t.child(i),"constant"))
	p._constant = 1;
      if (activeAttribute(t.child(i),"dependent"))
	p._dependent = 1;
    }
  return p;
}

void SymbolPass::walkProperties(const Tree &t) {
  int start = 0;
  symbol_flags_t attr;
  if ((t.numChildren() > 0) && t.first().is(TOK_ATTRIBUTES))
    {
      attr = walkAttributes(t.first());
      start = 1;
    }
  for (int j=start;j<t.numChildren();j++)
    walkProperty(t.child(j),attr);
}

void SymbolPass::walkMethods(const Tree &t) {
  int start = 0;
  symbol_flags_t attr;
  if ((t.numChildren() > 0) && t.first().is(TOK_ATTRIBUTES))
    {
      attr = walkAttributes(t.first());
      start = 1;
    }
  for (int j=start;j<t.numChildren();j++)
    {
      const Tree &p = t.child(j);
      FMString fname = p.child(1).text();
      if (fname == _current->name)
	walkFunction(p,ConstructorFunction,attr);
      else if (fname.startsWith("set."))
	walkFunction(p,SetterFunction,attr);
      else if (fname.startsWith("get."))
	walkFunction(p,GetterFunction,attr);
      else
	walkFunction(p,MethodFunction,attr);
    }
}

void SymbolPass::walkClassDef(const Tree &t) {
  _current->name = t.first().text();
  for (int i=1;i<t.numChildren();i++)
    {
      switch (t.child(i).token())
	{
	case TOK_PROPERTIES:
	  walkProperties(t.child(i));
	  break;
	case TOK_METHODS:
	  walkMethods(t.child(i));
	  break;
	case '<':
	  break;
	default:
	  throw Exception("Unhandled classdef block");
	}
    }
}

void SymbolPass::walkFunction(const Tree &t, FunctionTypeEnum funcType, symbol_flags_t attr) {
  FMString name = t.child(1).text();
  switch (funcType)
    {
    case NestedFunction:
      {
	if (_current->syms.contains(name))
	  {
	    symbol_flags_t old_flag = _current->syms[name];
	    old_flag._nested = 1;
	    old_flag._dynamic = 0;
	    _current->syms[name] = old_flag | attr;
	  }
	else
	  addSymbol(name, symbol_flags_t::NESTED() | attr);
	break;
      }
    case MethodFunction:
      {
	if (_current->syms.contains(name))
	  throw Exception("Cannot redefine symbol " + name + " in class " + _current->name);
	addSymbol(name, symbol_flags_t::METHOD() | attr);
	break;
      }
    case ConstructorFunction:
      {
	if (_current->syms.contains(name))
	  throw Exception("Only one constructor allowed for class " + _current->name);
	addSymbol(name, symbol_flags_t::CONSTRUCTOR() | attr);
	break;
      }
    case GetterFunction:
      {
	if (_current->syms.contains(name))
	  throw Exception("Cannot redefine getter function " + name + " in class " + _current->name);
	addSymbol(name, symbol_flags_t::GETTER() | attr);
	break;
      }
    case SetterFunction:
      {
	if (_current->syms.contains(name))
	  throw Exception("Cannot redefine setter function " + name + " in class " + _current->name);
	addSymbol(name, symbol_flags_t::SETTER() | attr);
	break;
      }      
    default:
      {
      }
    }
  const Tree &rets = t.child(0);
  beginFunction(name,funcType == NestedFunction);
  const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  for (int index=0;index < args.numChildren();index++)
    if (args.child(index).is('&'))
      addSymbol(args.child(index).first().text(), symbol_flags_t::REFERENCE() | symbol_flags_t::PARAMETER(index));
    else
      addSymbol(args.child(index).text(), symbol_flags_t::PARAMETER(index));
  for (int index=0;index < rets.numChildren();index++)
    addSymbol(rets.child(index).text(), symbol_flags_t::RETURN(index));
  // Check that the constructor contains one return
  // And that the return does not appear in the list of parameters.
  if (funcType == ConstructorFunction)
    {
      if (_current->countReturnValues() != 1)
	throw Exception("Constructor for " + _current->name + " must return exactly 1 value");
      // Get the name of the object being constructed
      FMString objname = _current->returnName(0);
      // Check that it is not a parameter
      symbol_flags_t objflags = _current->syms[objname];
      if (objflags.is_parameter())
	throw Exception("Object returned by constructor cannot appear as an argument: i.e., function A = ClassName(A,...) is not allowed");
      // Insert the object into the parameter list at the front.
      objflags._parameter = 1;
      objflags.param_position = 0;
      for (auto i=_current->syms.begin();i!=_current->syms.end();++i)
	{
	  if (i.value().is_parameter()) {
	    symbol_flags_t flags = i.value();
	    flags.param_position++;
	    i.value() = flags;
	  }
	}
      _current->syms[objname] = objflags;
    }
  if (funcType == NestedFunction)
    walkCode(code, NestedScopeType);  
  else
    walkCode(code, NormalScopeType);
  switch (funcType)
    {
    case NestedFunction:
    case MethodFunction:
    case GetterFunction:
    case SetterFunction:
      popToParent();
      break;
    case ConstructorFunction:
      addSymbol(name,symbol_flags_t::DYNAMIC());
      popToParent();
      break;
    }
}

void SymbolPass::walkAnonymousFunction(const Tree &t) {
  std::cout << "Walk anonymous function " << t.context() << "\n";
  FMString fname = "anon" + Stringify(t.context());
  addSymbol(fname, symbol_flags_t::NESTED());
  SymbolTable *s = new SymbolTable;
  s->name = fname;
  s->property_count = 0;
  newChild(s);
  const Tree &args = t.child(0);
  const Tree &code = t.child(1);
  for (int index=0;index < args.numChildren();index++)
    addSymbol(args.child(index).text(), symbol_flags_t::PARAMETER(index));
  // Anonymous functions have an implicitly defined return value 
  addSymbol("_", symbol_flags_t::RETURN(0));
  walkCode(code,AnonymousScopeType);
  popToParent();
}

void SymbolPass::walkCode(const Tree &t, ScopeTypeEnum scopeType) {
  switch (t.token())
    {
    case TOK_GLOBAL:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),symbol_flags_t::GLOBAL());
	    walkChildren(s,scopeType);
	  }
	break;
      }
    case '@':
      {
	if (t.first().is(TOK_IDENT))
	  {
	    addSymbol(t.first().text(),symbol_flags_t::DYNAMIC());
	  }
	else
	  walkAnonymousFunction(t.first());
	break;
      }
    case TOK_GET_METHOD:
      {
	if (!_current->syms.contains(t.first().text()))
	  addSymbol(t.first().text(),symbol_flags_t::DYNAMIC());
	break;
      }
    case TOK_VARIABLE:
      {
	switch (scopeType)
	  {
	  case NestedScopeType:
	    {
	      if (!_current->syms.contains(t.first().text()))
		{
		  if (parentScopeDefines(t.first().text()))
		    {
		      addSymbol(t.first().text(),symbol_flags_t::FREE());
		      markParentSymbolCaptured(t.first().text());
		    }
		  else
		    addSymbol(t.first().text(),symbol_flags_t::DYNAMIC()); 
		}
	      break;
	    }
	  case AnonymousScopeType:
	  case NormalScopeType:
	    {
	      addSymbol(t.first().text(),symbol_flags_t::DYNAMIC());
	      break;
	    }
	  }
	walkChildren(t,scopeType);
	break;
      }
    case TOK_FOR:
      {
	addSymbol(t.first().first().text(),symbol_flags_t::DYNAMIC());
	walkChildren(t,scopeType);
	break;
      }
    case TOK_PERSISTENT:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),symbol_flags_t::PERSISTENT());
	    walkChildren(s,scopeType);
	  }
	break;
      }
    case TOK_FUNCTION:
      {
	walkFunction(t);
	break;
      }
    case TOK_NEST_FUNC:
      {
	walkFunction(t,NestedFunction);
	break;
      }
    case TOK_CLASSDEF:
      {
	walkClassDef(t);
	break;
      }
    default:
      walkChildren(t,scopeType);
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
  for (auto s=t->syms.constBegin(); s != t->syms.constEnd(); ++s)
    std::cout << spacer << "   Symbol: " << s.key() << " flags: " << s.value().str() << "\n";
  for (int i=0;i<t->children.size();++i)
    dump(t->children[i],indent+3);
}

