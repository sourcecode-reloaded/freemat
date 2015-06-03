#ifndef __EndRemover_hpp__
#define __EndRemover_hpp__

#include "Tree.hpp"

namespace FM
{
  class EndRemoverPass
  {
    Tree augmentEndReferences(const Tree &t, const Tree &s, const Tree &arguments);
  public:
    EndRemoverPass();
    Tree walkCode(const Tree &t);
  };

};


#endif
