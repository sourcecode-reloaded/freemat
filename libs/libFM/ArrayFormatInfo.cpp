#include "ArrayFormatInfo.hpp"
#include "Complex.hpp"
#include "Exception.hpp"
#include <math.h>

namespace FM
{
  template <class T>
  ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode formatMode) {
    switch (formatMode)
      {
      case format_short:
	return ArrayFormatInfo(9,9,false,4);
      case format_long:
	return ArrayFormatInfo(18,18,false,14);
      case format_short_e:
	return ArrayFormatInfo(11,11,false,4,true);
      case format_long_e:
	return ArrayFormatInfo(21,21,false,14,true);
      default:
	throw Exception("Unhandled format type");
      }
  }

  template <>
  ArrayFormatInfo GetArrayFormatForPOD<char>(FMFormatMode) {
    ArrayFormatInfo p(1,1,false,0,false,1.0);
    p.needs_space = false;
    return p;
  }


  //Visually Tuned
  template <>
  ArrayFormatInfo GetArrayFormatForPOD<float>(FMFormatMode formatMode) {
    switch (formatMode)
      {
      case format_short:
	return ArrayFormatInfo(9,9,false,4);
      case format_long:
	return ArrayFormatInfo(11,11,false,7);
      case format_short_e:
	return ArrayFormatInfo(11,11,false,4,true);
      case format_long_e:
	return ArrayFormatInfo(14,14,false,7,true);
      default:
	throw Exception("Unhandled format type");
      }
  }

  //Visually Tuned
  template <>
  ArrayFormatInfo GetArrayFormatForPOD<double>(FMFormatMode formatMode) {
    switch (formatMode)
      {
      case format_short:
	return ArrayFormatInfo(9,9,false,4);
      case format_long:
	return ArrayFormatInfo(18,18,false,14);
      case format_short_e:
	return ArrayFormatInfo(11,11,false,4,true);
      case format_long_e:
	return ArrayFormatInfo(21,21,false,14,true);
      default:
	throw Exception("Unhandled format type");
      }
  }


  //Visually Tuned
  template <>
  ArrayFormatInfo GetArrayFormatForPOD<cfloat>(FMFormatMode formatMode) {
    switch (formatMode)
      {
      case format_short:
	return ArrayFormatInfo(20,9,false,4);
      case format_long:
	return ArrayFormatInfo(24,11,false,7);
      case format_short_e:
	return ArrayFormatInfo(26,11,false,4,true);
      case format_long_e:
	return ArrayFormatInfo(32,14,false,7,true);
      default:
	throw Exception("Unhandled format type");
      }
  }

  //Visually Tuned
  template <>
  ArrayFormatInfo GetArrayFormatForPOD<cdouble>(FMFormatMode formatMode) {
    switch (formatMode)
      {
      case format_short:
	return ArrayFormatInfo(20,9,false,4);
      case format_long:
	return ArrayFormatInfo(40,18,false,14);
      case format_short_e:
	return ArrayFormatInfo(26,11,false,4,true);
      case format_long_e:
	return ArrayFormatInfo(46,21,false,14,true);
      default:
	throw Exception("Unhandled format type");
      }
  }

  static inline void updateArrayFormatInfoWithAmplitude(ArrayFormatInfo &format, double max_amplitude, double maxval) {
    if (max_amplitude >= maxval)
      format.scalefact = ::pow(double(10.0),double(floor(log10(max_amplitude))));
    else if (max_amplitude <= -maxval)
      format.scalefact = ::pow(double(10.0),double(floor(log10(-max_amplitude))));
    else if ((max_amplitude <= .1) && (max_amplitude>0))
      format.scalefact = ::pow(double(10.0),double(floor(log10(max_amplitude))));
    else if ((max_amplitude >= -.1) && (max_amplitude<0))
      format.scalefact = ::pow(double(10.0),double(floor(log10(-max_amplitude))));
    else
      format.scalefact = 1.0;
  }

  template <class T>
  static void ComputeScaleFactor(const T* array, ndx_t count, ArrayFormatInfo &format) {
    double max_amplitude = 0;
    if (count == 0) return;
    if (format.expformat) return;
    bool finiteElementFound = false;
    for (ndx_t i=0;i<count;i++) {
      if (isfinite(array[i]) && !finiteElementFound) {
	max_amplitude = array[i];
	finiteElementFound = true;
      }
      if ((isfinite(array[i])) && 
	  (fabs((double) array[i]) > fabs((double) max_amplitude)))
	max_amplitude = array[i];
    }
    if (!finiteElementFound) return;
    updateArrayFormatInfoWithAmplitude(format,max_amplitude,1000);
  }

  template <class T>
  static void ComputeScaleFactor(const Complex<T>* array, ndx_t count, ArrayFormatInfo &format) {
    double max_amplitude = 0;
    if (count == 0) return;
    if (format.expformat) return;
    bool finiteElementFound = false;
    for (ndx_t i=0;i<count;i++) {
      if (isfinite(array[i].r) && !finiteElementFound) {
	max_amplitude = array[i].r;
	finiteElementFound = true;
      }
      if (isfinite(array[i].i) && !finiteElementFound) {
	max_amplitude = array[i].i;
	finiteElementFound = true;
      }
      if (isfinite(array[i].r) && fabs((double)array[i].r) > fabs((double)max_amplitude))
	max_amplitude = array[i].r;
      if (isfinite(array[i].i) && fabs((double)array[i].i) > fabs((double)max_amplitude))
	max_amplitude = array[i].i;
    }
    if (!finiteElementFound) return;
    updateArrayFormatInfoWithAmplitude(format,max_amplitude,100);  
  }

  static inline int integerWidth(double t, bool imaginary = false) {
    if (t == 0) return 1;
    int signbit = 0;
    if ((t < 0) && !imaginary) signbit++;
    return int(ceil(log10(fabs(t)+1))) + signbit;
  }

  static inline bool isIntegerLike(double t) {
    return (isfinite(t) &&
	    ((ndx_t)(t) == t) &&
	    (t > -10000) &&
	    (t < 10000));
  }

  template <class T>
  static inline bool isIntegerLike(const Complex<T> &t) {
    return (isIntegerLike(t.r) && isIntegerLike(t.i));
  }

  template <class T>
  static inline int integerWidth(const Complex<T> &t) {
    return (std::max(integerWidth(t.r),integerWidth(t.i,true)));
  }

  template <class T>
  static inline int computeMaximumEntryWidth(const T* rv, ndx_t count) {
    int maxwidth_real = 1;
    for (ndx_t i=0;i<count;i++) 
      maxwidth_real = std::max(maxwidth_real,integerWidth(rv[i]));
    return maxwidth_real;
  }

  template <class T>
  static inline int computeMaximumEntryWidth(const Complex<T>* rv, ndx_t count) {
    int maxwidth_real = 1;
    int maxwidth_imag = 1;
    for (ndx_t i=0;i<count;i++) {
      maxwidth_real = std::max(maxwidth_real,integerWidth(rv[i].r));
      maxwidth_imag = std::max(maxwidth_imag,integerWidth(rv[i].i,true));
    }
    return std::max(maxwidth_real,maxwidth_imag);
  }

  template <typename T>
  void GetArrayFormatForPODArray(FMFormatMode mode, const T* rv, ndx_t count, ArrayFormatInfo &info) {
    info = GetArrayFormatForPOD<T>(mode);
    // Check for float as int flag
    bool float_as_int = true;
    for (ndx_t i=0;i<count;i++) {
      if (!isIntegerLike(rv[i])) {
	float_as_int = false;
	break;
      } 
    }
    info.floatasint = float_as_int;
    if (!float_as_int)
      ComputeScaleFactor(rv,count,info);
    else
      info.scalefact = 1.0;
    if (info.floatasint) {
      info.total_width = computeMaximumEntryWidth(rv,count);
      info.number_width = computeMaximumEntryWidth(rv,count);
    }
    return;
  }

  template void GetArrayFormatForPODArray(FMFormatMode mode, const double* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const float* rv, ndx_t count, ArrayFormatInfo &info);
  //  template void GetArrayFormatForPODArray(FMFormatMode mode, const ndx_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const uint64_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const int64_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const uint32_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const int32_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const uint16_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const int16_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const uint8_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const int8_t* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const char* rv, ndx_t count, ArrayFormatInfo &info);

  template <typename T>
  void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<T>* rv, ndx_t count, ArrayFormatInfo &info) {
    // Check for all real case
    bool all_real_case = true;
    for (ndx_t i=0;i<count;i++)
      if (rv[i].i != 0) {
	all_real_case = false;
	break;
      }
    bool all_imag_case = true;
    for (ndx_t i=0;i<count;i++)
      if (rv[i].r != 0) {
	all_imag_case = false;
	break;
      }
    if (all_real_case)
      info = GetArrayFormatForPOD<T>(mode);
    else if (all_imag_case) {
      info = GetArrayFormatForPOD<T>(mode);
      info.total_width += 1;
    } else {
      info = GetArrayFormatForPOD<T>(mode);
      info.total_width = info.number_width*2+5;
    }
    info.pure_real = all_real_case;
    info.pure_imag = all_imag_case;
    // Check for float as int flag
    bool float_as_int = true;
    for (ndx_t i=0;i<count;i++) {
      if (!isIntegerLike(rv[i].r) || !isIntegerLike(rv[i].i)) {
	float_as_int = false;
	break;
      } 
    }
    info.floatasint = float_as_int;
    if (!float_as_int)
      ComputeScaleFactor(rv,count,info);
    else
      info.scalefact = 1.0;
    if (info.floatasint) {
      info.number_width = computeMaximumEntryWidth(rv,count);
      info.total_width = 2*info.number_width + 5;
    }
    if (info.pure_real) info.total_width = info.number_width;
    if (info.pure_imag) {
      if (info.floatasint) info.number_width++;
      info.total_width = info.number_width+2;
    }
    return;
  }

  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<double>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<float>* rv, ndx_t count, ArrayFormatInfo &info);
  //  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<ndx_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<uint64_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<int64_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<uint32_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<int32_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<uint16_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<int16_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<uint8_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<int8_t>* rv, ndx_t count, ArrayFormatInfo &info);
  template void GetArrayFormatForPODArray(FMFormatMode mode, const Complex<char>* rv, ndx_t count, ArrayFormatInfo &info);  

