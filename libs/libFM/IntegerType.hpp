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
  public:
    IntegerType(ThreadContext* ctxt, const FMString& name) : NumericType<T,codeNum>(ctxt,name) {}
    virtual ~IntegerType() {}
    virtual Type* typeInstance() {return this;}
    template <class Op>
    Object binop(const Object &a, const Object &b)
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
    virtual Object Add(const Object &a, const Object &b) 
    {
      return binop<OpAdd>(a,b);
    }
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
  };

  template <class T, FM::DataCode codeNum>
  class SignedIntegerType : public IntegerType<T,codeNum> {
    SignedIntegerType(ThreadContext* ctxt, const FMString &name) : IntegerType<T,codeNum>(ctxt,name) {}
    virtual ~SignedIntegerType() {}
    virtual Type* typeInstance() {return this;}
    virtual Object Neg(const Object & a)
    {
      return dispatch_unaryop<T,OpNeg>(a,this);
    }
  };

  class Int32Type : public IntegerType<int32_t,TypeInt32>
  {
  public:
    Int32Type(ThreadContext *ctxt) : IntegerType<int32_t,TypeInt32>(ctxt,"int32") {}
    virtual ~Int32Type() {}
  };

  class UInt32Type : public IntegerType<uint32_t,TypeUInt32>
  {
  public:
    UInt32Type(ThreadContext *ctxt) : IntegerType<uint32_t,TypeUInt32>(ctxt,"uint32") {}
    virtual ~UInt32Type() {}
  };

  class Int64Type : public IntegerType<int64_t,TypeInt64>
  {
  public:
    Int64Type(ThreadContext *ctxt) : IntegerType<int64_t,TypeInt64>(ctxt,"int64") {}
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
    Object expandColons(dim_t len) {
      Object output = this->makeMatrix(len,1);
      ndx_t *op = this->readWriteData(output);
      for (dim_t i=0;i<len;i++)
	op[i] = i;
      return output;
    }
  };
};

#endif
