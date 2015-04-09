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
    size_t aincr = (a.isScalar() ? 0 : 1);
    size_t bincr = (b.isScalar() ? 0 : 1);
    bool isComplex = (a.isComplex() || b.isComplex());
    PODType<ctype> *opod = reinterpret_cast<PODType<ctype> *>(o);
	PODType<atype> *apod = reinterpret_cast<PODType<atype> *>(a.type());
	PODType<btype> *bpod = reinterpret_cast<PODType<btype> *>(b.type());
    Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()),isComplex);
    size_t elcnt = c.count();    
    if (!a.isComplex() && !b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const btype *bptr = bpod->ro(b);
      ctype *cptr = opod->rw(c);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (a.isComplex() && !b.isComplex()) {
      const Complex<atype> *aptr = apod->roComplex(a);
      const btype *bptr = bpod->ro(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,Complex<atype>,btype,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (!a.isComplex() && b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,atype,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else {
      const Complex<atype> *aptr = apod->roComplex(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      Complex<ctype> *cptr = opod->rwComplex(c);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    }
    return c;
  }

  template <class ctype, class atype, class btype, class vtype, class Op>
  static inline Object dispatch_cmpop(const Object &a, const Object &b, Type *otype)
  {
    size_t aincr = (a.isScalar() ? 0 : 1);
    size_t bincr = (b.isScalar() ? 0 : 1);
	PODType<ctype> *opod = reinterpret_cast<PODType<ctype> *>(otype);
	PODType<atype> *apod = reinterpret_cast<PODType<atype> *>(a.type());
	PODType<btype> *bpod = reinterpret_cast<PODType<btype> *>(b.type());
	Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(), b.dims()),RealCase);
    ctype *cptr = opod->rw(c);
    size_t elcnt = c.count();    
    if (!a.isComplex() && !b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const btype *bptr = bpod->ro(b);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (a.isComplex() && !b.isComplex()) {
      const Complex<atype> *aptr = apod->roComplex(a);
      const btype *bptr = bpod->ro(b);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<ctype,Complex<atype>,btype,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else if (!a.isComplex() && b.isComplex()) {
      const atype *aptr = apod->ro(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<ctype,atype,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    } else {
      const Complex<atype> *aptr = apod->roComplex(a);
      const Complex<btype> *bptr = bpod->roComplex(b);
      for (size_t i=0;i<elcnt;++i)
	Op::template func<ctype,Complex<atype>,Complex<btype>,Complex<vtype> >(cptr+i,aptr+i*aincr,bptr+i*bincr);
    }
    return c;
  }

}
#endif
