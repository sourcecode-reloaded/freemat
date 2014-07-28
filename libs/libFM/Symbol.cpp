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

void SymbolPass::walkChildren(const Tree &t) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index));
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

void SymbolPass::addSymbol(const FMString &name, int32_t flags)
{
  if (!_current->syms.contains(name))
    {
      // Adding a symbol - keep track of it's order
      flags |= (_current->syms.count()+1) << 16;
      _current->syms[name] = flags;
    }
  else
    {
      int32_t oldflags = _current->syms[name];
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

void SymbolPass::walkFunction(const Tree &t, bool nested) {
  const Tree &rets = t.child(0);
  beginFunction(t.child(1).text(),nested);
  const Tree &args = t.child(2);
  const Tree &code = t.child(3);
  for (int index=0;index < args.numChildren();index++)
    if (args.child(index).is('&'))
      addSymbol(args.child(index).first().text(), SYM_REFERENCE | SYM_PARAMETER);
    else
      addSymbol(args.child(index).text(), SYM_PARAMETER);
  for (int index=0;index < rets.numChildren();index++)
    addSymbol(rets.child(index).text(), SYM_RETURN);
  walkCode(code);  
  if (nested) popToParent();
}

void SymbolPass::walkCode(const Tree &t) {
  switch (t.token())
    {
    case TOK_GLOBAL:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_GLOBAL);
	    walkChildren(s);
	  }
	break;
      }
    case TOK_VARIABLE:
      {
	addSymbol(t.first().text(),SYM_DYNAMIC);
	walkChildren(t);
	break;
      }
    case TOK_FOR:
      {
	addSymbol(t.first().first().text(),SYM_DYNAMIC);
	walkChildren(t);
	break;
      }
    case TOK_PERSISTENT:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_PERSISTENT);
	    walkChildren(s);
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
	walkFunction(t,true);
	break;
      }
    default:
      walkChildren(t);
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
  for (FMMap<FMString, int>::const_iterator s=t->syms.constBegin(); s != t->syms.constEnd(); ++s)
    std::cout << spacer << "   Symbol: " << s.key() << " flags: " << symbolFlagsToString(s.value()) << "\n";
  for (int i=0;i<t->children.size();++i)
    dump(t->children[i],indent+3);
}

FMString FM::symbolFlagsToString(int32_t flag)
{
  FMString ret;
  int32_t position = flag >> 16;
  if (flag & SYM_GLOBAL) ret += " global";
  if (flag & SYM_PERSISTENT) ret += " persist";
  if (flag & SYM_PARAMETER) ret += " parameter";
  if (flag & SYM_REFERENCE) ret += " reference";
  if (flag & SYM_RETURN) ret += " return";
  if (flag & SYM_DYNAMIC) ret += " dynamic";
  ret += " pos=" + Stringify(position);
  return ret;
}
