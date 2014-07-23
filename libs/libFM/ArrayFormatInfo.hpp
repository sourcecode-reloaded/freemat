#ifndef __ArrayFormatInfo_hpp__
#define __ArrayFormatInfo_hpp__

#include "TermIF.hpp"

// Helper class used in printing information
namespace FM
{
  struct ArrayFormatInfo {
    int width;
    bool floatasint;
    int decimals;
    bool expformat;
    double scalefact;
    ArrayFormatInfo(int w = 0, bool f = false, 
		    int d = 0, bool e = false, 
		    double s = 1.0) : 
      width(w), floatasint(f), 
      decimals(d), expformat(e), 
      scalefact(s) {}
  };

  template <typename T>
  ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode mode, T x = 0);
};


#endif
