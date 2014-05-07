#ifndef __FMEnvironment_hpp__
#define __FMEnvironment_hpp__

#include "FMObjectPool.hpp"

class FMEnvironment
{
public:
  FMObject makeScalar(double t);
  FMObject makeComplexScalar(double real, double imag);
};

#endif
