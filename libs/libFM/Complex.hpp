#ifndef __Complex_hpp__
#define __Complex_hpp__

#include "IEEEFP.hpp"

namespace FM
{

  template <class elem>
  struct Complex
  {
    elem r;
    elem i;
    Complex() : r(0), i(0) {}
    Complex(elem real) : r(real), i(elem(0)) {}
    Complex(elem real, elem imag) : r(real), i(imag) {}
    template <class elem2>
    Complex(Complex<elem2> other)
    {
      r = other.r;
      i = other.i;
    }
  };

  template <typename elem>
  static inline Complex<elem> complex_conj(const Complex<elem> &a)
  {
    Complex<elem> c;
    c.r = a.r;
    c.i = -a.i;
    return c;
  }

  template <typename T>
  static inline void complex_divide(const T& ar, const T& ai,
				    const T& br, const T& bi,
				    T& c0, T& c1) {
    double ratio, den;
    double abr, abi, cr;
    if ((ai == 0) && (bi == 0)) {
      c1 = 0;
      c0 = ar/br;
      return;
    }
    if (bi == 0) {
      c0 = ar/br;
      c1 = ai/br;
      return;
    }
    if ((ar == 0) && (bi == 0)) {
      c0 = 0;
      c1 = ai/br;
      return;
    }
    if ((ai == 0) && (br == 0)) {
      c0 = 0;
      c1 = -ar/bi;
      return;
    }
    if ((ar == br) && (ai == bi)) {
      c0 = 1; c1 = 0;
      return;
    }
    if( (abr = br) < 0.)
      abr = - abr;
    if( (abi = bi) < 0.)
      abi = - abi;
    if( abr <= abi )
      {
	if(abi == 0) {
	  if (ai != 0 || ar != 0)
	    abi = 1.;
	  c1 = c0 = abi / abr;
	  return;
	}
	ratio = br / bi ;
	den = bi * (1 + ratio*ratio);
	cr = (ar*ratio + ai) / den;
	c1 = (ai*ratio - ar) / den;
      }
    else
      {
	ratio = bi / br ;
	den = br * (1 + ratio*ratio);
	cr = (ar + ai*ratio) / den;
	c1 = (ai - ar*ratio) / den;
      }
    c0 = cr;
  }

  template <typename T>
  static inline void complex_recip(const T& ar, const T& ai, T& cr, T& ci) {
    if (ai == 0) {
      ci = 0;
      cr = 1/ar;
      return;
    }
    complex_divide<T>(1,0,ar,ai,cr,ci);
  }

  template <class T>
  inline T complex_abs(T real, T imag) {
    T swap;
    if(real < 0)
      real = -real;
    if(imag < 0)
      imag = -imag;
    if(imag > real){
      swap = real;
      real = imag;
      imag = swap;
    }
    if((real+imag) == real)
      return(real);
    double temp = double(imag)/double(real);
    temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
    return(T(temp));
  }

  template <typename T>
  inline T complex_phase(const T &ar, const T &ai) {
    return T(atan2(double(ai),double(ar)));
  }

  template <typename T>
  inline void complex_multiply(const T &ar, const T &ai, 
			       const T &br, const T &bi, 
			       T &cr, T &ci) {
    if ((ai == 0) && (bi == 0)) {
      cr = ar * br;
      ci = 0;
    } else if (ai == 0) {
      cr = ar * br;
      ci = ar * bi;
    } else if (bi == 0) {
      cr = br * ar;
      ci = br * ai;
    } else {
      cr = ar * br - ai * bi;
      ci = ar * bi + ai * br;
    }
  }

  template <typename T>
  inline bool complex_lt(const T &ar, const T &ai, 
			 const T &br, const T &bi) {
    T mag_a = complex_abs(ar,ai);
    T mag_b = complex_abs(br,bi);
    if ((mag_b-mag_a) > feps(mag_a)*4) return true;
    if ((mag_a-mag_b) > feps(mag_b)*4) return false;
    return (complex_phase(ar,ai) < complex_phase(br,bi));
  }

  template <typename T>
  inline bool complex_gt(const T &ar, const T &ai, 
			 const T &br, const T &bi) {
    T mag_a = complex_abs(ar,ai);
    T mag_b = complex_abs(br,bi);
    if ((mag_b-mag_a) > feps(mag_a)*4) return false;
    if ((mag_a-mag_b) > feps(mag_b)*4) return true;
    return (complex_phase(ar,ai) > complex_phase(br,bi));
  }

