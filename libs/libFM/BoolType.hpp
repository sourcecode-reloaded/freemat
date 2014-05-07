#ifndef __BoolType_hpp__
#define __BoolType_hpp__

#include "IntegerType.hpp"

namespace FM
{
  class BoolType : public IntegerType<bool,TypeBool>
  {
  public:
    BoolType(BaseTypes *base) : IntegerType<bool,TypeBool>(base,"bool") {}
    virtual ~BoolType() {}
    virtual Object asIndex(const Object &a, dim_t max);
  };
}

#endif
