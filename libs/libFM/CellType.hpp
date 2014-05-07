#ifndef __CellType_hpp__
#define __CellType_hpp__

#include "PODType.hpp"
#include "Object.hpp"

namespace FM
{
  class BaseTypes;

  class CellType : public PODType<Object> {
  public:
    CellType(BaseTypes *base) : PODType<Object>(base,"cell") {}
    virtual DataCode code() const {return TypeCellArray;}
    virtual Type* typeInstance() {return this;}
    virtual Object add(const Object &a, const Object &b) {throw Exception("Adding cell arrays is unsupported");}
    virtual FMString describe(const Object &a) {
      if (a.isScalar())
	return FMString("[") + scalarValue(a).description() + FMString("]");
      else
	return(a.dims().toString() + " " + this->name() + " array");
    }
    virtual Object asIndex(const Object &, dim_t) {
      throw Exception("Cell arrays cannot be used as index arrays");
    }
  };
}



#endif
