#ifndef __Graph_hpp__
#define __Graph_hpp__

#include <list>

namespace FM
{
  struct GraphNode
  {
    FMString name;
    std::list<GraphNode*> deps;    
  };

  struct Graph
  {
    std::vector<GraphNode> nodes;
  };

}
#endif
