#ifndef __IntegerType_hpp__
#define __IntegerType_hpp__

#include "NumericType.hpp"
#include "BinOp.hpp"
#include "UnaryOp.hpp"
#include "Operators.hpp"
#include "SaturatingInteger.hpp"

namespace FM
{

  struct ThreadContext;

  template <class T>
  class IntegerType : public NumericType<T> {
    // template <class Op>
    // inline Object binop(const Object &a, const Object &b)
    // {
    //   if (b.type()->code() == TypeDouble)
    // 	return dispatch_binop<T,double,double,Op>(a,b,this);
    //   else if (b.type()->code() == this->code())
    // 	return dispatch_binop<T,T,double,Op>(a,b,this);
    //   throw Exception("Unsupported type combination of " + a.type()->name() + " and " + b.type()->name());
    // }
  public:
    IntegerType(ThreadContext* ctxt, const FMString& name) : NumericType<T>(ctxt,name) {}
    virtual ~IntegerType() {}
    virtual Type* typeInstance() {return this;}
    //    virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    // virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    // virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    // virtual Object DotRightDivide(const Object &a, const Object &b) {return binop<OpDotRightDivide>(a,b);}
    // virtual Object DotLeftDivide(const Object &a, const Object &b) {return binop<OpDotLeftDivide>(a,b);}
    // virtual Object LessEquals(const Object &a, const Object &b);
    // virtual Object LessThan(const Object &a, const Object &b);
    // virtual Object GreaterEquals(const Object &a, const Object &b);
    // virtual Object GreaterThan(const Object &a, const Object &b);
    // virtual Object Equals(const Object &a, const Object &b);
    // virtual Object NotEquals(const Object &a, const Object &b);    
    // virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format);
    // virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
  };

  template <class T>
  class SignedIntegerType : public IntegerType<T> {
  public:
    SignedIntegerType(ThreadContext* ctxt, const FMString &name) : IntegerType<T>(ctxt,name) {}
    virtual ~SignedIntegerType() {}
    virtual Type* typeInstance() {return this;}
    // virtual Object Neg(const Object & a)
    // {
    //   return dispatch_unaryop<T,OpNeg>(a,this);
    // }
    virtual Object Hermitian(const Object &a) {return MatrixHermitian<T>(a);}    
  };

  class Int8Type : public SignedIntegerType<sint8_t>
  {
  public:
    Int8Type(ThreadContext *ctxt) : SignedIntegerType<sint8_t>(ctxt,"int8") {}
    virtual DataCode code() const {return TypeInt8;}
    virtual ~Int8Type() {}
  };

  class ComplexInt8Type : public SignedIntegerType<Complex<sint8_t> >
  {
  public:
    ComplexInt8Type(ThreadContext *ctxt) : SignedIntegerType<Complex<sint8_t> >(ctxt,"zint8") {}
    virtual DataCode code() const {return TypeZInt8;}
    virtual ~ComplexInt8Type() {}
  };

  class UInt8Type : public IntegerType<usint8_t>
  {
  public:
    UInt8Type(ThreadContext *ctxt) : IntegerType<usint8_t>(ctxt,"uint8") {}
    virtual DataCode code() const {return TypeUInt8;}
    virtual ~UInt8Type() {}
  };

  class ComplexUInt8Type : public IntegerType<Complex<usint8_t> >
  {
  public:
    ComplexUInt8Type(ThreadContext *ctxt) : IntegerType<Complex<usint8_t> >(ctxt,"zuint8") {}
    virtual DataCode code() const {return TypeZUInt8;}
    virtual ~ComplexUInt8Type() {}
  };

  class Int16Type : public SignedIntegerType<sint16_t>
  {
  public:
    Int16Type(ThreadContext *ctxt) : SignedIntegerType<sint16_t>(ctxt,"int16") {}
    virtual DataCode code() const {return TypeInt16;}
    virtual ~Int16Type() {}
  };

  class ComplexInt16Type : public SignedIntegerType<Complex<sint16_t> >
  {
  public:
    ComplexInt16Type(ThreadContext *ctxt) : SignedIntegerType<Complex<sint16_t> >(ctxt,"zint16") {}
    virtual DataCode code() const {return TypeZInt16;}
    virtual ~ComplexInt16Type() {}
  };

