#ifndef __NumericType_hpp__
#define __NumericType_hpp__

#include "PODType.hpp"
#include "Hermitian.hpp"
#include "Transpose.hpp"

namespace FM
{

  struct ThreadContext;

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
    virtual Object Plus(const Object &a) {
      return a;
    }
    virtual T minValue(const Object &a)
    {
      if (a.isEmpty()) throw Exception("Empty arguments do no support minValue");
      const T* ap = this->ro(a);
      ndx_t acnt = a.count();
      T ret = ap[0];
      for (ndx_t i=1;i<acnt;i++)
	ret = (ap[i] < ret) ? ap[i] : ret;
      return ret;
    }
    virtual T maxValue(const Object &a) 
    {
      if (a.isEmpty()) throw Exception("Empty arguments do no support maxValue");
      const T* ap = this->ro(a);
      ndx_t acnt = a.count();
      T ret = ap[0];
      for (ndx_t i=1;i<acnt;i++)
	ret = (ap[i] > ret) ? ap[i] : ret;
      return ret;
    }
    virtual Object getBraces(const Object &, const Object &) {throw Exception("{} indexing unsupported for numeric arrays");}
    virtual Object getField(const Object &, const Object &) {throw Exception(". indexing unsupported for numeric arrays");}
    virtual Object convert(const Object &a);
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
