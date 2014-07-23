#ifndef __IntegerType_hpp__
#define __IntegerType_hpp__

#include "NumericType.hpp"
#include "BinOp.hpp"
#include "Operators.hpp"

namespace FM
{

  struct BaseTypes;

  template <class T, FM::DataCode codeNum>
  class IntegerType : public NumericType<T,codeNum> {
  public:
    IntegerType(BaseTypes* base, const FMString& name) : NumericType<T,codeNum>(base,name) {}
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
    virtual void printElement(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t offset);
  };

  class Int32Type : public IntegerType<int32_t,TypeInt32>
  {
  public:
    Int32Type(BaseTypes *base) : IntegerType<int32_t,TypeInt32>(base,"int32") {}
    virtual ~Int32Type() {}
  };

  class UInt32Type : public IntegerType<uint32_t,TypeUInt32>
  {
  public:
    UInt32Type(BaseTypes *base) : IntegerType<uint32_t,TypeUInt32>(base,"uint32") {}
    virtual ~UInt32Type() {}
  };

  class Int64Type : public IntegerType<int64_t,TypeInt64>
  {
  public:
    Int64Type(BaseTypes *base) : IntegerType<int64_t,TypeInt64>(base,"int64") {}
    virtual ~Int64Type() {}
  };

  class UInt64Type : public IntegerType<uint64_t,TypeUInt64>
  {
  public:
    UInt64Type(BaseTypes *base) : IntegerType<uint64_t,TypeUInt64>(base,"uint64") {}
    virtual ~UInt64Type() {}
  };

  class IndexType : public IntegerType<ndx_t,TypeIndex>
  {
  public:
    IndexType(BaseTypes *base) : IntegerType<ndx_t,TypeIndex>(base,"index") {}
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
