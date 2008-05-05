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

#endif
