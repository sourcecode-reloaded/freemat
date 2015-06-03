#ifndef __UnaryOp_hpp__
#define __UnaryOp_hpp__

#include "Object.hpp"
#include "PODType.hpp"
#include "Complex.hpp"

namespace FM
{
  template <class T, class Op>
  inline Object dispatch_unaryop(const Object &a, Type *o)
  {
    PODComplexType<T> *otype = static_cast<PODComplexType<T> *>(o);
    Object c = otype->zeroArrayOfSize(a.dims(),a.isComplex());
    T *cptr = otype->rw(c);
    const T* aptr = otype->ro(a);
    ndx_t elcnt = c.count();
    for (ndx_t i=0;i<elcnt;++i)
      Op::template func<T>(cptr+i,aptr+i);
    return c;
  }
}


#endif
