#ifndef __DoubleType_hpp__
#define __DoubleType_hpp__

#include "FloatType.hpp"
#include "BinOp.hpp"
#include "Operators.hpp"
#include "OpColon.hpp"
#include "BoolType.hpp"

namespace FM
{

  struct BaseTypes;

  class DoubleType : public FloatType<double,TypeDouble> {
  public:
    DoubleType(BaseTypes* base) : FloatType<double,TypeDouble>(base,"double") {}
    virtual ~DoubleType() {}
    virtual Type* typeInstance() {return this;}
    template <class Op>
    inline Object cmpop(const Object &a, const Object &b, BoolType *o)
    {
      if (a.isScalar() && b.isScalar() && !a.isComplex() && !b.isComplex() && b.type()->code() == TypeDouble) {
	const double * ap = static_cast<const double*>(a.d->data->ptr);
	const double * bp = static_cast<const double*>(b.d->data->ptr);
	bool p;
	Op::template func<bool,double,double,double>(&p,ap,bp);
	return o->makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_cmpop<bool,double,double,double,Op>(a,b,o);
	case TypeSingle:
	  return dispatch_cmpop<bool,double,float,float,Op>(a,b,o);
	case TypeInt64:
	  return dispatch_cmpop<bool,double,int64_t,double,Op>(a,b,o);
	case TypeUInt64:
	  return dispatch_cmpop<bool,double,uint64_t,double,Op>(a,b,o);
	case TypeInt32:
	  return dispatch_cmpop<bool,double,int32_t,double,Op>(a,b,o);
	case TypeUInt32:
	  return dispatch_cmpop<bool,double,uint32_t,double,Op>(a,b,o);
	case TypeInt16:
	  return dispatch_cmpop<bool,double,int16_t,double,Op>(a,b,o);
	case TypeUInt16:
	  return dispatch_cmpop<bool,double,uint16_t,double,Op>(a,b,o);
	case TypeInt8:
	  return dispatch_cmpop<bool,double,int8_t,double,Op>(a,b,o);
	case TypeUInt8:
	  return dispatch_cmpop<bool,double,uint8_t,double,Op>(a,b,o);
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}      
    }
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
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}
    }
    virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    virtual Object DotDivide(const Object &a, const Object &b) {return binop<OpDotDivide>(a,b);}
    virtual Object LessEquals(const Object &a, const Object &b);
    virtual Object LessThan(const Object &a, const Object &b);
    virtual Object GreaterEquals(const Object &a, const Object &b);
    virtual Object GreaterThan(const Object &a, const Object &b);
    virtual Object Equals(const Object &a, const Object &b);
    virtual Object NotEquals(const Object &a, const Object &b);    
    virtual Object Or(const Object &a, const Object &b);
    virtual Object And(const Object &a, const Object &b);
    virtual Object Colon(const Object &a, const Object &b)
    {
      if (!a.isScalar() || !b.isScalar()) throw Exception("arguments to : operator must be scalars");
      if (a.isComplex() || b.isComplex()) throw Exception("arguments to : operator must be real");
      if (b.type()->code() == TypeSingle)
	return OpColon::binary<float,TypeSingle>(this->scalarValue(a),b.asDouble(),static_cast<FloatType<float,TypeSingle>*>(b.type()));
      return OpColon::binary<double,TypeDouble>(this->scalarValue(a),b.asDouble(),static_cast<FloatType<double,TypeDouble>*>(a.type()));
    }
    virtual Object DoubleColon(const Object &a, const Object &b, const Object &c)
    {
      if (!(a.isScalar() && b.isScalar() && c.isScalar())) 
	throw Exception("arguments to : operator must be scalars");
      if (a.isComplex() || b.isComplex() || c.isComplex())
	throw Exception("arguments to : operator must be real");
      if (b.type()->code() == TypeSingle)
	return OpColon::trinary<float,TypeSingle>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<float,TypeSingle>*>(b.type()));
      if (c.type()->code() == TypeSingle)
	return OpColon::trinary<float,TypeSingle>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<float,TypeSingle>*>(c.type()));
      return OpColon::trinary<double,TypeDouble>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<double,TypeDouble>*>(a.type()));      
    }
  };
}

#endif
