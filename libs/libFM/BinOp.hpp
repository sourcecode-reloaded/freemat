#ifndef __BinOp_hpp__
#define __BinOp_hpp__

#include "Object.hpp"
#include "PODType.hpp"
#include "Complex.hpp"

namespace FM
{

  template <class ctype, class atype, class btype, class vtype, class Op>
  inline Object dispatch_binop_lev2(const Object &a, const Object &b, Type *o, bool isComplex)
  {
    PODType<ctype,false> *otype = static_cast<PODType<ctype,false> *>(o);
    Object c = otype->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()),isComplex);
    ctype *cptr = otype->readWriteData(c);
    const atype *aptr = static_cast<PODType<atype,false>*>(a.type())->readOnlyData(a);
    const btype *bptr = static_cast<PODType<btype,false>*>(b.type())->readOnlyData(b);
    size_t aincr = (a.isScalar() ? 0 : 1);
    size_t bincr = (b.isScalar() ? 0 : 1);
    size_t elcnt = c.elementCount();
    for (size_t i=0;i<elcnt;++i)
      Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
    return c;
  }

  template <class ctype, class atype, class btype, class vtype, class Op>
  static inline Object dispatch_binop(const Object &a, const Object &b, Type *otype)
  {
    if (!a.isComplex() && !b.isComplex())
      return dispatch_binop_lev2<ctype,atype,btype,vtype,Op>(a,b,otype,RealCase);
    else if (a.isComplex() && !b.isComplex())
      return dispatch_binop_lev2<Complex<ctype>,Complex<atype>,btype,Complex<vtype>,Op>(a,b,otype,ComplexCase);
    else if (!a.isComplex() && b.isComplex())
      return dispatch_binop_lev2<Complex<ctype>,atype,Complex<btype>,Complex<vtype>,Op>(a,b,otype,ComplexCase);
    else
      return dispatch_binop_lev2<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype>,Op>(a,b,otype,ComplexCase);
  }

}
#endif
