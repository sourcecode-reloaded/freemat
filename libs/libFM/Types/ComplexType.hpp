#ifndef __ComplexType_hpp__
#define __ComplexType_hpp__

#include "DoubleType.hpp"

namespace FM
{
  struct ThreadContext;

  // This is a proxy type - ideally, we would eliminate the notion of a complex type (or
  // use it everywhere complexity is required).  Unfortunately, that isn't really possible.
  // So as a result, we build this proxy type which really just wraps the double type.  All
  // operations are proxied to the underlying double type, with parameters set so that
  // complex objects are created when necessary.  
  class ComplexProxyType : public ArrayType<Complex<double> > {
    ComplexProxyType(ThreadContext *ctxt) : FloatType(ctxt, "complex double proxy") {}
    virtual ~ComplexProxyType() {}
    virtual DataCode code() const {return ctxt->_double->code();}
    virtual const FMString& name() const {return ctxt->_double->name();}
    virtual void destroyObject(ObjectBase* p) {ctxt->_double->destroyObject(p);}
    virtual Data* duplicateData(const ObjectBase * p, ndx_t &reserve) const = 0;
    virtual void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const = 0;
    virtual Object empty() = 0;
    virtual Object import(const Object &foreign) = 0;
    virtual Object Add(const Object &a, const Object &b);
    virtual Object Multiply(const Object &a, const Object &b);
    virtual Object Power(const Object &a, const Object &b);
    virtual Object LeftDivide(const Object &a, const Object &b);
    virtual Object RightDivide(const Object &a, const Object &b);
    virtual Object DotLeftDivide(const Object &a, const Object &b);
    virtual Object DotRightDivide(const Object &a, const Object &b);
    virtual Object DotMultiply(const Object &a, const Object &b);
    virtual Object DotPower(const Object &a, const Object &b);
    virtual Object Subtract(const Object &a, const Object &b);
    virtual Object LessEquals(const Object &a, const Object &b);
    virtual Object LessThan(const Object &a, const Object &b);
    virtual Object GreaterThan(const Object &a, const Object &b);
    virtual Object GreaterEquals(const Object &a, const Object &b);
    virtual Object NotEquals(const Object &a, const Object &b);
    virtual Object Equals(const Object &a, const Object &b);
    virtual Object Or(const Object &a, const Object &b);
    virtual Object And(const Object &a, const Object &b);
    virtual Object Neg(const Object &a);
    virtual Object Not(const Object &a);
    virtual Object Plus(const Object &a);
    virtual Object Colon(const Object &a, const Object &b);
    virtual Object NCat(const Object &p, ndx_t dimension);
    virtual Object DoubleColon(const Object &a, const Object &b, const Object &c);
    virtual Object Transpose(const Object &a);
    virtual Object Hermitian(const Object &a);
    virtual bool equals(const Object &a, const Object &b) = 0;
    virtual FMString describe(const Object &a) = 0;
    virtual FMString brief(const Object &a) = 0;
    virtual Object asIndex(const Object &a, ndx_t max);
    virtual Object asIndexNoBoundsCheck(const Object &a);
    virtual Object asLogical(const Object &a);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object getBraces(const Object &a, const Object &b);
    virtual Object getField(const Object &a, const Object &b);
    virtual Object getFieldNoGetters(const Object &a, const Object &b);
    virtual Object sliceColumn(const Object &a, ndx_t column);
    virtual Object convert(const Object &a);
    virtual double doubleValue(const Object &a);
    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual void setBraces(Object &a, const Object &args, const Object &b);
    virtual void setField(Object &a, const Object &args, const Object &b);
    virtual void setFieldNoSetters(Object &a, const Object &args, const Object &b);
    virtual Object get(const Object &a, const Object &b, bool invokeGetters);
    virtual void set(Object &a, const Object &args, const Object &b, bool invokeSetters);
    virtual void resize(Object &a, const Tuple &newsize);
    virtual void print(const Object &a);
    virtual Object deref(const Object &a);
    virtual Object call(const Object &a, const Object &args, ndx_t nargout);    
  };
};

#endif
