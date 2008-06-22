#ifndef __Complex_hpp__
#define __Complex_hpp__

template <typename T>
static inline void complex_divide(const T& ar, const T& ai,
				  const T& br, const T& bi,
				  T& c0, T& c1) {
  double ratio, den;
  double abr, abi, cr;
  
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
  return atan2(ai,ar);
}

template <typename T>
inline void complex_multiply(const T &ar, const T &ai, 
			     const T &br, const T &bi, 
			     T &cr, T &ci) {
  cr = ar * br - ai * bi;
  ci = ar * bi + ai * br;  
}

template <typename T>
inline bool complex_lt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if (mag_a < mag_b) return true;
  if (mag_b < mag_a) return false;
  return (complex_phase(ar,ai) < complex_phase(br,bi));
}

template <typename T>
inline bool complex_gt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if (mag_a > mag_b) return true;
  if (mag_b > mag_a) return false;
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

#endif
