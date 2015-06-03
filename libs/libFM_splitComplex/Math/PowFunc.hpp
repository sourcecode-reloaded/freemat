#ifndef __Pow_hpp__
#define __Pow_hpp__

#include "Complex.hpp"
#include <cmath>

namespace FM {
  
template <typename T>
static inline T powi(T a, int b) {
  T p, x;
  int n;
  unsigned long u;
    
  p = 1;
  x = a;
  n = b;
  
  if(n != 0) {
    if(n < 0) {
      n = -n;
      x = 1/x;
    }
    for(u = n; ; )  {
      if(u & 01)
	p *= x;
      if(u >>= 1)
	x *= x;
      else
	break;
    }
  }
  return(p);
}  

template <typename T>
static inline void powi(const T& ar, const T& ai, int b,
			T& pr, T& pi) {
  int n;
  unsigned long u;
  T t;
  T q[2], x[2];
  static T one[2] = {1.0, 0.0};
  n = b;
  q[0] = 1;
  q[1] = 0;
    
  if(n == 0)
    goto done;
  if(n < 0)
    {
      n = -n;
      complex_divide<T>(one[0],one[1],ar,ai,x[0],x[1]);
    }
  else
    {
      x[0] = ar;
      x[1] = ai;
    }
  for(u = n; ; )
    {
      if(u & 01)
	{
	  t = q[0] * x[0] - q[1] * x[1];
	  q[1] = q[0] * x[1] + q[1] * x[0];
	  q[0] = t;
	}
      if(u >>= 1)
	{
	  t = x[0] * x[0] - x[1] * x[1];
	  x[1] = 2 * x[0] * x[1];
	  x[0] = t;
	}
      else
	break;
    }
 done:
  pi = q[1];
  pr = q[0];
}

template <typename T>
static inline void powfunc(const T& ar, const T& ai,
			   const T& br, const T& bi,
			   T& cr, T& ci) {
  T logr, logi, x, y;
  T mag = complex_abs<T>(ar, ai);
  if (mag == 0) {
    cr = 0;
    ci = 0;
    return;
  }
  logr = log(mag);
  logi = atan2(ai, ar);
  
  x = exp( logr * br - logi * bi );
  y = logr * bi + logi * br;
  
  cr = x * cos(y);
  ci = x * sin(y);
}

inline float powfunc(const float &a, const float &b) {
  if (b == int(b))
    return powi(a,int(b));
  else
    return ::powf(a,b);
}

inline double powfunc(const double &a, const double &b) {
  if (b == int(b))
    return powi(a,int(b));
  else
    return ::pow(a,b);
}

  inline long double powfunc(const long double &a, const long double &b) {
  if (b == int(b))
    return powi(a,int(b));
  else
    return ::powl(a,b);
}

inline Complex<float> powfunc(const Complex<float> &a, const Complex<float> &b) {
  Complex<float> ret;
  if ((b.i == 0) && (b.r == int(b.r)))
    powi(a.r,a.i,int(b.r),ret.r,ret.i);
  else
    powfunc(a.r,a.i,b.r,b.i,ret.r,ret.i);
  return ret;
}

  inline Complex<double> powfunc(const Complex<double> &a, const Complex<double> &b) {
  Complex<double> ret;
  if ((b.i == 0) && (b.r == int(b.r)))
    powi(a.r,a.i,int(b.r),ret.r,ret.i);
  else
    powfunc(a.r,a.i,b.r,b.i,ret.r,ret.i);
  return ret;
  }

  inline Complex<long double> powfunc(const Complex<long double> &a, const Complex<long double> &b) {
  Complex<long double> ret;
  if ((b.i == 0) && (b.r == int(b.r)))
    powi(a.r,a.i,int(b.r),ret.r,ret.i);
  else
    powfunc(a.r,a.i,b.r,b.i,ret.r,ret.i);
  return ret;
}

}
#endif
