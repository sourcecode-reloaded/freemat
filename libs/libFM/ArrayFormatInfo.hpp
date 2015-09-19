#ifndef __ArrayFormatInfo_hpp__
#define __ArrayFormatInfo_hpp__

#include "TermIF.hpp"
#include "Type.hpp"
#include "Complex.hpp"

// Helper class used in printing information
namespace FM
{
  struct ArrayFormatInfo {
    int total_width;
    int number_width;
    bool floatasint;
    int decimals;
    bool expformat;
    double scalefact;
    bool needs_space;
    bool pure_real;
    bool pure_imag;
    ArrayFormatInfo(int t = 0, int w = 0,
		    bool f = false, int d = 0,
		    bool e = false, double s = 1.0) :
      total_width(t), number_width(w), floatasint(f), 
      decimals(d), expformat(e), 
      scalefact(s), needs_space(true),
      pure_real(false), pure_imag(false) {}
  };

  template <typename T>
  void GetArrayFormatForPODArray(FMFormatMode mode, const T* rp, ndx_t count, ArrayFormatInfo &info);

  template <typename T>
  void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<T>* rp, ndx_t count, ArrayFormatInfo &info);

  template <typename T>
  FMString FormattedNumber(const ArrayFormatInfo &format, const T& val);

  template <typename T>
  FMString FormattedNumber(const ArrayFormatInfo &format, const Complex<T>& val);
};



#endif
