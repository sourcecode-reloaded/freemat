#ifndef __SingleType_hpp__
#define __SingleType_hpp__

#include "FloatType.hpp"
#include "BinOp.hpp"
#include "Operators.hpp"

namespace FM
{
  struct ThreadContext;

  class SingleType : public FloatType<float,TypeSingle> {
  public:
    SingleType(ThreadContext *ctxt) : FloatType<float,TypeSingle>(ctxt,"single") {}
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
    virtual Object Add(const Object &a, const Object &b)
    {
      return binop<OpAdd>(a,b);
    }
    virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    virtual Object Multiply(const Object &a, const Object &b);
  };
}

#endif
