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
    PODType<T> *otype = static_cast<PODType<T> *>(o);
    Object c = otype->zeroArrayOfSize(a.dims(),a.isComplex());
    T *cptr = otype->readWriteData(c);
    const T* aptr = otype->readOnlyData(a);
    size_t elcnt = c.elementCount();
    for (size_t i=0;i<elcnt;++i)
      Op::template func<T>(cptr+i,aptr+i);
    return c;
  }
}


#endif
