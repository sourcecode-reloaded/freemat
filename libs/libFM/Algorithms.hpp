#ifndef __Algorithms_hpp__
#define __Algorithms_hpp__

#include "Object.hpp"
#include "FloatType.hpp"

namespace FM
{

  template <class T>
  static inline void do_single_sided_algo_double(double a, double b, T *pvec, 
						 ndx_t adder, ndx_t count) {
    double d = a;
    for (ndx_t i=0;i<count;i++) {
      pvec[i*adder] = (T) d;
      d += b;
    }
  }
  
  template <class T>
  static inline void do_double_sided_algo_double(double a, double b, double c, T *pvec, 
						 ndx_t adder, ndx_t count) {
    if (count%2) {
      do_single_sided_algo_double(a,b,pvec,adder,count/2);
      do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2+1);
    } else {
      do_single_sided_algo_double(a,b,pvec,adder,count/2);
      do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2);
    }
  }

  template <class T>
  Object RangeConstructor(double minval, double stepsize, double maxval, bool vert, FloatType<T> *type)
  {
    Object ret(type->empty());
    if (stepsize == 0) 
      throw Exception("step size must be nonzero in colon expression");
    if (!std::isfinite(minval) || !std::isfinite(stepsize) || !std::isfinite(maxval))
      return type->empty();
    //ideally, n = (c-a)/b
    // But this really defines an interval... we let
    // n_min = min(c-a)/max(b)
    // n_max = max(c-a)/min(b)
    // where min(x) = {y \in fp | |y| is max, |y| < |x|, sign(y) = sign(x)}
    //       max(x) = {y \in fp | |y| is min, |y| > |x|, sign(y) = sign(x)}
    double ntest_min = nextafter(maxval-minval,0)/nextafter(stepsize,stepsize+stepsize);
    double ntest_max = nextafter(maxval-minval,maxval-minval+stepsize)/nextafter(stepsize,0);
    ndx_t npts = (ndx_t) floor(ntest_max);
    bool use_double_sided = (ntest_min <= npts) && (npts <= ntest_max);
    npts++;
    if (npts < 0) npts = 0;
    if (vert)
      ret = type->makeMatrix(npts,1);
    else
      ret = type->makeMatrix(1,npts);
    if (use_double_sided)
      do_double_sided_algo_double<T>(minval,stepsize,maxval,type->rw(ret),ndx_t(1),ndx_t(npts));
    else
      do_single_sided_algo_double<T>(minval,stepsize,type->rw(ret),ndx_t(1),ndx_t(npts));
    return ret;
  }
};

#endif
