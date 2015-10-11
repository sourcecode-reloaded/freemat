#ifndef __BoolType_hpp__
#define __BoolType_hpp__

#include "PODType.hpp"

namespace FM
{
  struct ThreadContext;

  class BoolType : public PODType<bool>
  {
  public:
    BoolType(ThreadContext *ctxt) : PODType<bool>(ctxt,"logical") {}
    virtual ~BoolType() {}
    virtual Type* typeInstance() {return this;}
    virtual void computeArrayFormatInfo(FMFormatMode, const Object &, ArrayFormatInfo &format) {
      format.total_width = 1;
    }
    virtual void printElement(const Object &a, const ArrayFormatInfo &, ndx_t ndx) {
      const bool* dp = this->ro(a);
      Type::_ctxt->_io->output("%d",dp[ndx]);
    }
    virtual DataCode code() const {return TypeBool;}
    virtual Object asIndex(const Object &a, ndx_t max);
    virtual Object asIndexNoBoundsCheck(const Object &a);
    virtual Object Not(const Object &a);
    bool any(const Object &a);
    bool all(const Object &a);
    ndx_t countOne(const Object &a);
    ndx_t countZero(const Object &a);
    virtual Object Equals(const Object &a, const Object &b);
    virtual Object NotEquals(const Object &a, const Object &b);
    virtual Object And(const Object &a, const Object &b);
    virtual Object Or(const Object &a, const Object &b);
	// FIXME - overload/redefine the comparison operators?
  };
}

#endif
