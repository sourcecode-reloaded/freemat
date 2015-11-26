#ifndef __DoubleType_hpp__
#define __DoubleType_hpp__

#include "FloatType.hpp"
#include "BinOp.hpp"
#include "Operators.hpp"
#include "OpColon.hpp"
#include "BoolType.hpp"
#include "MatrixOps.hpp"
#include "SingleType.hpp"
#include "IntegerType.hpp"

namespace FM
{

  struct ThreadContext;

  class ComplexDoubleType;
  
  class DoubleType : public FloatType<double> {
    template <class Op>
    inline Object cmpop(const Object &a, const Object &b, BoolType *o)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeDouble) {
	const double * ap = this->ro(a);
	const double * bp = this->ro(b);
	bool p;
	Op::template func<bool,double,double,double>(&p,*ap,*bp);
	return o->makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_cmpop<double,double,double,Op>(a,b,o);
	case TypeSingle:
	  return dispatch_cmpop<double,float,float,Op>(a,b,o);
	case TypeInt64:
	  return dispatch_cmpop<double,int64_t,double,Op>(a,b,o);
	case TypeUInt64:
	  return dispatch_cmpop<double,uint64_t,double,Op>(a,b,o);
	case TypeInt32:
	  return dispatch_cmpop<double,int32_t,double,Op>(a,b,o);
	case TypeUInt32:
	  return dispatch_cmpop<double,uint32_t,double,Op>(a,b,o);
	case TypeInt16:
	  return dispatch_cmpop<double,int16_t,double,Op>(a,b,o);
	case TypeUInt16:
	  return dispatch_cmpop<double,uint16_t,double,Op>(a,b,o);
	case TypeInt8:
	  return dispatch_cmpop<double,int8_t,double,Op>(a,b,o);
	case TypeUInt8:
	  return dispatch_cmpop<double,uint8_t,double,Op>(a,b,o);
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}      
    }
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeDouble) {
	const double * ap = this->ro(a);
	const double * bp = this->ro(b);
	double p;
	Op::template func<double,double,double,double>(&p,*ap,*bp);
	return makeScalar(p);
      }
      switch (b.type()->code())
	{
	  // This seems very verbose - can it be replaced somehow?
 	case TypeDouble:
	  return dispatch_binop<double,double,double,Op>(a,b,_ctxt->_double);
	case TypeZDouble:
	  return dispatch_binop<double,cdouble,double,Op>(a,b,_ctxt->_zdouble);
	case TypeSingle:
	  return dispatch_binop<double,float,float,Op>(a,b,_ctxt->_single);
	case TypeZSingle:
	  return dispatch_binop<double,Complex<float>,double,Op>(a,b,_ctxt->_zsingle);
	case TypeInt64:
	  return dispatch_binop<double,int64_t,long double,Op>(a,b,_ctxt->t_int64);
	case TypeZInt64:
	  return dispatch_binop<double,Complex<int64_t>,long double,Op>(a,b,_ctxt->t_zint64);
	case TypeUInt64:
	  return dispatch_binop<double,uint64_t,long double,Op>(a,b,_ctxt->_uint64);
	case TypeZUInt64:
	  return dispatch_binop<double,Complex<uint64_t>,long double,Op>(a,b,_ctxt->_zuint64); 
	case TypeInt32:
	  return dispatch_binop<double,int32_t,double,Op>(a,b,_ctxt->t_int32);
	case TypeZInt32:
	  return dispatch_binop<double,Complex<int32_t>,double,Op>(a,b,_ctxt->t_zint32);
	case TypeUInt32:
	  return dispatch_binop<double,uint32_t,double,Op>(a,b,_ctxt->_uint32);
	case TypeZUInt32:
	  return dispatch_binop<double,Complex<uint32_t>,double,Op>(a,b,_ctxt->_zuint32);
	case TypeInt16:
	  return dispatch_binop<double,int16_t,double,Op>(a,b,_ctxt->t_int16);
	case TypeZInt16:
	  return dispatch_binop<double,Complex<int16_t>,double,Op>(a,b,_ctxt->t_zint16);
	case TypeUInt16:
	  return dispatch_binop<double,uint16_t,double,Op>(a,b,_ctxt->_uint16);
	case TypeZUInt16:
	  return dispatch_binop<double,Complex<uint16_t>,double,Op>(a,b,_ctxt->_zuint16);
	case TypeInt8:
	  return dispatch_binop<double,int8_t,double,Op>(a,b,_ctxt->t_int8);
	case TypeZInt8:
	  return dispatch_binop<double,Complex<int8_t>,double,Op>(a,b,_ctxt->t_zint8);
	case TypeUInt8:
	  return dispatch_binop<double,uint8_t,double,Op>(a,b,_ctxt->_uint8);
	case TypeZUInt8:
	  return dispatch_binop<double,Complex<uint8_t>,double,Op>(a,b,_ctxt->_zuint8);
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}
    }
  public:
    DoubleType(ThreadContext* ctxt) : FloatType<double>(ctxt,"double") {}
    virtual ~DoubleType() {}
    virtual DataCode code() const {return TypeDouble;}
    virtual Type* typeInstance() {return this;}
    // virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    // virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    // virtual Object DotPower(const Object &a, const Object &b) {
    //   // FIXME - dotpower can change types.
    //   //      if (!this->isNonNegative(a) && !this->isIntegerValued(b)) 
    //   //	return binop<OpDotPower>(this->asComplex(a),this->asComplex(b));
    //   return binop<OpDotPower>(a,b);
    // }
    // virtual Object Power(const Object &a, const Object &b) {
    //   return MatrixPower(a,b,_ctxt);
    // }
    // virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    // virtual Object DotRightDivide(const Object &a, const Object &b) {return binop<OpDotRightDivide>(a,b);}
    // virtual Object DotLeftDivide(const Object &a, const Object &b) {return binop<OpDotLeftDivide>(a,b);}
    // virtual Object LessEquals(const Object &a, const Object &b) {return cmpop<OpLE>(a,b,_ctxt->_bool);}
    // virtual Object LessThan(const Object &a, const Object &b) {return cmpop<OpLT>(a,b,_ctxt->_bool);}
    // virtual Object GreaterEquals(const Object &a, const Object &b) {return cmpop<OpGE>(a,b,_ctxt->_bool);}
    // virtual Object GreaterThan(const Object &a, const Object &b) {return cmpop<OpGT>(a,b,_ctxt->_bool);}
    // virtual Object Equals(const Object &a, const Object &b) {return cmpop<OpEQ>(a,b,_ctxt->_bool);}
    // virtual Object NotEquals(const Object &a, const Object &b) {return cmpop<OpNE>(a,b,_ctxt->_bool);}
    // virtual Object Or(const Object &a, const Object &b) {return cmpop<OpOr>(a,b,_ctxt->_bool);}
    // virtual Object And(const Object &a, const Object &b) {return cmpop<OpAnd>(a,b,_ctxt->_bool);}
    virtual Object Colon(const Object &a, const Object &b)
    {
      if (!a.isScalar() || !b.isScalar()) throw Exception("arguments to : operator must be scalars");
      if (a.isComplex() || b.isComplex()) throw Exception("arguments to : operator must be real");
      if (b.type()->code() == TypeSingle)
	return OpColon::binary<float>(this->scalarValue(a),b.asDouble(),static_cast<FloatType<float>*>(b.type()));
      return OpColon::binary<double>(this->scalarValue(a),b.asDouble(),static_cast<FloatType<double>*>(a.type()));
    }
    virtual Object DoubleColon(const Object &a, const Object &b, const Object &c)
    {
      if (!(a.isScalar() && b.isScalar() && c.isScalar())) 
	throw Exception("arguments to : operator must be scalars");
      if (a.isComplex() || b.isComplex() || c.isComplex())
       	throw Exception("arguments to : operator must be real");
      if (b.type()->code() == TypeSingle)
	return OpColon::trinary<float>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<float>*>(b.type()));
      if (c.type()->code() == TypeSingle)
	return OpColon::trinary<float>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<float>*>(c.type()));
      return OpColon::trinary<double>(a.asDouble(),b.asDouble(),c.asDouble(),static_cast<FloatType<double>*>(a.type()));      
    }
    DoubleType * realType() {
      return this;
    }
    ComplexDoubleType *complexType() {
      return _ctxt->_zdouble;
    }
  };


  class ComplexDoubleType : public FloatType<cdouble> {
  public:
    ComplexDoubleType(ThreadContext* ctxt) : FloatType<cdouble >(ctxt,"zdouble") {}
    virtual ~ComplexDoubleType() {}
    virtual Type* typeInstance() {return this;}
    virtual DataCode code() const {return TypeZDouble;}
    DoubleType * realType() {
      return Type::_ctxt->_double;
    }
    ComplexDoubleType* complexType() {
      return this;
    }
    virtual bool isComplexType() const {return true;}
    virtual Object Add(const Object &a, const Object &b) {return dispatch_complex_binop<OpAdd>(a,b,Type::_ctxt,this);}
    virtual Object DotMultiply(const Object &a, const Object &b) {return dispatch_complex_binop<OpDotMultiply>(a,b,Type::_ctxt,this);}
  };

}

#endif
