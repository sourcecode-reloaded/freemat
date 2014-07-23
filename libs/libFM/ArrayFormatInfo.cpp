#include "ArrayFormatInfo.hpp"
#include "Complex.hpp"
#include "Exception.hpp"

namespace FM
{
//Visually Tuned
template <>
ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode formatMode, float) {
  switch (formatMode)
    {
    case format_short:
      return ArrayFormatInfo(9,false,4);
    case format_long:
      return ArrayFormatInfo(11,false,7);
    case format_short_e:
      return ArrayFormatInfo(11,false,4,true);
    case format_long_e:
      return ArrayFormatInfo(14,false,7,true);
    default:
      throw Exception("Unhandled format type");
    }
}

//Visually Tuned
template <>
ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode formatMode, double) {
  switch (formatMode)
    {
    case format_short:
      return ArrayFormatInfo(9,false,4);
    case format_long:
      return ArrayFormatInfo(18,false,14);
    case format_short_e:
      return ArrayFormatInfo(11,false,4,true);
    case format_long_e:
      return ArrayFormatInfo(21,false,14,true);
    default:
      throw Exception("Unhandled format type");
    }
}


//Visually Tuned
template <>
ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode formatMode, Complex<float>) {
  switch (formatMode)
    {
    case format_short:
      return ArrayFormatInfo(20,false,4);
    case format_long:
      return ArrayFormatInfo(24,false,7);
    case format_short_e:
      return ArrayFormatInfo(26,false,4,true);
    case format_long_e:
      return ArrayFormatInfo(32,false,7,true);
    default:
      throw Exception("Unhandled format type");
    }
}

//Visually Tuned
template <>
ArrayFormatInfo GetArrayFormatForPOD(FMFormatMode formatMode, Complex<double>) {
  switch (formatMode)
    {
    case format_short:
      return ArrayFormatInfo(20,false,4);
    case format_long:
      return ArrayFormatInfo(40,false,14);
    case format_short_e:
      return ArrayFormatInfo(26,false,4,true);
    case format_long_e:
      return ArrayFormatInfo(46,false,14,true);
    default:
      throw Exception("Unhandled format type");
    }
}

}
