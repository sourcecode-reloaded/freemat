#ifndef __ScalarType_hpp__
#define __ScalarType_hpp__

#include <complex>
#include "FMTypeObject.hpp"

typedef complex<double> FMScalarData;

class FMScalarType : public FMType
{
  inline const FMScalarData& val(const FMObject&a) {
    return *(a->as<FMScalarData>());
  }
public:
  const FMObject& Add(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(val(a)+val(b));
  }
  const FMObject& Subtract(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(val(a)-val(b));
  }
  const FMObject& Multiply(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(val(a)*val(b));
  }
  const FMObject& RightDivide(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(val(a)/val(b));
  }
  const FMObject& LeftDivide(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(val(b)/val(a));
  }
  const FMObject& Or(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    return env->makeScalar(BOOL(val(a).real() | val(b).real() | val(a).imag() | val(b).imag()));
  }
  const FMObject& And(const FMObject &a, const FMObject &b, FMEnvironment *env)
  {
    
  }
  
};

#endif
