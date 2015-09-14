#ifndef __OpColon_hpp__
#define __OpColon_hpp__

#include "Algorithms.hpp"

namespace FM
{

  struct OpColon
  {
    template <class ctype>
    inline static Object binary(double minval, double maxval, FloatType<ctype> *o)
    {
      return RangeConstructor<ctype>(minval,1.0,maxval,false,o);
    }
    template <class ctype>
    inline static Object trinary(double minval, double stepsize, double maxval, FloatType<ctype> *o)
    {
      return RangeConstructor<ctype>(minval,stepsize,maxval,false,o);
    }
  };
};


#endif
