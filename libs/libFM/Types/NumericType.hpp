#ifndef __NumericType_hpp__
#define __NumericType_hpp__

#include "PODType.hpp"
#include "Hermitian.hpp"
#include "Transpose.hpp"
#include "SaturatingInteger.hpp"
#include "TypeCompute.hpp"

namespace FM
{

  struct ThreadContext;

  // TODO - fix scalar/scalar case
 
  // Map type code to type
  template <class T>
  struct datacode_type;

  template <>
  struct datacode_type<bool>
  {
    static PODType<bool>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<bool>*>(tc->_bool);}
  };

  template <>
  struct datacode_type<double>
  {
    static PODType<double>* Type(ThreadContext  *tc) {return reinterpret_cast<PODType<double>*>(tc->_double);}
  };

  template <>
  struct datacode_type<float>
  {
    static PODType<float>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<float>*>(tc->_single);}
  };

  template <>
  struct datacode_type<sint8_t>
  {
    static PODType<sint8_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<sint8_t>*>(tc->t_int8);}
  };

  template <>
  struct datacode_type<sint16_t>
  {
    static PODType<sint16_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<sint16_t>*>(tc->t_int16);}
  };

  template <>
  struct datacode_type<sint32_t>
  {
    static PODType<sint32_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<sint32_t>*>(tc->t_int32);}
  };

  template <>
  struct datacode_type<sint64_t>
  {
    static PODType<sint64_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<sint64_t>*>(tc->t_int64);}
  };

  template <>
  struct datacode_type<usint8_t>
  {
    static PODType<usint8_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<usint8_t>*>(tc->_uint8);}
  };

  template <>
  struct datacode_type<usint16_t>
  {
    static PODType<usint16_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<usint16_t>*>(tc->_uint16);}
  };

  template <>
  struct datacode_type<usint32_t>
  {
    static PODType<usint32_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<usint32_t>*>(tc->_uint32);}
  };

  template <>
  struct datacode_type<usint64_t>
  {
    static PODType<usint64_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<usint64_t>*>(tc->_uint64);}
  };
  
  template <>
  struct datacode_type<Complex<double> >
  {
    static PODType<Complex<double> >* Type(ThreadContext  *tc) {return reinterpret_cast<PODType<Complex<double> >*>(tc->_zdouble);}
  };

  template <>
  struct datacode_type<Complex<float> >
  {
    static PODType<Complex<float> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<float> >*>(tc->_zsingle);}
  };

  template <>
  struct datacode_type<Complex<sint8_t> >
  {
    static PODType<Complex<sint8_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<sint8_t> >*>(tc->t_zint8);}
  };

  template <>
  struct datacode_type<Complex<sint16_t> >
  {
    static PODType<Complex<sint16_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<sint16_t> >*>(tc->t_zint16);}
  };

  template <>
  struct datacode_type<Complex<sint32_t> >
  {
    static PODType<Complex<sint32_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<sint32_t> >*>(tc->t_zint32);}
  };

  template <>
  struct datacode_type<Complex<sint64_t> >
  {
    static PODType<Complex<sint64_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<sint64_t> >*>(tc->t_zint64);}
  };

  template <>
  struct datacode_type<Complex<usint8_t> >
  {
    static PODType<Complex<usint8_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<usint8_t> >*>(tc->_zuint8);}
  };

  template <>
  struct datacode_type<Complex<usint16_t> >
  {
    static PODType<Complex<usint16_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<usint16_t> >*>(tc->_zuint16);}
  };

  template <>
  struct datacode_type<Complex<usint32_t> >
  {
    static PODType<Complex<usint32_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<usint32_t> >*>(tc->_zuint32);}
  };

  template <>
  struct datacode_type<Complex<usint64_t> >
  {
    static PODType<Complex<usint64_t> >* Type(ThreadContext *tc) {return reinterpret_cast<PODType<Complex<usint64_t> >*>(tc->_zuint64);}
  };

  template<>
  struct datacode_type<FMChar>
  {
    static PODType<FMChar>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<FMChar>*>(tc->_string);}
  };

  template<>
  struct datacode_type<ndx_t>
  {
    static PODType<ndx_t>* Type(ThreadContext *tc) {return reinterpret_cast<PODType<ndx_t>*>(tc->_index);}
  };

  template <class T>
  struct type_printer
  {
    static void printit() {
      std::cout << "Typename: " << __PRETTY_FUNCTION__ << "\n";
    }
  };

  // Generic binary op dispatch with resolved types
  template <bool enabled, class atype, class btype, class vtype, class ctype, class Func>
  struct disp_binop_helper_resolved {
    static inline Object dispatch(const Object &a, const Object &b, Func op);
  };

  template <class atype, class btype, class vtype, class ctype, class Func>
  struct disp_binop_helper_resolved<false,atype,btype,vtype,ctype,Func> {
    static inline Object dispatch(const Object &a, const Object &b, Func op) {
      throw Exception("Operation is unsupported with arguments of type "+
		      a.type()->name() + " and " + b.type()->name());      
    }
  };
  
  template <class atype, class btype, class vtype, class ctype, class Func>
  struct disp_binop_helper_resolved<true,atype,btype,vtype,ctype,Func> {
    static inline Object dispatch(const Object &a, const Object &b, Func op) {
      unsigned aincr = (a.isScalar() ? 0 : 1);
      unsigned bincr = (b.isScalar() ? 0 : 1);
      auto typeobj_for_b = reinterpret_cast<PODType<btype>*>(b.type());
      auto typeobj_for_a = reinterpret_cast<PODType<atype>*>(a.type());
      auto typeobj_for_c = datacode_type<ctype>::Type(a.context());
      auto ap = typeobj_for_a->ro(a);
      auto bp = typeobj_for_b->ro(b);
      Object c = typeobj_for_c->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()));
      auto cptr = typeobj_for_c->rw(c);
      for (ndx_t i=0;i<c.count();i++)
	cptr[i] = ctype(op(vtype(ap[i*aincr]),vtype(bp[i*bincr])));
      return c;
    }
  };

  enum op_type {
    ArithmeticOp = 0,
    CompareOp = 1,
    OrderOp = 2
  };
  
  template <class atype, class btype, class Func, op_type OpType>
  struct disp_helper;

  template <class atype, class btype, class Func>
  struct disp_helper<atype,btype,Func,ArithmeticOp> {
    static inline Object dispatch(const Object &a, const Object &b, Func op)
    {
      using crt = compute_result_type<atype,btype>;
      typedef typename crt::result_type ctypedef;
      typedef typename crt::via_type vtypedef;
      return disp_binop_helper_resolved<crt::is_valid,atype,btype,vtypedef,ctypedef,Func>::dispatch(a,b,op);
    }    
  };

  template <class atype, class btype, class Func>
  struct disp_helper<atype,btype,Func,CompareOp> {
    static inline Object dispatch(const Object &a, const Object &b, Func op)
    {
      using crt = compare_result_type<atype,btype>;
      typedef typename crt::via_type vtypedef;
      return disp_binop_helper_resolved<crt::is_valid,atype,btype,vtypedef,bool,Func>::dispatch(a,b,op);
    }
  };

  template <class atype, class btype, class Func>
  struct disp_helper<atype,btype,Func,OrderOp> {
    static inline Object dispatch(const Object &a, const Object &b, Func op)
    {
      using crt = compute_result_type<atype,btype>;
      typedef typename crt::via_type vtypedef;
      return disp_binop_helper_resolved<crt::is_valid,atype,btype,vtypedef,bool,Func>::dispatch(a,b,op);
    }    
  };

    
  template <class T, class Func, op_type OpType>
  struct btype_dispatch {
    static inline Object dispatch(const Object &a, const Object &b, Func op)
    {
      switch (b.type()->code())
	{
	case TypeDouble:
	  return disp_helper<T,double,Func,OpType>::dispatch(a,b,op);
	case TypeSingle:
	  return disp_helper<T,float,Func,OpType>::dispatch(a,b,op);
	case TypeInt64:
	  return disp_helper<T,sint64_t,Func,OpType>::dispatch(a,b,op);
	case TypeUInt64:
	  return disp_helper<T,usint64_t,Func,OpType>::dispatch(a,b,op);
	case TypeInt32:
	  return disp_helper<T,sint32_t,Func,OpType>::dispatch(a,b,op);
	case TypeUInt32:
	  return disp_helper<T,usint32_t,Func,OpType>::dispatch(a,b,op);
	case TypeInt16:
	  return disp_helper<T,sint16_t,Func,OpType>::dispatch(a,b,op);
	case TypeUInt16:
	  return disp_helper<T,usint16_t,Func,OpType>::dispatch(a,b,op);
	case TypeInt8:
	  return disp_helper<T,sint8_t,Func,OpType>::dispatch(a,b,op);
	case TypeUInt8:
	  return disp_helper<T,usint8_t,Func,OpType>::dispatch(a,b,op);
	case TypeZDouble:
	  return disp_helper<T,Complex<double>,Func,OpType>::dispatch(a,b,op);
	case TypeZSingle:
	  return disp_helper<T,Complex<float>,Func,OpType>::dispatch(a,b,op);
	case TypeZInt64:
	  return disp_helper<T,Complex<sint64_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZUInt64:
	  return disp_helper<T,Complex<usint64_t>,Func,OpType>::dispatch(a,b,op); 
	case TypeZInt32:
	  return disp_helper<T,Complex<sint32_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZUInt32:
	  return disp_helper<T,Complex<usint32_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZInt16:
	  return disp_helper<T,Complex<sint16_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZUInt16:
	  return disp_helper<T,Complex<usint16_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZInt8:
	  return disp_helper<T,Complex<sint8_t>,Func,OpType>::dispatch(a,b,op);
	case TypeZUInt8:
	  return disp_helper<T,Complex<usint8_t>,Func,OpType>::dispatch(a,b,op);
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}
    }
  };

  template <class T>
  class OrderedType : public PODType<T> {
  public:
    OrderedType(ThreadContext *ctxt, const FMString &name) : PODType<T>(ctxt,name) {}
    virtual ~OrderedType() {}
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
    virtual T minValue(const Object &a)
    {
      if (a.isEmpty()) throw Exception("Empty arguments do not support minValue");
      const T* ap = this->ro(a);
      ndx_t acnt = a.count();
      T ret = ap[0];
      for (ndx_t i=1;i<acnt;i++)
	ret = (ap[i] < ret) ? ap[i] : ret;
      return ret;
    }
    virtual T maxValue(const Object &a) 
    {
      if (a.isEmpty()) throw Exception("Empty arguments do not support maxValue");
      const T* ap = this->ro(a);
      ndx_t acnt = a.count();
      T ret = ap[0];
      for (ndx_t i=1;i<acnt;i++)
	ret = (ap[i] > ret) ? ap[i] : ret;
      return ret;
    }    
  };
  
  template <class T>
  class NumericType : public OrderedType<T> {
  public:
    NumericType(ThreadContext *ctxt, const FMString &name) : OrderedType<T>(ctxt,name) {}
    virtual ~NumericType() {}
    virtual Object asLogical(const Object &a);
    virtual Object asIndex(const Object &a, ndx_t max);
    virtual Object asIndexNoBoundsCheck(const Object &a);
    virtual double doubleValue(const Object &a) {
      return static_cast<double>(this->scalarValue(a));
    }
    inline Type* complexType() {
      return ThreadContext::GetComplexTypeForCode(Type::_ctxt,this->code());      
    }
    virtual Object Plus(const Object &a) {
      return a;
    }
    template <class Func>
    inline Object do_ordop(const Object &a, const Object &b, Func op)
    {
      auto logical = reinterpret_cast<PODType<bool>*>(this->context()->_bool);
      if (a.isScalar() && b.isScalar() && b.type()->code() == this->code()) {
	const T* ap = this->ro(a);
	const T* bp = this->ro(b);
	return logical->makeScalar(op(*ap,*bp));
      }
      return btype_dispatch<T,Func,OrderOp>::dispatch(a,b,op);
    }
    template <class Func>
    inline Object do_cmpop(const Object &a, const Object &b, Func op)
    {
      auto logical = reinterpret_cast<PODType<bool>*>(this->context()->_bool);
      if (a.isScalar() && b.isScalar() && b.type()->code() == this->code()) {
	const T* ap = this->ro(a);
	const T* bp = this->ro(b);
	return logical->makeScalar(op(*ap,*bp));
      }
      return btype_dispatch<T,Func,CompareOp>::dispatch(a,b,op);
    }
    template <class Func>
    inline Object do_binop(const Object &a, const Object &b, Func op)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == this->code()) {
	const T* ap = this->ro(a);
	const T* bp = this->ro(b);
	using crt = compute_result_type<T,T>;
	typedef typename crt::via_type vtypedef;
	if (!crt::is_valid)
	  throw Exception("Unsupported operation for operands of type " + a.type()->name());
	return this->makeScalar(T(op(vtypedef(*ap),vtypedef(*bp))));
      }
      return btype_dispatch<T,Func,ArithmeticOp>::dispatch(a,b,op);
    }
    template <class S>
    void convertLoop(const Object &a, T* op, ndx_t len)
    {
      auto a_type = static_cast<NumericType<S>*>(a.type());
      const S* ip = a_type->ro(a);
      for (ndx_t i=0;i<len;i++)
	op[i] = static_cast<T>(ip[i]);
    }
    Object convert(const Object &a)
    {
      if (this->code() == a.type()->code()) return a;
      Object ret = this->zeroArrayOfSize(a.dims());
      ndx_t len = a.dims().count();
      T* op = this->rw(ret);
      switch (a.type()->code())
	{
	case TypeString:
	  convertLoop<FMChar>(a,op,len);
	  break;
	case TypeSingle:
	  convertLoop<float>(a,op,len);
	  break;
	case TypeDouble:
	  convertLoop<double>(a,op,len);
	  break;
	case TypeInt8:
	  convertLoop<sint8_t>(a,op,len);
	  break;
	case TypeUInt8:
	  convertLoop<usint8_t>(a,op,len);
	  break;
	case TypeInt16:
	  convertLoop<sint16_t>(a,op,len);
	  break;
	case TypeUInt16:
	  convertLoop<usint16_t>(a,op,len);
	  break;
	case TypeInt32:
	  convertLoop<sint32_t>(a,op,len);
	  break;
	case TypeUInt32:
	  convertLoop<usint32_t>(a,op,len);
	  break;
	case TypeInt64:
	  convertLoop<sint64_t>(a,op,len);
	  break;
	case TypeUInt64:
	  convertLoop<usint64_t>(a,op,len);
	  break;
	case TypeZSingle:
	  convertLoop<Complex<float> >(a,op,len);
	  break;
	case TypeZDouble:
	  convertLoop<Complex<double> >(a,op,len);
	  break;
	case TypeZInt8:
	  convertLoop<Complex<sint8_t> >(a,op,len);
	  break;
	case TypeZUInt8:
	  convertLoop<Complex<usint8_t> >(a,op,len);
	  break;
	case TypeZInt16:
	  convertLoop<Complex<sint16_t> >(a,op,len);
	  break;
	case TypeZUInt16:
	  convertLoop<Complex<usint16_t> >(a,op,len);
	  break;
	case TypeZInt32:
	  convertLoop<Complex<sint32_t> >(a,op,len);
	  break;
	case TypeZUInt32:
	  convertLoop<Complex<usint32_t> >(a,op,len);
	  break;
	case TypeZInt64:
	  convertLoop<Complex<sint64_t> >(a,op,len);
	  break;
	case TypeZUInt64:
	  convertLoop<Complex<usint64_t> >(a,op,len);
	  break;
	default:
	  throw Exception("Type conversion from " + a.type()->name() + " to " + this->name() + " is unsupported.");
	}
      return ret;
    }
    virtual Object Add(const Object &a, const Object &b) {return do_binop(a,b,[](auto x, auto y){return x + y;});}
    virtual Object Subtract(const Object &a, const Object &b) {return do_binop(a,b,[](auto x, auto y) {return x - y;});}
    virtual Object DotMultiply(const Object &a, const Object &b) {return do_binop(a,b,[](auto x, auto y) {return x * y;});}
    virtual Object DotRightDivide(const Object &a, const Object &b) {return do_binop(a,b,[](auto x, auto y) {return x / y;});}
    virtual Object DotLeftDivide(const Object &a, const Object &b) {return do_binop(a,b,[](auto x, auto y) {return y / x;});}
    virtual Object getBraces(const Object &, const Object &) {throw Exception("{} indexing unsupported for numeric arrays");}
    virtual Object getField(const Object &, const Object &) {throw Exception(". indexing unsupported for numeric arrays");}
    virtual Object LessEquals(const Object &a, const Object &b) {return do_ordop(a,b,[](auto x, auto y) {return x <= y;});}
    virtual Object LessThan(const Object &a, const Object &b) {return do_ordop(a,b,[](auto x, auto y) {return x < y;});}
    virtual Object GreaterThan(const Object &a, const Object &b) {return do_ordop(a,b,[](auto x, auto y) {return x > y;});}
    virtual Object GreaterEquals(const Object &a, const Object &b) {return do_ordop(a,b,[](auto x, auto y) {return x >= y;});}
    virtual Object NotEquals(const Object &a, const Object &b) {return do_cmpop(a,b,[](auto x, auto y) {return x != y;});}
    virtual Object Equals(const Object &a, const Object &b) {return do_cmpop(a,b,[](auto x, auto y) {return x == y;});}
    bool anyNonzeroImaginary(const Object &a);
    bool isNonNegative(const Object &a);
    bool isIntegerValued(const Object &b);
    Object asDiagonalMatrix(const Object &a);
    Object getDiagonal(const Object &a);
    Object realPart(const Object &a);
    Object imagPart(const Object &a);
    void fill(Object &a, T val);
  };
}


#endif
