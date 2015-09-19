#ifndef __StringType_hpp__
#define __StringType_hpp__

#include "IntegerType.hpp"

namespace FM
{
  struct ThreadContext;

  class StringType : public IntegerType<FMChar>
  {
  public:
    StringType(ThreadContext *ctxt) : IntegerType<FMChar>(ctxt,"string") {}
    virtual DataCode code() const {return TypeString;}
    virtual Type* typeInstance() {return this;}
    inline Object makeString(const FMString &x) {
      Object p = makeMatrix(1,x.length());
      FMChar *t = rw(p);
      memcpy(t,&x[0],sizeof(FMChar)*x.length());
      return p;
    }
    Object asIndex(const Object &a, ndx_t ndx); 
    Object asIndexNoBoundsCheck(const Object &a);
    inline FMString str(const Object &o) const {
      assert(o.type()->code() == TypeString);
      const FMChar *t = ro(o);
      return FMString (t,size_t(o.dims().count()));
    }
    inline FMString describe(const Object &o) {
      if (o.dims().is2D() && (o.dims().rows() == 1))
	return FMString("'") + this->str(o) + FMString("'");
      return IntegerType<FMChar>::describe(o);
    }
    inline void computeArrayFormatInfo(FMFormatMode, const Object &, ArrayFormatInfo &format) {
      format.total_width = 1;
      format.needs_space = false;
    }
    void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx);
  };
}

#endif
