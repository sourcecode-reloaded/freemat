#ifndef __IntegerType_hpp__
#define __IntegerType_hpp__

#include "NumericType.hpp"
#include "BinOp.hpp"
#include "UnaryOp.hpp"
#include "Operators.hpp"

namespace FM
{

  struct ThreadContext;

  template <class T, FM::DataCode codeNum>
  class IntegerType : public NumericType<T,codeNum> {
    template <class Op>
    inline Object binop(const Object &a, const Object &b)
    {
      switch (b.type()->code())
	{
	case TypeDouble:
	  return dispatch_binop<T,T,double,double,Op>(a,b,a.type());
	case codeNum:
	  return dispatch_binop<T,T,T,double,Op>(a,b,a.type());
	default:
	  throw Exception("Unsupported type combination of " + a.type()->name() + " and " + b.type()->name());
	}
    }
  public:
    IntegerType(ThreadContext* ctxt, const FMString& name) : NumericType<T,codeNum>(ctxt,name) {}
    virtual ~IntegerType() {}
    virtual Type* typeInstance() {return this;}
    virtual Object Add(const Object &a, const Object &b) {return binop<OpAdd>(a,b);}
    virtual Object Subtract(const Object &a, const Object &b) {return binop<OpSubtract>(a,b);}
    virtual Object DotMultiply(const Object &a, const Object &b) {return binop<OpDotMultiply>(a,b);}
    virtual Object DotRightDivide(const Object &a, const Object &b) {return binop<OpDotRightDivide>(a,b);}
    virtual Object DotLeftDivide(const Object &a, const Object &b) {return binop<OpDotLeftDivide>(a,b);}
    virtual Object LessEquals(const Object &a, const Object &b);
    virtual Object LessThan(const Object &a, const Object &b);
    virtual Object GreaterEquals(const Object &a, const Object &b);
    virtual Object GreaterThan(const Object &a, const Object &b);
    virtual Object Equals(const Object &a, const Object &b);
    virtual Object NotEquals(const Object &a, const Object &b);    
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
  };

  template <class T, FM::DataCode codeNum>
  class SignedIntegerType : public IntegerType<T,codeNum> {
  public:
    SignedIntegerType(ThreadContext* ctxt, const FMString &name) : IntegerType<T,codeNum>(ctxt,name) {}
    virtual ~SignedIntegerType() {}
    virtual Type* typeInstance() {return this;}
    virtual Object Neg(const Object & a)
    {
      return dispatch_unaryop<T,OpNeg>(a,this);
    }
    virtual Object Hermitian(const Object &a) {return MatrixHermitian<T>(a);}    
  };

  class Int8Type : public SignedIntegerType<int8_t,TypeInt8>
  {
  public:
    Int8Type(ThreadContext *ctxt) : SignedIntegerType<int8_t,TypeInt8>(ctxt,"int8") {}
    virtual ~Int8Type() {}
  };

  class UInt8Type : public IntegerType<uint8_t,TypeUInt8>
  {
  public:
    UInt8Type(ThreadContext *ctxt) : IntegerType<uint8_t,TypeUInt8>(ctxt,"uint8") {}
    virtual ~UInt8Type() {}
  };

  class Int16Type : public SignedIntegerType<int16_t,TypeInt16>
  {
  public:
    Int16Type(ThreadContext *ctxt) : SignedIntegerType<int16_t,TypeInt16>(ctxt,"int16") {}
    virtual ~Int16Type() {}
  };

  class UInt16Type : public IntegerType<uint16_t,TypeUInt16>
  {
  public:
    UInt16Type(ThreadContext *ctxt) : IntegerType<uint16_t,TypeUInt16>(ctxt,"uint16") {}
    virtual ~UInt16Type() {}
  };

  class Int32Type : public SignedIntegerType<int32_t,TypeInt32>
  {
  public:
    Int32Type(ThreadContext *ctxt) : SignedIntegerType<int32_t,TypeInt32>(ctxt,"int32") {}
    virtual ~Int32Type() {}
  };

  class UInt32Type : public IntegerType<uint32_t,TypeUInt32>
  {
  public:
    UInt32Type(ThreadContext *ctxt) : IntegerType<uint32_t,TypeUInt32>(ctxt,"uint32") {}
    virtual ~UInt32Type() {}
  };

  class Int64Type : public SignedIntegerType<int64_t,TypeInt64>
  {
  public:
    Int64Type(ThreadContext *ctxt) : SignedIntegerType<int64_t,TypeInt64>(ctxt,"int64") {}
    virtual ~Int64Type() {}
  };

  class UInt64Type : public IntegerType<uint64_t,TypeUInt64>
  {
  public:
    UInt64Type(ThreadContext *ctxt) : IntegerType<uint64_t,TypeUInt64>(ctxt,"uint64") {}
    virtual ~UInt64Type() {}
  };

  class IndexType : public IntegerType<ndx_t,TypeIndex>
  {
  public:
    IndexType(ThreadContext *ctxt) : IntegerType<ndx_t,TypeIndex>(ctxt,"index") {}
    virtual ~IndexType() {}
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
