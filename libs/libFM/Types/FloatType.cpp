#include "FloatType.hpp"
#include "TermIF.hpp"
#include "Complex.hpp"
#include "ArrayFormatInfo.hpp"
#include "ThreadContext.hpp"
#include <math.h>

using namespace FM;

static void updateArrayFormatInfoWithAmplitude(ArrayFormatInfo &format, double max_amplitude, double maxval) {
  if (max_amplitude >= maxval)
    format.scalefact = pow(double(10.0),double(floor(log10(max_amplitude))));
  else if (max_amplitude <= -maxval)
    format.scalefact = pow(double(10.0),double(floor(log10(-max_amplitude))));
  else if ((max_amplitude <= .1) && (max_amplitude>0))
    format.scalefact = pow(double(10.0),double(floor(log10(max_amplitude))));
  else if ((max_amplitude >= -.1) && (max_amplitude<0))
    format.scalefact = pow(double(10.0),double(floor(log10(-max_amplitude))));
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
    if (std::isfinite(array[i]) && !finiteElementFound) {
      max_amplitude = array[i];
      finiteElementFound = true;
    }
    if ((std::isfinite(array[i])) && 
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
    if (std::isfinite(array[i].r) && !finiteElementFound) {
      max_amplitude = array[i].r;
      finiteElementFound = true;
    }
    if (std::isfinite(array[i].i) && !finiteElementFound) {
      max_amplitude = array[i].i;
      finiteElementFound = true;
    }
    if (std::isfinite(array[i].r) && fabs((double)array[i].r) > fabs((double)max_amplitude))
      max_amplitude = array[i].r;
    if (std::isfinite(array[i].i) && fabs((double)array[i].i) > fabs((double)max_amplitude))
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
  return (std::isfinite(t) &&
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
  return maxwidth_real+maxwidth_imag+1;
}

template <typename T>
void FloatType<T>::computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info) {
  info = GetArrayFormatForPOD<T>(mode);
  // Check for float as int flag
  bool float_as_int = true;
  const T* rv = this->ro(rp);
  for (ndx_t i=0;i<rp.count();i++) {
    if (!isIntegerLike(rv[i])) {
      float_as_int = false;
      break;
    } 
  }
  info.floatasint = float_as_int;
  if (!float_as_int)
    ComputeScaleFactor(this->ro(rp),rp.count(),info);
  else
    info.scalefact = 1.0;
  if (info.floatasint)
    info.width = computeMaximumEntryWidth(rv,rp.count());
  return;
}

static inline void printDouble(double val, TermIF *io, const ArrayFormatInfo &format, bool includeSign = false) {
  if (format.floatasint) {
    if (includeSign) {
      if (val >= 0)
	io->output("%+ *lld",format.width,(ndx_t)(val));
      else
	io->output("%- *lld",format.width,(ndx_t)(-val));
    }
    else
      io->output("%*lld",format.width,(ndx_t)(val));
    return;
  }
  if (isnan(val)) {
    if (includeSign)
      io->output(" + ");
    io->output("NaN");
    return;
  }
  if (!std::isfinite(val)) {
    if (val > 0)
      {
	if (includeSign) io->output(" + ");
	io->output("Inf");
      }
    else
      io->output("-Inf");
    return;
  }
  if (val != 0)
    if (format.expformat)
      {
	if (includeSign)
	  io->output("%+*.*e",format.width,format.decimals,double(val));
	else
	  io->output("%*.*e",format.width,format.decimals,double(val));
      }
    else
      {
	if (includeSign)
	  io->output("%+*.*f",format.width,format.decimals,double(val)/format.scalefact);
	else
	  io->output("%*.*f",format.width,format.decimals,double(val)/format.scalefact);
      }
  else
    {
      if (includeSign)
	io->output("%+*d",format.width,0);
      else
	io->output("%*d",format.width,0);
    }
}

template <class T>
static inline void printNumber(const T &val, TermIF *io, const ArrayFormatInfo &format) {
  if (format.floatasint) {
    io->output("%*lld",format.width,(ndx_t)(val));
    return;
  }
  if (isnan(val)) {
    io->output("NaN");
    return;
  }
  if (!std::isfinite(val)) {
    if (val > 0)
      io->output("Inf");
    else
      io->output("-Inf");
    return;
  }
  if (val != 0)
    if (format.expformat)
      io->output("%*.*e",format.width,format.decimals,double(val));
    else
      io->output("%*.*f",format.width,format.decimals,double(val)/format.scalefact);
  else
    io->output("%*d",format.width,0);
}


template <class T>
static inline void printNumber(const Complex<T> &val, TermIF *io, const ArrayFormatInfo &format) {
  char buffer[4096];
  if (format.floatasint) {
    sprintf(buffer,"%*lld%-+*lld",format.width/2-1,(ndx_t)(val.r),format.width/2-1,(ndx_t)(val.i));
    io->output(FMString(buffer).rightTrimmed().append('i').leftJustified(format.width));
    return;
  }
  if (format.expformat) {
    sprintf(buffer,"%*.*e%-+*.*e",
	    format.width/2-1,format.decimals,double(val.r),
	    format.width/2-1,format.decimals,double(val.i));
    io->output(FMString(buffer).rightTrimmed().append('i').leftJustified(format.width));
  } else {
    sprintf(buffer,"%*.*f%-+*.*f",
	    format.width/2-1,format.decimals,double(val.r)/format.scalefact,
	    format.width/2-1,format.decimals,double(val.i)/format.scalefact);
    io->output(FMString(buffer).rightTrimmed().append('i').leftJustified(format.width));
  }
}

template <typename T>
void FloatType<T>::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset) {
  printNumber(this->ro(a)[offset],Type::_ctxt->_io,format);
}

template <typename T>
bool FloatType<T>::isSymmetric(const Object &a) {
  if (!a.isSquare()) throw Exception("Cannot test non-square matrices for symmetry");
  ndx_t N = a.rows();
  const T* ap = this->ro(a);
  for (ndx_t i=0;i<N;i++)
    for (ndx_t j=0;j<i;j++)
      if (ap[i*N+j] != complex_conj(ap[j*N+i])) return false;
  return true;
}

template class FM::FloatType<float>;
template class FM::FloatType<double>;
template class FM::FloatType<Complex<float> >;
template class FM::FloatType<Complex<double> >;
