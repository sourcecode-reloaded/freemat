#ifndef __BinOp_hpp__
#define __BinOp_hpp__

#include "Object.hpp"
#include "PODType.hpp"
#include "Complex.hpp"
#include "Operators.hpp"
#include "FloatType.hpp"

namespace FM
{

  template <class atype, class btype, class vtype, class Op, class ctype>
  static inline Object dispatch_binop(const Object &a, const Object &b, PODType<ctype> *opod)
  {
    unsigned aincr = (a.isScalar() ? 0 : 1);
    unsigned bincr = (b.isScalar() ? 0 : 1);
    PODType<atype> *apod = reinterpret_cast<PODType<atype> *>(a.type());
    PODType<btype> *bpod = reinterpret_cast<PODType<btype> *>(b.type());
    Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()));
    ndx_t elcnt = c.count();    
    const atype *aptr = apod->ro(a);
    const btype *bptr = bpod->ro(b);
    ctype *cptr = opod->rw(c);
    for (ndx_t i=0;i<elcnt;++i)
      OpComplexWrapper<Op,ctype,atype,btype,vtype>::func(cptr+i,aptr[i*aincr],bptr[i*bincr]);
    return c;
  }

  template <class atype, class btype, class vtype, class Op>
  static inline Object dispatch_cmpop(const Object &a, const Object &b, PODType<bool> *opod)
  {
    unsigned aincr = (a.isScalar() ? 0 : 1);
    unsigned bincr = (b.isScalar() ? 0 : 1);
    PODType<atype> *apod = reinterpret_cast<PODType<atype> *>(a.type());
    PODType<btype> *bpod = reinterpret_cast<PODType<btype> *>(b.type());
    Object c = opod->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(), b.dims()));
    bool *cptr = opod->rw(c);
    ndx_t elcnt = c.count();    
    const atype *aptr = apod->ro(a);
    const btype *bptr = bpod->ro(b);
    for (ndx_t i=0;i<elcnt;++i)
      OpComplexWrapper<Op,bool,atype,btype,vtype>::func(cptr+i,aptr[i*aincr],bptr[i*bincr]);
    return c;
  }

  template <class Op, class T>
  static inline Object dispatch_complex_binop(const Object &a, const Object &b, ThreadContext *_ctxt, FloatType<Complex<T> > *type)
  {
    if (a.isScalar() && b.isScalar() && b.type()->code() == type->code()) {
      const Complex<T> * ap = static_cast<const Complex<T>*>(a.raw()->ptr());
      const Complex<T> * bp = static_cast<const Complex<T>*>(b.raw()->ptr());
      Complex<T> p;
      Op::template func<Complex<T>,Complex<T>,Complex<T>,Complex<T> >(&p,*ap,*bp);
      return type->makeScalar(p);
    }
    switch (b.type()->code())
      {
      case TypeDouble:
	return dispatch_binop<Complex<T>,double,double,Op>(a,b,type);
      case TypeZDouble:
	return dispatch_binop<Complex<T>,Complex<double>,double,Op>(a,b,type);
      case TypeSingle:
	return dispatch_binop<Complex<T>,float,float,Op>(a,b,_ctxt->_zsingle);
      case TypeZSingle:
	return dispatch_binop<Complex<T>,Complex<float>,float,Op>(a,b,_ctxt->_zsingle);
      default:
	throw Exception(FMString("Unsupported combination of ") + a.type()->name() + 
			FMString(" and ") + b.type()->name());	  
      }
  }
}

#endif
