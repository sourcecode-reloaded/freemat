#ifndef __SingleType_hpp__
#define __SingleType_hpp__

#include "PODType.hpp"
#include "BinOp.hpp"
#include "OpAdd.hpp"

namespace FM
{
  class BaseTypes;

  class SingleType : public NumericType<float,TypeSingle> {
  public:
    SingleType(BaseTypes *base) : NumericType(base,"single") {}
    virtual Type* typeInstance() {return this;}
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      if (a.isScalar() && b.isScalar() && !a.isComplex() && !b.isComplex() && b.type()->code() == TypeSingle) {
	const float * ap = static_cast<const float*>(a.d->data->ptr);
	const float * bp = static_cast<const float*>(b.d->data->ptr);
	float p;
	Op::template func<float,float,float,float>(&p,ap,bp);
	return makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_binop<float,float,double,double,Op>(a,b,a.type());
	case TypeSingle:
	  return dispatch_binop<float,float,float,float,Op>(a,b,a.type());
	default:
	  throw Exception(FMString("Unsupported combination of ") + this->name() + 
			  FMString(" and ") + b.type()->name());
	}
    }
    virtual Object add(const Object &a, const Object &b)
    {
      return binop<OpAdd>(a,b);
    }
  };
}

#endif
