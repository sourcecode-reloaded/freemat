#include "EndRemover.hpp"

using namespace FM;

EndRemoverPass::EndRemoverPass() {
}

static bool childrenHaveEnd(const Tree &t);

static bool hasEnd(const Tree &t) {
  switch (t.token()) 
    {
    case TOK_END: 
      return true;
    case TOK_VARIABLE: 
      return false;
    default:
      return childrenHaveEnd(t);
    }
  return false;
}

static bool childrenHaveEnd(const Tree &t) {
  for (int index=0;index < t.numChildren();++index)
    if (hasEnd(t.child(index))) return true;
  return false;
}

static Tree copyTree(const Tree &t) {
  Tree ret(t.node());
  for (int index=0;index<t.numChildren();++index)
    ret.addChild(copyTree(t.child(index)));
  return ret;
}


Tree EndRemoverPass::augmentEndReferences(const Tree &t, const Tree &s, const Tree &arguments) {
  // Assume that we are somewhere in the tree of a variable reference:
  //
  //  VARIABLE
  //      A
  //      ()
  //          +
  //             1
  //             VARIABLE
  //               ^------ This VARIABLE means we can no longer augment End references for
  //                       A.  So we recurse on this subtree using walkCode.
  if (t.token() == TOK_VARIABLE) return walkCode(t);

  Tree ret(t.node());

  switch (t.token())
    {
    case TOK_END:
      ret.addChild(s);
      if (arguments.valid())
	ret.addChild(arguments);
      break;
    case TOK_BRACES:
    case TOK_PARENS:
      {
	// If the TOK_PARENS has only one child, then
	// there is no need to consider END's position
	// in the argument list, i.e., A(expression(END))
	// means that END is the column mode END.
	if (t.numChildren() == 1)
	  {
	    Tree arguments = Tree(Token(TOK_INVALID));
	    ret.addChild(augmentEndReferences(t.child(0),s,arguments));
	  }
	else
	  {
	    Tree arguments = Tree(Token(TOK_PARENS));
	    for (int index=0;index<t.numChildren();++index)
	      {
		Tree x = augmentEndReferences(t.child(index),s,arguments);
		ret.addChild(copyTree(x));
		arguments.addChild(copyTree(x));
	      }
	  }
      }
      break;
    default:
      {
	for (int index=0;index < t.numChildren();++index)
	  ret.addChild(augmentEndReferences(t.child(index),s,arguments));
      }
    }
  return ret;
}

Tree EndRemoverPass::walkCode(const Tree &t) {
  Tree ret(t.node());
  bool copyChildren = true;
  switch (t.token())
    {
    case TOK_VARIABLE:
      {
	if ((t.numChildren() >= 1) && childrenHaveEnd(t))
	  {
	    Tree sub(t.node());
	    for (int index=0;index < t.numChildren();++index)
	      {
		Tree arguments = Tree(Token(TOK_PARENS));
		Tree x = augmentEndReferences(t.child(index),sub,arguments);
		ret.addChild(copyTree(x));
		sub.addChild(copyTree(x));
	      }
	    copyChildren = false;
	  }
	break;
      }      
    }
  if (copyChildren)
    for (int index=0;index < t.numChildren();++index)
      ret.addChild(walkCode(t.child(index)));
  return ret;
}