  class UInt16Type : public IntegerType<usint16_t>
  {
  public:
    UInt16Type(ThreadContext *ctxt) : IntegerType<usint16_t>(ctxt,"uint16") {}
    virtual DataCode code() const {return TypeUInt16;}
    virtual ~UInt16Type() {}
  };

  class ComplexUInt16Type : public IntegerType<Complex<usint16_t> >
  {
  public:
    ComplexUInt16Type(ThreadContext *ctxt) : IntegerType<Complex<usint16_t> >(ctxt,"zuint16") {}
    virtual DataCode code() const {return TypeZUInt16;}
    virtual ~ComplexUInt16Type() {}
  };

  class Int32Type : public SignedIntegerType<sint32_t>
  {
  public:
    Int32Type(ThreadContext *ctxt) : SignedIntegerType<sint32_t>(ctxt,"int32") {}
    virtual DataCode code() const {return TypeInt32;}
    virtual ~Int32Type() {}
  };

  class ComplexInt32Type : public SignedIntegerType<Complex<sint32_t> >
  {
  public:
    ComplexInt32Type(ThreadContext *ctxt) : SignedIntegerType<Complex<sint32_t> >(ctxt,"zint32") {}
    virtual DataCode code() const {return TypeZInt32;}
    virtual ~ComplexInt32Type() {}
  };

  class UInt32Type : public IntegerType<usint32_t>
  {
  public:
    UInt32Type(ThreadContext *ctxt) : IntegerType<usint32_t>(ctxt,"uint32") {}
    virtual DataCode code() const {return TypeUInt32;}
    virtual ~UInt32Type() {}
  };

  class ComplexUInt32Type : public IntegerType<Complex<usint32_t> >
  {
  public:
    ComplexUInt32Type(ThreadContext *ctxt) : IntegerType<Complex<usint32_t> >(ctxt,"zuint32") {}
    virtual DataCode code() const {return TypeZUInt32;}
    virtual ~ComplexUInt32Type() {}
  };

  class Int64Type : public SignedIntegerType<sint64_t>
  {
  public:
    Int64Type(ThreadContext *ctxt) : SignedIntegerType<sint64_t>(ctxt,"int64") {}
    virtual DataCode code() const {return TypeInt64;}
    virtual ~Int64Type() {}
  };

  class ComplexInt64Type : public SignedIntegerType<Complex<sint64_t> >
  {
  public:
    ComplexInt64Type(ThreadContext *ctxt) : SignedIntegerType<Complex<sint64_t> >(ctxt,"zint64") {}
    virtual DataCode code() const {return TypeZInt64;}
    virtual ~ComplexInt64Type() {}
  };

  class UInt64Type : public IntegerType<usint64_t>
  {
  public:
    UInt64Type(ThreadContext *ctxt) : IntegerType<usint64_t>(ctxt,"uint64") {}
    virtual DataCode code() const {return TypeUInt64;}
    virtual ~UInt64Type() {}
  };

  class ComplexUInt64Type : public IntegerType<Complex<usint64_t> >
  {
  public:
    ComplexUInt64Type(ThreadContext *ctxt) : IntegerType<Complex<usint64_t> >(ctxt,"zuint64") {}
    virtual DataCode code() const {return TypeZUInt64;}
    virtual ~ComplexUInt64Type() {}
  };

  class IndexType : public IntegerType<ndx_t>
  {
  public:
    IndexType(ThreadContext *ctxt) : IntegerType<ndx_t>(ctxt,"index") {}
    virtual ~IndexType() {}
    virtual DataCode code() const {return TypeIndex;}
    bool isColon(const Object &a) {
      return (a.isScalar() && this->scalarValue(a) == -1);
    }
    Object expandColons(ndx_t len) {
      Object output = this->makeMatrix(len,1);
      ndx_t *op = this->rw(output);
      for (ndx_t i=0;i<len;i++)
	op[i] = i;
      return output;
    }
  };
};

#endif
