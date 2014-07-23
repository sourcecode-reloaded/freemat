#ifndef __FloatType_hpp__
#define __FloatType_hpp__

#include "NumericType.hpp"

namespace FM
{
  
  struct BaseTypes;

  template <class T, FM::DataCode codeNum>
  class FloatType : public NumericType<T,codeNum> {
  public:
    FloatType(BaseTypes* base, const FMString &name) : NumericType<T,codeNum>(base,name) {}
    virtual ~FloatType() {}
    virtual Type* typeInstance() {return this;}
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info);
    virtual void printElement(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t offset);
  };
};


#endif