#if 0
  template <typename T>
  void PrintFormattedNumberToStream(TermIF *io, const ArrayFormatInfo &format, const T& val) {
    if (format.floatasint) {
      io->output("%*lld",format.number_width,(ndx_t)(val));
      return;
    }
    if (isnan(val)) {
      io->output("NaN");
      return;
    }
    if (!isfinite(val)) {
      if (val > 0)
	io->output("Inf");
      else
	io->output("-Inf");
      return;
    }
    if (val != 0)
      if (format.expformat)
	io->output("%*.*e",format.number_width,format.decimals,double(val));
      else
	io->output("%*.*f",format.number_width,format.decimals,double(val)/format.scalefact);
    else
      io->output("%*d",format.number_width,0);    
  }
#endif

  template <typename T>
  FMString FormattedNumber(const ArrayFormatInfo &format, const T& val) {
    char buffer[4096];
    if (format.floatasint) {
      sprintf(buffer,"%*lld",format.number_width,(ndx_t)(val));
      return FMString(buffer);
    }
    if (isnan(val)) {
      sprintf(buffer,"NaN");
      return FMString(buffer);
    }
    if (!isfinite(val)) {
      if (val > 0)
	sprintf(buffer,"Inf");
      else
	sprintf(buffer,"-Inf");
      return FMString(buffer);
    }
    if (val != 0)
      if (format.expformat)
	sprintf(buffer,"%*.*e",format.number_width,format.decimals,double(val));
      else
	sprintf(buffer,"%*.*f",format.number_width,format.decimals,double(val)/format.scalefact);
    else
      sprintf(buffer,"%*d",format.number_width,0);
    return FMString(buffer);
  }

  template FMString FormattedNumber(const ArrayFormatInfo &, const double& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const float& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const uint64_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const int64_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const uint32_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const int32_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const uint16_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const int16_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const uint8_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const int8_t& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const char& val);

  template <typename T>
  FMString FormattedNumber(const ArrayFormatInfo &format, const Complex<T>& val) {
    if (format.pure_real) {
      return FormattedNumber(format,val.r);
    }
    if (format.pure_imag) {
      return FormattedNumber(format,val.i) + FMString(" i");
    }
    if ((val.r == 0) && (val.i==0)) {
      return FMString("0").rightJustified(format.number_width).leftJustified(format.total_width);
    }
    FMString build = FormattedNumber(format,val.r);
    if ((!isfinite(val.i)) || (val.i >= 0)) {
      build += FMString(" + ");
      build += FormattedNumber(format,val.i).trimmed().append(" i").leftJustified(format.number_width+2);
    } else {
      build += FMString(" - ");
      build += FormattedNumber(format,-val.i).trimmed().append(" i").leftJustified(format.number_width+2);
    }
    return build;
  }

  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<double>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<float>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<uint64_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<int64_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<uint32_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<int32_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<uint16_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<int16_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<uint8_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<int8_t>& val);
  template FMString FormattedNumber(const ArrayFormatInfo &, const Complex<char>& val);

}
