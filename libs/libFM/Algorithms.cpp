#include "Algorithms.hpp"

using namespace FM;

Object RangeConstructor(double minval, double stepsize, double maxval, bool vert, FloatType *type) {
  Object ret;
  if (stepsize == 0) 
    throw Exception("step size must be nonzero in colon expression");
  if (!std::isfinite(minval) || !std::isfinite(stepsize) || !std::isfinite(maxval))
    return EmptyConstructor();
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
    do_double_sided_algo_double(minval,stepsize,maxval,rp.data(),int(1),int(npts));
  else
    do_single_sided_algo_double(minval,stepsize,rp.data(),int(1),int(npts));
  return Array(rp);
}
