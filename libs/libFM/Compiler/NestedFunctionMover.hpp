#ifndef __NestedFunctionMover_hpp__
#define __NestedFunctionMover_hpp__

#include "Tree.hpp"

namespace FM
{
  class NestedFunctionMoverPass
  {
  public:
    NestedFunctionMoverPass() {}
    Tree walkCode(const Tree &t);
  };
};

#endif
