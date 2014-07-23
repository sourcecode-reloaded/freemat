#ifndef __CellType_hpp__
#define __CellType_hpp__

#include "PODType.hpp"
#include "Object.hpp"

namespace FM
{
  struct BaseTypes;

  class CellType : public PODType<Object,true> {
  public:
    CellType(BaseTypes *base) : PODType<Object,true>(base,"cell") {}
    virtual DataCode code() const {return TypeCellArray;}
    virtual Type* typeInstance() {return this;}
    virtual Object add(const Object &a, const Object &b) {throw Exception("Adding cell arrays is unsupported");}
    virtual FMString describe(const Object &a) {
      if (a.isScalar())
	return FMString("[") + scalarValue(a).description() + FMString("]");
      else
	return(a.dims().toString() + " " + this->name() + " array");
    }
    void setBraces(Object &a, const Object &args, const Object &b) {
      setParens(a,args,this->makeScalar(b));
    }
    void computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
      int maxwidth = 1;
      //const Object *t = this->readOnlyData(a);
      for (ndx_t i=0;i<a.elementCount();i++)
	maxwidth = std::max<ndx_t>(maxwidth,a.description().length());
      format.width = maxwidth+2;
    }
    void printElement(const Object &a, TermIF &o, const ArrayFormatInfo &info, ndx_t ndx) {
      const Object *t = this->readOnlyData(a);
      o.output("[" + t[ndx].description() + "]");
    }
  };
}



#endif
