#ifndef __DoubleType_hpp__
#define __DoubleType_hpp__

#include "NumericType.hpp"
#include "BinOp.hpp"
#include "OpAdd.hpp"

namespace FM
{

  class BaseTypes;

  class DoubleType : public NumericType<double,TypeDouble> {
  public:
    DoubleType(BaseTypes* base) : NumericType<double,TypeDouble>(base,"double") {}
    virtual ~DoubleType() {}
    virtual Type* typeInstance() {return this;}
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      if (a.isScalar() && b.isScalar() && !a.isComplex() && !b.isComplex() && b.type()->code() == TypeDouble) {
	const double * ap = static_cast<const double*>(a.d->data->ptr);
	const double * bp = static_cast<const double*>(b.d->data->ptr);
	double p;
	Op::template func<double,double,double,double>(&p,ap,bp);
	return makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_binop<double,double,double,double,Op>(a,b,b.type());
	case TypeSingle:
	  return dispatch_binop<float,double,float,float,Op>(a,b,b.type());
	case TypeInt64:
	  return dispatch_binop<int64_t,double,int64_t,double,Op>(a,b,b.type());
	case TypeUInt64:
	  return dispatch_binop<uint64_t,double,uint64_t,double,Op>(a,b,b.type());
	case TypeInt32:
	  return dispatch_binop<int32_t,double,int32_t,double,Op>(a,b,b.type());
	case TypeUInt32:
	  return dispatch_binop<uint32_t,double,uint32_t,double,Op>(a,b,b.type());
	case TypeInt16:
	  return dispatch_binop<int16_t,double,int16_t,double,Op>(a,b,b.type());
	case TypeUInt16:
	  return dispatch_binop<uint16_t,double,uint16_t,double,Op>(a,b,b.type());
	case TypeInt8:
	  return dispatch_binop<int8_t,double,int8_t,double,Op>(a,b,b.type());
	case TypeUInt8:
	  return dispatch_binop<uint8_t,double,uint8_t,double,Op>(a,b,b.type());
	}
    }
    virtual Object add(const Object &a, const Object &b)
    {
      return binop<OpAdd>(a,b);
    }
  };
}

#endif
