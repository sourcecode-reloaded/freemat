#ifndef __Convert_hpp__
#define __Convert_hpp__

#include "Complex.hpp"

namespace FM
{
  
  template <class T, class S>
  struct OpRangeConvert
  {
    // FIXME - does using double here break int64?
    inline static void rangeConvert(T& y, const S& x) {
      if (double(x) < double(std::numeric_limits<T>::min()))
	y = std::numeric_limits<T>::min();
      else if (double(x) > double(std::numeric_limits<T>::max()))
	y = std::numeric_limits<T>::max();
      else
	y = (T)(x);
    }
  };

  template <class T, class S>
  struct OpRangeConvert<Complex<T>,Complex<S> >
  {
    inline static void rangeConvert(Complex<T> &y, const Complex<S> &x) {
      OpRangeConvert<T,S>::rangeConvert(y.r,x.r);
      OpRangeConvert<T,S>::rangeConvert(y.i,x.i);
    }
  };
  
  template <class T>
  struct OpRangeConvert<Complex<T>,Complex<T> >
  {
    inline static void rangeConvert(Complex<T>& y, const Complex<T>& x) {
      y.r = x.r;
      y.i = x.i;
    }
  };

  template <>
  struct OpRangeConvert<Complex<double>,double >
  {
    inline static void rangeConvert(Complex<double>& y, const double& x) {
      y.r = x;
      y.i = 0;
    }
  };

  template <class T>
  struct OpRangeConvert<T,Complex<T> >
  {
    inline static void rangeConvert(T& y, const Complex<T>& x) {
      y = x.r;
    }
  };

  template <class S>
  struct OpRangeConvert<bool, S>
  {
    inline static void rangeConvert(bool&y, const S& x) {
      y = (x != 0);
    }
  };
  
  template <>
  struct OpRangeConvert<bool, Complex<double> >
  {
    inline static void rangeConvert(bool&y, const Complex<double>& x) {
      y = ((x.r != 0) || (x.i != 0));
    }
  };
  
  template <>
  struct OpRangeConvert<Complex<double>, bool >
  {
    inline static void rangeConvert(Complex<double> &y, const bool& x) {
      y.r = (x ? 1 : 0);
      y.i = 0;
    }
  };
  
  template <>
  struct OpRangeConvert<bool,float>
  {
    inline static void rangeConvert(bool& y, const float&x) {
      if (isnan(x)) throw Exception("Cannot convert NaN values to logicals");
      y = (x != 0);
    }
  };

  template <>
  struct OpRangeConvert<bool,double>
  {
    inline static void rangeConvert(bool& y, const double&x) {
      if (isnan(x)) throw Exception("Cannot convert NaN values to logicals");
      y = (x != 0);
    }
  };

  template <class T>
  struct OpRangeConvert<T,float>
  {
    inline static void rangeConvert(T& y, const float &x) {
      if (x <= std::numeric_limits<T>::min())
	y = std::numeric_limits<T>::min();
      else if (x >= std::numeric_limits<T>::max())
	y = std::numeric_limits<T>::max();
      else
	y = (T)(lroundf(x));
    }
  };

  template <>
  struct OpRangeConvert<float,float>
  {
    inline static void rangeConvert(float& y, const float &x) {
      y = x;
    }
  };

  template <>
  struct OpRangeConvert<double,float>
  {
    inline static void rangeConvert(double& y, const float &x) {
      y = x;
    }
  };

  template <class T>
  struct OpRangeConvert<T,double>
  {
    inline static void rangeConvert(T& y, const double &x) {
      if (x <= std::numeric_limits<T>::min())
	y = std::numeric_limits<T>::min();
      else if (x >= std::numeric_limits<T>::max())
	y = std::numeric_limits<T>::max();
      else
	y = (T)(lround(x));
    }
  };

  template <>
  struct OpRangeConvert<double,double>
  {
    inline static void rangeConvert(double& y, const double &x) {
      y = x;
    }
  };

  template <>
  struct OpRangeConvert<float,double>
  {
    inline static void rangeConvert(float& y, const double &x) {
      y = float(x); // Check for under/overflow?
    }
  };

  template <class T, class S>
  void convertLoop(const S* ip, T* op, ndx_t len)
  {
    for (ndx_t i=0;i<len;i++)
      op[i] = static_cast<T>(ip[i]);
  }
}

#endif
