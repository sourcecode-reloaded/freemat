#ifndef __EndRemover_hpp__
#define __EndRemover_hpp__

#include "Tree.hpp"

namespace FM
{
  class EndRemoverPass
  {
    void walkChildren(const Tree &t);
    void walkReferences(const Tree &t, const Tree &s);
  public:
    EndRemoverPass();
    void walkCode(const Tree &t);
  };

};


#endif
