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

void SymbolPass::walkChildren(const Tree &t, int blockDepth) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index),blockDepth);
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

void SymbolPass::addSymbol(const FMString &name, int32_t flags, int blockDepth)
{
  if (!_current->syms.contains(name))
    {
      // If the first time a symbol is used, it is undeclared, it is automatically dynamic
      if (flags & SYM_USED) flags |= SYM_DYNAMIC;
      std::cout << "Add new symbol: " << name << " flags [" 
		<< symbolFlagsToString(flags) << "] must run: " 
		<< blockDepth << "\n";
      // Do not allow localdef if we aren't at a blockDepth of 2
      if ((flags & SYM_LOCAL_DEF) && (blockDepth > 1))
	  flags = (flags ^ SYM_LOCAL_DEF) | SYM_DYNAMIC;
      // Adding a symbol - keep track of it's order
      flags |= (_current->syms.count()+1) << 16;
      _current->syms[name] = flags;
    }
  else
    {
      int32_t oldflags = _current->syms[name];
      if (IS_EXPLICIT_DECLARED(oldflags) && (flags & SYM_LOCAL_DEF)) return;
      if ((oldflags & SYM_USED) && 
	  (!(oldflags & SYM_LOCAL_DEF)) &&
	  (flags & SYM_LOCAL_DEF)) flags = SYM_DYNAMIC;
      if ((oldflags | flags) != oldflags)
	{
	  std::cout << "Update of symbol: " << name 
		    << " new flags [" << symbolFlagsToString(flags) 
		    << "] old flags [" << symbolFlagsToString(oldflags) 
		    << "] must run: " << blockDepth << "\n";
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
      addSymbol(args.child(index).first().text(), SYM_REFERENCE | SYM_PARAMETER, 0);
    else
      addSymbol(args.child(index).text(), SYM_PARAMETER, 0);
  for (int index=0;index < rets.numChildren();index++)
    addSymbol(rets.child(index).text(), SYM_RETURN, 0);
  walkCode(code, 0);  
  if (nested) popToParent();
}

void SymbolPass::walkCode(const Tree &t, int blockDepth) {
  std::cout << "Walking Token: " << TokenToString(t.token()) << "\n";
  switch (t.token())
    {
    case '=':
      {
	walkCode(t.second(),blockDepth);
	const Tree &lhs = t.first();
	addSymbol(lhs.first().text(),SYM_LOCAL_DEF,blockDepth);
	//	for (int index=1;index < lhs.first().numChildren();++index) 
	//	  walkCode(lhs.first().child(index),blockDepth);
	walkChildren(t.first(), blockDepth);
	break;
      }
    case TOK_GLOBAL:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_GLOBAL,blockDepth);
	    walkChildren(s, blockDepth);
	  }
	break;
      }
    case TOK_VARIABLE:
      {
	addSymbol(t.first().text(),SYM_USED,blockDepth);
	walkChildren(t, blockDepth);
	break;
      }
    case TOK_FOR:
      {
	addSymbol(t.first().first().text(),SYM_LOCAL_DEF,blockDepth);
	walkCode(t.first().second(),blockDepth);
	walkCode(t.second(),false);
	break;
      }
    case TOK_PERSISTENT:
      {
	for (int index=0;index < t.numChildren();++index)
	  {
	    const Tree &s = t.child(index);
	    addSymbol(s.text(),SYM_PERSISTENT,blockDepth);
	    walkChildren(s,blockDepth);
	  }
	break;
      }
    case TOK_MULTI:
      {
	const Tree &lhs = t.first();
	for (int index=0;index < lhs.numChildren();++index)
	  {
	    addSymbol(lhs.child(index).first().text(),SYM_LOCAL_DEF,blockDepth);
	    walkCode(lhs.child(index),blockDepth);
	  }
	const Tree &rhs = t.second();
	walkCode(rhs,blockDepth);
	break;
      }
    case TOK_FUNCTION_DEFS:
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
    case TOK_BLOCK:
      {
	// Blocks (in general) do not have to run
	std::cout << "Block encountered: depth increased to " << blockDepth+1 << "\n";
	walkChildren(t,blockDepth+1);
	break;
      }
    default:
      walkChildren(t,blockDepth);
    }
}

bool SymbolPass::childNeedsSymbol(SymbolTable *t, const FMString &symname)
{
  std::cout << "Checking if " << t->name << " needs symbol " << symname << "\n";
  if (!t->syms.contains(symname))
    {
      for (int i=0;i<t->children.size();++i)
	if (childNeedsSymbol(t->children[i],symname)) return true;
      return false;
    }
  if (IS_EXPLICIT_DECLARED(t->syms[symname])) return false;
  t->syms[symname] |= SYM_FREE;
  return true;
}

void SymbolPass::markCaptured(SymbolTable *t)
{
  for (FMMap<FMString,int>::iterator i=t->syms.begin();i!=t->syms.end();++i)
    {
      for (int j=0;j<t->children.size();++j)
	if (childNeedsSymbol(t->children[j],i.key()))
	  {
	    std::cout << "Child " << t->name << " needs symbol " << i.key() << "\n";
	    i.value() = (i.value() | SYM_CAPTURED) & (~SYM_LOCAL_DEF);
	  }
    }
  for (int i=0;i<t->children.size();++i)
    markCaptured(t->children[i]);
}

void SymbolPass::findCaptured()
{
  // The root doesn't allow for captured variables.  All other functions do
  for (int i=0;i<_root->children.size();++i)
    {
      markCaptured(_root->children[i]);
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
  if (flag & SYM_LOCAL_DEF) ret += " localdef";
  if (flag & SYM_PARAMETER) ret += " parameter";
  if (flag & SYM_USED) ret += " used";
  if (flag & SYM_SCALAR) ret += " scalar";
  if (flag & SYM_REFERENCE) ret += " reference";
  if (flag & SYM_RETURN) ret += " return";
  if (flag & SYM_FREE) ret += " free";
  if (flag & SYM_CAPTURED) ret += " captured";
  if (flag & SYM_DYNAMIC) ret += " dynamic";
  ret += " pos=" + Stringify(position);
  return ret;
}
