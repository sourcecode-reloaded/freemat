#ifndef __NumericType_hpp__
#define __NumericType_hpp__

#include "PODType.hpp"
#include "Hermitian.hpp"
#include "Transpose.hpp"
#include "SaturatingInteger.hpp"

namespace FM
{

  struct ThreadContext;

  template <typename T>
  struct is_complex{
    static const bool value = false;
  };

  template <typename T>
  struct is_complex<Complex<T> >{
    static const bool value = true;
  };
  
  template <class T>
  class NumericType : public PODType<T> {
  public:
    NumericType(ThreadContext *ctxt, const FMString &name) : PODType<T>(ctxt,name) {}
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
    template <class btype, class Func>
    inline Object disp_binop(const Object &a, const Object &b, Func op)
    {
      unsigned aincr = (a.isScalar() ? 0 : 1);
      unsigned bincr = (b.isScalar() ? 0 : 1);
      auto typeobj_for_b = reinterpret_cast<NumericType<btype>*>(b.type());
      const T* ap = this->ro(a);
      const btype* bp = typeobj_for_b->ro(b);
      // Check for the complex promotion case, i.e., a is real, and b is complex.
      // Then the output is complex
      if ((!is_complex<T>::value) && (is_complex<btype>::value)) {
	NumericType<Complex<T> > *comp = static_cast<NumericType<Complex<T> >*>(ThreadContext::GetComplexTypeForCode(Type::_ctxt,this->code()));
	Object c = comp->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()));
	Complex<T>* cptr = comp->rw(c);
	for (ndx_t i=0;i<c.count();i++)
	  cptr[i] = op(ap[i*aincr],bp[i*bincr]);
	return c;
      } else {
	Object c = this->zeroArrayOfSize(Tuple::computeDotOpSize(a.dims(),b.dims()));
	T* cptr = this->rw(c);
	for (ndx_t i=0;i<c.count();i++)
	  cptr[i] = op(ap[i*aincr],bp[i*bincr]);
	return c;
      }
    }
    template <class Func>
    inline Object do_binop(const Object &a, const Object &b, Func op)
    {
      if (a.isScalar() && b.isScalar() && b.type()->code() == this->code()) {
	const T* ap = this->ro(a);
	const T* bp = this->ro(b);
	return this->makeScalar(op(*ap,*bp));
      }
      switch (b.type()->code())
	{
	case TypeDouble:
	  return disp_binop<double>(a,b,op);
	case TypeZDouble:
	  return disp_binop<cdouble>(a,b,op);
	case TypeSingle:
	  return disp_binop<float>(a,b,op);
	case TypeZSingle:
	  return disp_binop<Complex<float> >(a,b,op);
	case TypeInt64:
	  return disp_binop<sint64_t>(a,b,op);
	case TypeZInt64:
	  return disp_binop<Complex<sint64_t> >(a,b,op);
	case TypeUInt64:
	  return disp_binop<usint64_t>(a,b,op);
	case TypeZUInt64:
	  return disp_binop<Complex<usint64_t> >(a,b,op); 
	case TypeInt32:
	  return disp_binop<sint32_t>(a,b,op);
	case TypeZInt32:
	  return disp_binop<Complex<sint32_t> >(a,b,op);
	case TypeUInt32:
	  return disp_binop<usint32_t>(a,b,op);
	case TypeZUInt32:
	  return disp_binop<Complex<usint32_t> >(a,b,op);
	case TypeInt16:
	  return disp_binop<sint16_t>(a,b,op);
	case TypeZInt16:
	  return disp_binop<Complex<sint16_t> >(a,b,op);
	case TypeUInt16:
	  return disp_binop<usint16_t>(a,b,op);
	case TypeZUInt16:
	  return disp_binop<Complex<usint16_t> >(a,b,op);
	case TypeInt8:
	  return disp_binop<sint8_t>(a,b,op);
	case TypeZInt8:
	  return disp_binop<Complex<sint8_t> >(a,b,op);
	case TypeUInt8:
	  return disp_binop<usint8_t>(a,b,op);
	case TypeZUInt8:
	  return disp_binop<Complex<usint8_t> >(a,b,op);
	default:
	  throw Exception("Unhanded type arguments to binary operator");
	}
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
    virtual Object getBraces(const Object &, const Object &) {throw Exception("{} indexing unsupported for numeric arrays");}
    virtual Object getField(const Object &, const Object &) {throw Exception(". indexing unsupported for numeric arrays");}
    bool anyNonzeroImaginary(const Object &a);
    bool isNonNegative(const Object &a);
    bool isIntegerValued(const Object &b);
    Object asDiagonalMatrix(const Object &a);
    Object getDiagonal(const Object &a);
    Object realPart(const Object &a);
    Object imagPart(const Object &a);
    void fill(Object &a, T val);
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
  };
}


#endif
