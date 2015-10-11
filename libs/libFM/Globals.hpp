#ifndef __Globals_hpp__
#define __Globals_hpp__

#include <mutex>
#include <map>
#include "FMLib.hpp"

namespace FM
{
  struct ThreadContext;
  class Object;
  
  class Globals
  {
    std::mutex _lock;
    std::map<FMString,Object> _dict;
  public:
    void set(const FMString &name, const Object &value);
    bool isDefined(const FMString &name);
    Object get(const FMString &name, ThreadContext *ctxt);
  };
};

#endif
