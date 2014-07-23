#ifndef __StringType_hpp__
#define __StringType_hpp__

#include "IntegerType.hpp"

namespace FM
{
  struct BaseTypes;

  class StringType : public IntegerType<FMChar,TypeString>
  {
  public:
    StringType(BaseTypes *base) : IntegerType<FMChar,TypeString>(base,"string") {}
    virtual DataCode code() const {return TypeString;}
    virtual Type* typeInstance() {return this;}
    Object makeString(const FMString &x) {
      Object p = makeMatrix(1,x.length());
      FMChar *t = readWriteData(p);
      memcpy(t,&x[0],sizeof(FMChar)*x.length());
      return p;
    }
    FMString getString(const Object &o) const {
      assert(o.type()->code() == TypeString);
      const FMChar *t = readOnlyData(o);
      FMString ret;
      for (dim_t i=0;i<o.dims().elementCount();++i)
	ret += t[i];
      return ret;
    }
    FMString describe(const Object &o) {
      if (o.dims().is2D() && (o.dims().rows() == 1))
	return FMString("'") + this->getString(o) + FMString("'");
      return IntegerType<FMChar,TypeString>::describe(o);
    }
    void computeArrayFormatInfo(FMFormatMode, const Object &, ArrayFormatInfo &format) {
      format.width = 1;
    }
    void printElement(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t ndx) {
      const FMChar *t = this->readOnlyData(a);
      io.output("%c",t[ndx]);
    }
  };
}

#endif
