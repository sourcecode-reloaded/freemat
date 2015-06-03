#ifndef __BinOp_hpp__
#define __BinOp_hpp__

#include "Object.hpp"
#include "PODType.hpp"
#include "Complex.hpp"

namespace FM
{

  template <class ctype, class atype, class btype, class vtype, class Op>
  static inline Object dispatch_binop(const Object &a, const Object &b, Type *o)
  {
    unsigned aincr = (a.isScalar() ? 0 : 1);
    unsigned bincr = (b.isScalar() ? 0 : 1);
    bool isComplex = (a.isComplex() || b.isComplex());
    PODComplexType<ctype> *opod = reinterpret_cast<PODComplexType<ctype> *>(o);
	PODComplexType<atype> *apod = reinterpret_cast<PODComplexType<atype> *>(a.type());
	PODComplexType<btype> *bpod = reinterpret_cast<PODComplexType<btype> *>(b.type());
    Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()),isComplex);
    ndx_t elcnt = c.count();    
    if (!a.isComplex() && !b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const btype *bptr = bpod->ro(b);
      ctype *cptr = opod->rw(c);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (a.isComplex() && !b.isComplex()) {
      const Complex<atype> *aptr = apod->roComplex(a);
      const btype *bptr = bpod->ro(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,Complex<atype>,btype,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (!a.isComplex() && b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,atype,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else {
      const Complex<atype> *aptr = apod->roComplex(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    }
    return c;
  }

  template <class ctype, class atype, class btype, class vtype, class Op>
  static inline Object dispatch_cmpop(const Object &a, const Object &b, Type *otype)
  {
    unsigned aincr = (a.isScalar() ? 0 : 1);
    unsigned bincr = (b.isScalar() ? 0 : 1);
	PODComplexType<ctype> *opod = reinterpret_cast<PODComplexType<ctype> *>(otype);
	PODComplexType<atype> *apod = reinterpret_cast<PODComplexType<atype> *>(a.type());
	PODComplexType<btype> *bpod = reinterpret_cast<PODComplexType<btype> *>(b.type());
	Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(), b.dims()),RealCase);
    ctype *cptr = opod->rw(c);
    ndx_t elcnt = c.count();    
    if (!a.isComplex() && !b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const btype *bptr = bpod->ro(b);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (a.isComplex() && !b.isComplex()) {
      const Complex<atype> *aptr = apod->roComplex(a);
      const btype *bptr = bpod->ro(b);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<ctype,Complex<atype>,btype,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (!a.isComplex() && b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else {
      const Complex<atype> *aptr = apod->roComplex(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      for (ndx_t i=0;i<elcnt;++i)
	Op::template func<ctype,Complex<atype>,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    }
    return c;
  }

}
#endif
