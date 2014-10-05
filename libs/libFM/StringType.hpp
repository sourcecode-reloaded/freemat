#ifndef __StringType_hpp__
#define __StringType_hpp__

#include "IntegerType.hpp"

namespace FM
{
  struct ThreadContext;

  class StringType : public IntegerType<FMChar,TypeString>
  {
  public:
    StringType(ThreadContext *ctxt) : IntegerType<FMChar,TypeString>(ctxt,"string") {}
    virtual DataCode code() const {return TypeString;}
    virtual Type* typeInstance() {return this;}
    inline Object makeString(const FMString &x) {
      Object p = makeMatrix(1,x.length());
      FMChar *t = readWriteData(p);
      memcpy(t,&x[0],sizeof(FMChar)*x.length());
      return p;
    }
    Object asIndex(const Object &a, dim_t ndx); 
    Object asIndexNoBoundsCheck(const Object &a);
    inline FMString getString(const Object &o) const {
      assert(o.type()->code() == TypeString);
      const FMChar *t = readOnlyData(o);
      FMString ret;
      for (dim_t i=0;i<o.dims().elementCount();++i)
	ret += t[i];
      return ret;
    }
    inline FMString describe(const Object &o) {
      if (o.dims().is2D() && (o.dims().rows() == 1))
	return FMString("'") + this->getString(o) + FMString("'");
      return IntegerType<FMChar,TypeString>::describe(o);
    }
    inline void computeArrayFormatInfo(FMFormatMode, const Object &, ArrayFormatInfo &format) {
      format.width = 1;
    }
    void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx);
  };
}

#endif
