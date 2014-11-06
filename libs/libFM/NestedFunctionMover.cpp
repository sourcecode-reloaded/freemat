#include "NestedFunctionMover.hpp"

using namespace FM;

static Tree copyTree(const Tree &t) {
  Tree ret(t.node());
  for (int index=0;index<t.numChildren();++index)
    ret.addChild(copyTree(t.child(index)));
  return ret;
}

Tree NestedFunctionMoverPass::walkCode(const Tree &t) {
  Tree ret(t.node());
  // Pass 1, add the children that are not nested funcs
  for (int i=0;i<t.numChildren();i++)
    if (!t.child(i).is(TOK_NEST_FUNC))
      ret.addChild(walkCode(t.child(i)));
  // Pass 2, add the children that are not nested funcs
  for (int i=0;i<t.numChildren();i++)
    if (t.child(i).is(TOK_NEST_FUNC))
      ret.addChild(walkCode(t.child(i)));
  return ret;
}
