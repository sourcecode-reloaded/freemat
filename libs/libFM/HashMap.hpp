#ifndef __HashMap__
#define __HashMap__

#include <boost/unordered_map.hpp>
#include "Object.hpp"

namespace FM
{
  struct FMHashMap_hash : std::unary_function<FM::Object,std::size_t>
  {
    std::size_t operator()(const Object &x) const;
  };

  template <class T>
  class HashMap : public boost::unordered_map<FM::Object,T,FMHashMap_hash> 
  {
  public:
    HashMap() : boost::unordered_map<FM::Object,T,FMHashMap_hash>() {}
  };
};

#endif
