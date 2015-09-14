#ifndef __FloatType_hpp__
#define __FloatType_hpp__

#include "NumericType.hpp"
#include "Operators.hpp"
#include "UnaryOp.hpp"
#include "MatrixOps.hpp"

namespace FM
{
  
  struct ThreadContext;

  template <class T>
  class FloatType : public NumericType<T> {
  public:
    FloatType(ThreadContext* ctxt, const FMString &name) : NumericType<T>(ctxt,name) {}
    virtual ~FloatType() {}
    virtual Type* typeInstance() {return this;}
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info);
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
    virtual Object Neg(const Object &a) {return FM::dispatch_unaryop<T,OpNeg>(a,this);};
    virtual Object Hermitian(const Object &a) {return MatrixHermitian<T>(a);}
    virtual Object Multiply(const Object &a, const Object &b) {
      if (a.isScalar() || b.isScalar()) return this->DotMultiply(a,b);
      return MatrixMultiply(a,b,Type::_ctxt);
    }
    virtual Object LeftDivide(const Object &a, const Object &b) {
      if (a.isScalar() || b.isScalar()) return this->DotLeftDivide(a,b);
      return MatrixLeftDivide(a,b,Type::_ctxt);
    }
    virtual Object RightDivide(const Object &a, const Object &b) {
      if (a.isScalar() || b.isScalar()) return this->DotRightDivide(a,b);
      return MatrixRightDivide(a,b,Type::_ctxt);
    }
    bool isSymmetric(const Object &b);
  };
};


#endif
