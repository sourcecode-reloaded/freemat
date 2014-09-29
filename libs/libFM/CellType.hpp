#ifndef __CellType_hpp__
#define __CellType_hpp__

#include "ObjectArrayType.hpp"
#include "Object.hpp"

namespace FM
{
  struct ThreadContext;

  class CellType : public ObjectArrayType {
  public:
    CellType(ThreadContext *ctxt) : ObjectArrayType(ctxt,"cell") {}
    virtual DataCode code() const {return TypeCellArray;}
    virtual Type* typeInstance() {return this;}
    virtual FMString describe(const Object &a) {
      if (a.isScalar())
	return FMString("[") + scalarValue(a).description() + FMString("]");
      else
	return(a.dims().toString() + " " + this->name() + " array");
    }
    void setBraces(Object &a, const Object &args, const Object &b);
    virtual Object getBraces(const Object &a, const Object &args);
    void computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
      int maxwidth = 1;
      //const Object *t = this->readOnlyData(a);
      for (ndx_t i=0;i<a.elementCount();i++)
	maxwidth = std::max<ndx_t>(maxwidth,a.description().length());
      format.width = maxwidth+2;
    }
    void printElement(const Object &a, const ArrayFormatInfo &info, ndx_t ndx);
  };
}



#endif
