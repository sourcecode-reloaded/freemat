#include "FloatType.hpp"
#include "TermIF.hpp"
#include "Complex.hpp"
#include "ArrayFormatInfo.hpp"
#include "ThreadContext.hpp"
#include <math.h>

using namespace FM;

template <class T>
static void ComputeScaleFactor(const T* array, dim_t count, ArrayFormatInfo &format, bool isComplex = false) {
  T max_amplitude = 0;
  if (count == 0) return;
  if (format.expformat) return;
  bool finiteElementFound = false;
  for (dim_t i=0;i<count;i++) {
    if (std::isfinite(array[i]) && !finiteElementFound) {
      max_amplitude = array[i];
      finiteElementFound = true;
    }
    if ((std::isfinite(array[i])) && 
	(fabs((double) array[i]) > fabs((double) max_amplitude)))
      max_amplitude = array[i];
  }
  if (!finiteElementFound) return;
  T maxval = isComplex ? 100 : 1000;
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

template <typename T, FM::DataCode codeNum>
void FloatType<T,codeNum>::computeArrayFormatInfo(FMFormatMode mode, const Object &rp, ArrayFormatInfo &info) {
  if (!rp.isComplex()) {
    // FIXME
    //    if (IsInteger(rp.constReal<T>()))
    //      return ArrayFormatInfo(GetNominalWidthInteger(rp.constReal<T>()));
    info = GetArrayFormatForPOD<T>(mode);
    ComputeScaleFactor(this->ro(rp),rp.elementCount(),info,false);
    return;
  }
  info = GetArrayFormatForPOD<Complex<T> >(mode);
  ComputeScaleFactor(this->ro(rp),rp.elementCount()*2,info,true);
  return;  
}


static inline void printDouble(double val, TermIF *io, const ArrayFormatInfo &format, bool includeSign = false) {
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

template <typename T, FM::DataCode codeNum>
void FloatType<T,codeNum>::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset) {
  if (!a.isComplex())
    {
      const T* dp = this->ro(a);
      printDouble(dp[offset],Type::_ctxt->_io,format);
    }
  else
    {
      const Complex<T>* dp = this->roComplex(a);
      printDouble(dp[offset].r,Type::_ctxt->_io,format);
      printDouble(dp[offset].i,Type::_ctxt->_io,format,true);
      Type::_ctxt->_io->output("i");
    }
}

template class FloatType<float,TypeSingle>;
template class FloatType<double,TypeDouble>;
