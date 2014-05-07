#ifndef __ListType_hpp__
#define __ListType_hpp__

#include "PODType.hpp"
#include "Object.hpp"

namespace FM
{
  class BaseTypes;

  class ListType : public PODType<Object> {
  public:
    ListType(BaseTypes *base) : PODType<Object>(base,"list",true) {}
    virtual DataCode code() const {return TypeListArray;}
    virtual Type* typeInstance() {return this;}
    virtual FMString describe(const Object &a) {
      return (a.dims().toString() + " " + this->name() + " list");
    }
    virtual Object asIndex(const Object &, dim_t) {
      throw Exception("Lists cannot be used as index arrays");
    }
  };
};



#endif
