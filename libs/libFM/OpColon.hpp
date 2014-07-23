#ifndef __OpColon_hpp__
#define __OpColon_hpp__

#include "Algorithms.hpp"

namespace FM
{

  struct OpColon
  {
    template <class ctype, FM::DataCode codeNum>
    inline static Object binary(double minval, double maxval, FloatType<ctype,codeNum> *o)
    {
      return RangeConstructor<ctype,codeNum>(minval,1.0,maxval,false,o);
    }
    template <class ctype, FM::DataCode codeNum>
    inline static Object trinary(double minval, double stepsize, double maxval, FloatType<ctype,codeNum> *o)
    {
      return RangeConstructor<ctype,codeNum>(minval,stepsize,maxval,false,o);
    }
  };
};


#endif
