#include "Symbol.hpp"
#include <iostream>
#include "Exception.hpp"

using namespace FM;

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

void SymbolPass::walkChildren(const Tree &t, bool nested) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index),nested);
}

bool SymbolPass::parentScopeDefines(const FMString &name) {
  if (_current == _root) return false;
  SymbolTable *sp = _current->parent;
  while (1) 
    {
      if (sp->syms.contains(name)) return true;
      sp = sp->parent;
      if (sp == _root) return false;
    }
  return false;
}

void SymbolPass::markParentSymbolCaptured(const FMString &name) {
  if (_current == _root) return;
  SymbolTable *sp = _current->parent;
  while (1)
    {
      if (sp->syms.contains(name) && !IS_FREE(sp->syms[name]))
	{
	  symbol_flag_t old_flag = sp->syms[name];
	  if (IS_CAPTURED(old_flag)) return;
	  old_flag |= SYM_CAPTURED;
	  old_flag &= ~SYM_DYNAMIC;
	  std::cout << "CAPTURE " << name << "\n";
	  std::cout << "  FLAG: " << symbolFlagsToString(old_flag) << "\n";
	  sp->syms[name] = old_flag;
	  return;
	}
      else
	{
	  sp->syms[name] = SYM_FREE;
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
  if (!nested)
    newSibling(t);
  else
    newChild(t);
}

void SymbolPass::addSymbol(const FMString &name, symbol_flag_t flags)
{
  if (!_current->syms.contains(name))
    {
      _current->syms[name] = flags;
      std::cout << "Insert of symbol: " << name << " with flags " << symbolFlagsToString(flags) << "\n";
    }
  else
    {
      symbol_flag_t oldflags = _current->syms[name];
      if ((oldflags | flags) != oldflags)
	{
	  std::cout << "Update of symbol: " << name 
		    << " new flags [" << symbolFlagsToString(flags) 
		    << "] old flags [" << symbolFlagsToString(oldflags) 
		    << "]\n";
	  _current->syms[name] = (oldflags | flags);
	}
    }
}

void SymbolPass::walkProperty(const Tree &t) {
  addSymbol(t.text(),SYM_PROPERTY);
  if (t.hasChildren())
    walkCode(t.first());
}

void SymbolPass::walkClassDef(const Tree &t) {
  _current->name = t.first().text();
  for (int i=1;i<t.numChildren();i++)
    {
      switch (t.child(i).token())
	{
	case TOK_PROPERTIES:
	  for (int j=0;j<t.child(i).numChildren();j++)
	    walkProperty(t.child(i).child(j));
	  break;
	case TOK_METHODS:
	  for (int j=0;j<t.child(i).numChildren();j++)
	    walkFunction(t.child(i).child(j),MethodFunction);
	  break;
	default:
	  throw Exception("Unhandled classdef block");
	}
    }
}

void SymbolPass::walkFunction(const Tree &t, FunctionTypeEnum funcType) {
  if (funcType == NestedFunction)
    {
      FMString name = t.child(1).text();
      if (_current->syms.contains(name))
	{
	  symbol_flag_t old_flag = _current->syms[name];
	  old_flag |= SYM_NESTED;
	  old_flag &= ~SYM_DYNAMIC;
	  _current->syms[name] = old_flag;
	}
      else
	addSymbol(t.child(1).text(), SYM_NESTED);
    }
  else if (funcType == MethodFunction)
    {
      FMString name = t.child(1).text();
      if (_current->syms.contains(name))
	throw Exception("Cannot redfine symbol " + name + " in class " + _current->name);
      addSymbol(t.child(1).text(),SYM_METHOD);
    }
  const Tree &rets = t.child(0);
  beginFunction(t.child(1).text(),funcType == NestedFunction);
  const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  for (int index=0;index < args.numChildren();index++)
    if (args.child(index).is('&'))
      addSymbol(args.child(index).first().text(), SYM_REFERENCE | SYM_PARAMETER | (index << 12));
    else
      addSymbol(args.child(index).text(), SYM_PARAMETER | (index << 12));
  for (int index=0;index < rets.numChildren();index++)
    addSymbol(rets.child(index).text(), SYM_RETURN | (index << 20));
  walkCode(code,funcType == NestedFunction);  
  if (funcType == NestedFunction) popToParent();
}

void SymbolPass::walkCode(const Tree &t, bool nested) {
  switch (t.token())
    {
    case TOK_GLOBAL:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_GLOBAL);
	    walkChildren(s,nested);
	  }
	break;
      }
    case TOK_VARIABLE:
      {
	if (nested)
	  {
	    if (!_current->syms.contains(t.first().text()))
	      {
		if (parentScopeDefines(t.first().text()))
		  {
		    addSymbol(t.first().text(),SYM_FREE);
		    markParentSymbolCaptured(t.first().text());
		  }
		else
		  addSymbol(t.first().text(),SYM_DYNAMIC);
	      }
	  }
	else
	  addSymbol(t.first().text(),SYM_DYNAMIC);
	walkChildren(t,nested);
	break;
      }
    case TOK_FOR:
      {
	addSymbol(t.first().first().text(),SYM_DYNAMIC);
	walkChildren(t,nested);
	break;
      }
    case TOK_PERSISTENT:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_PERSISTENT);
	    walkChildren(s,nested);
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
      walkChildren(t,nested);
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
    std::cout << spacer << "   Symbol: " << s.key() << " flags: " << symbolFlagsToString(s.value()) << "\n";
  for (int i=0;i<t->children.size();++i)
    dump(t->children[i],indent+3);
}

FMString FM::symbolFlagsToString(symbol_flag_t flag)
{
  FMString ret;
  symbol_flag_t param_position = SYM_PARAM_POSITION(flag);
  symbol_flag_t return_position = SYM_RETURN_POSITION(flag);
  if (flag & SYM_GLOBAL) ret += " global";
  if (flag & SYM_PERSISTENT) ret += " persist";
  if (flag & SYM_REFERENCE) ret += " reference";
  if (flag & SYM_DYNAMIC) ret += " dynamic";
  if (flag & SYM_PARAMETER) ret += (" parameter:" + Stringify(param_position));
  if (flag & SYM_FREE) ret += " free";
  if (flag & SYM_CAPTURED) ret += " captured";
  if (flag & SYM_NESTED) ret += " nested";
  if (flag & SYM_PROPERTY) ret += " property";
  if (flag & SYM_METHOD) ret += " method";
  if (flag & SYM_RETURN) ret += (" return:" + Stringify(return_position));
  return ret;
}
