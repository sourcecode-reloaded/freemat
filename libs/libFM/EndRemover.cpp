#include "EndRemover.hpp"

using namespace FM;

EndRemoverPass::EndRemoverPass() {
}

bool hasEnd(const Tree &root, const Tree &t) {
  switch (t.token()) 
    {
    case TOK_END: 
      {
	std::cout << "end found for root:\n";
	root.print();
	return true;
      }
    case TOK_VARIABLE: return false;
    default:
      for (int index = 0; index < t.numChildren();++index)
	if (hasEnd(root,t.child(index))) return true;
    }
  return false;
}

void EndRemoverPass::walkChildren(const Tree &t) {
  for (int index=0;index < t.numChildren();++index)
    walkCode(t.child(index));
}

void EndRemoverPass::walkReferences(const Tree &t, const Tree &s)
{
}

void EndRemoverPass::walkCode(const Tree &t) {
  switch (t.token())
    {
    case TOK_VARIABLE:
      {
	if (t.numChildren() >= 1) {
	  std::cout << "Variable reference: " << t.first().text() << " has end: ";
	  for (int index = 0;index < t.numChildren();++index)
	    std::cout << hasEnd(t,t.child(index)) << " ";
	  std::cout << "\n";
	}
	break;
      }
    }
  walkChildren(t);
}