  template <typename T>
  inline bool complex_eq(const T & ar, const T & ai, 
			 const T & br, const T & bi) {
    return ((ar == br) && (ai == bi));
  }

  template <typename T>
  inline bool complex_ne(const T & ar, const T & ai,
			 const T & br, const T & bi) {
    return ((ar != br) || (ai != bi));
  }

  template <typename T>
  inline bool complex_le(const T & ar, const T & ai, 
			 const T & br, const T & bi) {
    return complex_eq(ar,ai,br,bi) || complex_lt(ar,ai,br,bi);
  }

  template <typename T>
  inline bool complex_ge(const T & ar, const T & ai, 
			 const T & br, const T & bi) {
    return complex_eq(ar,ai,br,bi) || complex_gt(ar,ai,br,bi);
  }

  template <typename T>
  inline void complex_log(T xr, T xi, T &yr, T &yi) {
    yr = log(complex_abs(xr,xi));
    yi = atan2(xi,xr);
  }

  template <typename T>
  inline void complex_exp(T xr, T xi, T &yr, T &yi) {
    yr = exp(xr)*cos(xi);
    yi = exp(xr)*sin(xi);
  }

  template <typename T>
  inline void complex_sqrt(T xr, T xi, T &yr, T &yi) {
    T tr, ti;
    if ((xr >= 0) && (xi == 0)) {
      yr = sqrt(xr);
      yi = 0;
      return;
    }
    if ((xr < 0) && (xi == 0)) {
      yi = sqrt(-xr);
      yr = 0;
      return;
    }
    complex_log(xr,xi,tr,ti);
    tr /= 2.0;
    ti /= 2.0;
    complex_exp(tr,ti,yr,yi);
  }

  template <typename T>
  inline void complex_square(T xr, T xi, T &yr, T &yi) {
    yr = xr*xr - xi*xi;
    yi = 2*xr*xi;
  }

  template <typename T>
  static inline void complex_pow(const T& ar, const T& ai,
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

  
  template <class elem>
  static Complex<elem> operator+(const Complex<elem> &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator+(const elem &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a + b.r;
    c.i = b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator+(const Complex<elem> &a, const elem &b)
  {
    Complex<elem> c;
    c.r = a.r + b;
    c.i = a.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator-(const Complex<elem> &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a.r - b.r;
    c.i = a.i - b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator-(const elem &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a - b.r;
    c.i = b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator-(const Complex<elem> &a, const elem &b)
  {
    Complex<elem> c;
    c.r = a.r - b;
    c.i = a.i;
    return c;
  }

  template <class elem>
  static bool operator<(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_lt(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator<=(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_le(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator>(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_gt(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator>=(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_ge(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator!=(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_ne(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator==(const Complex<elem> &a , const Complex<elem> &b)
  {
    return complex_eq(a.r,a.i,b.r,b.i);
  }

  template <class elem>
  static bool operator||(const Complex<elem> &a , const Complex<elem> &b)
  {
    return (a.r || b.r || a.i || b.i);
  }

  template <class elem>
  static bool operator&&(const Complex<elem> &a , const Complex<elem> &b)
  {
    return (a.r || a.i) && (b.r || b.i);
  }

  template <class elem>
  static Complex<elem> pow(const Complex<elem> &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    complex_pow(a.r,a.i,b.r,b.i,c.r,c.i);
    return c;
  }
  
  template <class elem>
  static Complex<elem> operator*(const Complex<elem> &a , const Complex<elem> &b)
  {
    Complex<elem> c;
    complex_multiply(a.r,a.i,b.r,b.i,c.r,c.i);
    return c;
  }

  template <class elem>
  static Complex<elem> operator/(const Complex<elem> &a , const Complex<elem> &b)
  {
    Complex<elem> c;
    complex_divide(a.r,a.i,b.r,b.i,c.r,c.i);
    return c;
  }

  template <class elem>
  inline std::stringstream& operator<<(std::stringstream& o, const Complex<elem> &a)
  {
    o << a.r << "+" << a.i << "i";
    return o;
  }

  template <>
  inline std::stringstream& operator<<(std::stringstream& o, const Complex<int8_t> &a)
  {
	  o << int(a.r) << "+" << int(a.i) << "i";
	  return o;
  }
}


#endif
