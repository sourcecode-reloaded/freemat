#ifndef __BoolType_hpp__
#define __BoolType_hpp__

#include "IntegerType.hpp"

namespace FM
{
  struct ThreadContext;

  class BoolType : public IntegerType<bool,TypeBool>
  {
  public:
    BoolType(ThreadContext *ctxt) : IntegerType<bool,TypeBool>(ctxt,"logical") {}
    virtual ~BoolType() {}
    virtual Object asIndex(const Object &a, dim_t max);
    virtual Object asIndexNoBoundsCheck(const Object &a);
    virtual Object Not(const Object &a);
    bool any(const Object &a);
    bool all(const Object &a);
    dim_t countOne(const Object &a);
    dim_t countZero(const Object &a);
  };
}

#endif
