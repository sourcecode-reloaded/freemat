#ifndef __FloatType_hpp__
#define __FloatType_hpp__

#include "NumericType.hpp"
#include "Operators.hpp"
#include "UnaryOp.hpp"

namespace FM
{
  
  struct ThreadContext;

  template <class T, FM::DataCode codeNum>
  class FloatType : public NumericType<T,codeNum> {
  public:
    FloatType(ThreadContext* ctxt, const FMString &name) : NumericType<T,codeNum>(ctxt,name) {}
    virtual ~FloatType() {}
    virtual Type* typeInstance() {return this;}
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
    virtual Object Neg(const Object &a) {return FM::dispatch_unaryop<T,OpNeg>(a,this);};
  };
};


#endif
