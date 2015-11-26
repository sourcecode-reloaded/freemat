#ifndef __SingleType_hpp__
#define __SingleType_hpp__

#include "FloatType.hpp"
#include "BoolType.hpp"
#include "BinOp.hpp"
#include "Operators.hpp"
#include "MatrixOps.hpp"

namespace FM
{
  struct ThreadContext;

  class ComplexSingleType;
  
  class SingleType : public FloatType<float> {
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeSingle) {
	const float * ap = static_cast<const float*>(a.d->data->ptr);
	const float * bp = static_cast<const float*>(b.d->data->ptr);
	float p;
	Op::template func<float,float,float,float>(&p,*ap,*bp);
	return makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_binop<float,double,double,Op>(a,b,_ctxt->_single);
	case TypeZDouble:
	  return dispatch_binop<float,cdouble,double,Op>(a,b,_ctxt->_zsingle);
	case TypeSingle:
	  return dispatch_binop<float,float,float,Op>(a,b,_ctxt->_single);
	case TypeZSingle:
	  return dispatch_binop<float,cfloat,float,Op>(a,b,_ctxt->_zsingle);
	default:
	  throw Exception(FMString("Unsupported combination of ") + this->name() + 
			  FMString(" and ") + b.type()->name());
	}
    }
    template <class Op>
    inline Object cmpop(const Object &a, const Object &b, BoolType *o)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeSingle) {
	const float * ap = static_cast<const float*>(a.d->data->ptr);
	const float * bp = static_cast<const float*>(b.d->data->ptr);
	bool p;
	Op::template func<bool,float,float,float>(&p,*ap,*bp);
	return o->makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_cmpop<float,double,double,Op>(a,b,_ctxt->_bool);
	case TypeSingle:
	  return dispatch_cmpop<float,float,float,Op>(a,b,_ctxt->_bool);
	default:
	  throw Exception(FMString("Unsupported combination of ") + this->name() + 
			  FMString(" and ") + b.type()->name());
	}
    }
  public:
    SingleType(ThreadContext *ctxt) : FloatType<float>(ctxt,"single") {}
    virtual DataCode code() const {return TypeSingle;}
    virtual Type* typeInstance() {return this;}
    //    virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    virtual Object DotPower(const Object &a, const Object &b) {
      //FIXME - handle complex promotion case
      //      if (!a.isComplex() && !b.isComplex() && !this->isNonNegative(a) && !this->isIntegerValued(b)) 
      //	return binop<OpDotPower>(this->asComplex(a),this->asComplex(b));
      return binop<OpDotPower>(a,b);
    }
    virtual Object Power(const Object &a, const Object &b) {return MatrixPower(a,b,_ctxt);}
    virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    virtual Object DotRightDivide(const Object &a, const Object &b) {return binop<OpDotRightDivide>(a,b);}
    virtual Object DotLeftDivide(const Object &a, const Object &b) {return binop<OpDotLeftDivide>(a,b);}
    virtual Object LessEquals(const Object &a, const Object &b) {return cmpop<OpLE>(a,b,_ctxt->_bool);}
    virtual Object LessThan(const Object &a, const Object &b) {return cmpop<OpLT>(a,b,_ctxt->_bool);}
    virtual Object GreaterEquals(const Object &a, const Object &b) {return cmpop<OpGE>(a,b,_ctxt->_bool);}
    virtual Object GreaterThan(const Object &a, const Object &b) {return cmpop<OpGT>(a,b,_ctxt->_bool);}
    virtual Object Equals(const Object &a, const Object &b) {return cmpop<OpEQ>(a,b,_ctxt->_bool);}
    virtual Object NotEquals(const Object &a, const Object &b) {return cmpop<OpNE>(a,b,_ctxt->_bool);}
    virtual Object Or(const Object &a, const Object &b) {return cmpop<OpOr>(a,b,_ctxt->_bool);}
    virtual Object And(const Object &a, const Object &b) {return cmpop<OpAnd>(a,b,_ctxt->_bool);}
    SingleType *realType() {
      return this;
    }
    ComplexSingleType *complexType() {
      return _ctxt->_zsingle;
    }
  };

  
  
  class ComplexSingleType : public FloatType<cfloat > {
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeZSingle) {
	const cfloat * ap = static_cast<const cfloat*>(a.d->data->ptr);
	const cfloat * bp = static_cast<const cfloat*>(b.d->data->ptr);
	cfloat p;
	Op::template func<cfloat,cfloat,cfloat,cfloat >(&p,*ap,*bp);
	return makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_binop<cfloat,double,double,Op>(a,b,_ctxt->_zsingle);
	case TypeZDouble:
	  return dispatch_binop<cfloat,cdouble,double,Op>(a,b,_ctxt->_zsingle);
	case TypeSingle:
	  return dispatch_binop<cfloat,float,float,Op>(a,b,_ctxt->_zsingle);
	case TypeZSingle:
	  return dispatch_binop<cfloat,cfloat,float,Op>(a,b,_ctxt->_zsingle);
	default:
	  throw Exception(FMString("Unsupported combination of ") + this->name() + 
			  FMString(" and ") + b.type()->name());	  
	}
    }
    template <class Op>
    inline Object cmpop(const Object &a, const Object &b, BoolType *o)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == TypeZSingle) {
	const cfloat * ap = static_cast<const cfloat*>(a.d->data->ptr);
	const cfloat * bp = static_cast<const cfloat*>(b.d->data->ptr);
	bool p;
	Op::template func<bool,cfloat,cfloat,cfloat>(&p,*ap,*bp);
	return o->makeScalar(p);
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_cmpop<cfloat,double,double,Op>(a,b,_ctxt->_bool);
	case TypeZDouble:
	  return dispatch_cmpop<cfloat,cdouble,double,Op>(a,b,_ctxt->_bool);
	case TypeSingle:
	  return dispatch_cmpop<cfloat,float,float,Op>(a,b,_ctxt->_bool);
	case TypeZSingle:
	  return dispatch_cmpop<cfloat,cfloat,float,Op>(a,b,_ctxt->_bool);
	default:
	  throw Exception(FMString("Unsupported combination of ") + this->name() + 
			  FMString(" and ") + b.type()->name());
	}
    }
  public:
    ComplexSingleType(ThreadContext* ctxt) : FloatType<cfloat>(ctxt,"zsingle") {}
    virtual DataCode code() const {return TypeZSingle;}
    virtual ~ComplexSingleType() {}
    virtual Type* typeInstance() {return this;}
    SingleType* realType() {
      return Type::_ctxt->_single;
    }
    ComplexSingleType* complexType() {
      return this;
    }
    virtual bool isComplexType() const {return true;}
    //    virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    virtual Object DotPower(const Object &a, const Object &b) {return binop<OpDotPower>(a,b);}
    virtual Object Power(const Object &a, const Object &b) {return MatrixPower(a,b,_ctxt);}
    virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    virtual Object DotRightDivide(const Object &a, const Object &b) {return binop<OpDotRightDivide>(a,b);}
    virtual Object DotLeftDivide(const Object &a, const Object &b) {return binop<OpDotLeftDivide>(a,b);}
    virtual Object LessEquals(const Object &a, const Object &b) {return cmpop<OpLE>(a,b,_ctxt->_bool);}
    // virtual Object LessThan(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpLT>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object GreaterEquals(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpGE>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object GreaterThan(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpGT>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object Equals(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpEQ>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object NotEquals(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpNE>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object Or(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpOr>(a,b,_ctxt->_bool,Type::_ctxt,this);}
    // virtual Object And(const Object &a, const Object &b) {return dispatch_complex_cmpop<OpAnd>(a,b,_ctxt->_bool,Type::_ctxt,this);}
  };

}

#endif
