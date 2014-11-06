#ifndef __NumericType_hpp__
#define __NumericType_hpp__

#include "PODType.hpp"
#include "Hermitian.hpp"

namespace FM
{

  struct ThreadContext;

  template <class T, FM::DataCode codeNum>
  class NumericType : public PODType<T> {
  public:
    NumericType(ThreadContext *ctxt, const FMString &name) : PODType<T>(ctxt,name) {}
    virtual ~NumericType() {}
    virtual DataCode code() const {return codeNum;}
    virtual Object asLogical(const Object &a);
    virtual Object asComplex(const Object &a);
    virtual Object asIndex(const Object &a, dim_t max);
    virtual Object asIndexNoBoundsCheck(const Object &a);
    double doubleValue(const Object &a) {
      return static_cast<double>(this->scalarValue(a));
    }
    virtual Object Plus(const Object &a) {
      return a;
    }
    virtual T minValue(const Object &a)
    {
      if (a.isEmpty() || a.isComplex()) throw Exception("Complex or empty arguments do no support maxValue");
      const T* ap = this->ro(a);
      dim_t acnt = a.elementCount();
      T ret = ap[0];
      for (dim_t i=1;i<acnt;i++)
	ret = (ap[i] < ret) ? ap[i] : ret;
      return ret;
    }
    virtual T maxValue(const Object &a) 
    {
      if (a.isEmpty() || a.isComplex()) throw Exception("Complex or empty arguments do no support maxValue");
      const T* ap = this->ro(a);
      dim_t acnt = a.elementCount();
      T ret = ap[0];
      for (dim_t i=1;i<acnt;i++)
	ret = (ap[i] > ret) ? ap[i] : ret;
      return ret;
    }
    virtual Object getBraces(const Object &a, const Object &b) {throw Exception("{} indexing unsupported for numeric arrays");}
    virtual Object getField(const Object &a, const Object &b) {throw Exception(". indexing unsupported for numeric arrays");}
    virtual Object convert(const Object &a);
    virtual Object Hermitian(const Object &a) {return MatrixHermitian<T>(a);}
  };
}


#endif
