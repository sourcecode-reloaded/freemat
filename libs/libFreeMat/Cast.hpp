#ifndef __Cast_hpp__
#define __Cast_hpp__
#include "Types.hpp"

template <typename T, typename S>
inline T CastConvert(S val);

#define MacroBoolIn(outtype)			\
  template <>					\
  inline outtype CastConvert(bool val) {	\
    if (val)					\
      return 1;					\
    else					\
      return 0;					\
  }

MacroBoolIn(int8);
MacroBoolIn(int16);
MacroBoolIn(int32);
MacroBoolIn(int64);
MacroBoolIn(uint8);
MacroBoolIn(uint16);
MacroBoolIn(uint32);
MacroBoolIn(uint64);
MacroBoolIn(float);
MacroBoolIn(double);

#define MacroBoolOut(intype)			\
  template <>					\
  inline bool CastConvert(intype val) {		\
    if (val == 0) return false;			\
    return true;				\
  }

MacroBoolOut(int8);
MacroBoolOut(int16);
MacroBoolOut(int32);
MacroBoolOut(int64);
MacroBoolOut(uint8);
MacroBoolOut(uint16);
MacroBoolOut(uint32);
MacroBoolOut(uint64);
MacroBoolOut(float);
MacroBoolOut(double);

#define MacroCastLimitXform(outtype,intype,lowlimit,hilimit,roundfunc)	\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (val < lowlimit) return lowlimit;				\
    if (val > hilimit) return hilimit;					\
    return outtype(roundfunc(val));					\
  }

#define MacroCastLimitXformAll(source,round)				\
  MacroCastLimitXform(uint64,source,0,Q_UINT64_C(18446744073709551614),round); \
  MacroCastLimitXform(uint32,source,0,uint32(4294967295U),round);	\
  MacroCastLimitXform(uint16,source,0,uint16(65535),round);		\
  MacroCastLimitXform(uint8,source,0,uint8(255),round);			\
  MacroCastLimitXform(int64,source,Q_INT64_C(-9223372036854775807),Q_INT64_C(9223372036854775807),round); \
  MacroCastLimitXform(int32,source,int32(-2147483647),int32(2147483647),round);	\
  MacroCastLimitXform(int16,source,int16(-32767),int16(32767),round);	\
  MacroCastLimitXform(int8,source,int8(-127),int8(127),round);		

MacroCastLimitXformAll(double,rint);
MacroCastLimitXformAll(float,rintf);

#define MacroCastLimit(outtype,intype,lowlimit,hilimit)			\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (val < lowlimit) return lowlimit;				\
    if (val > hilimit) return hilimit;					\
    return outtype(val);						\
  }

#define MacroCastUpLimit(outtype,intype,hilimit)			\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (val > hilimit) return hilimit;					\
    return outtype(val);						\
  }

#define MacroCastPosLimit(outtype,intype)				\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (val < 0) return 0;						\
    return outtype(val);						\
  }

#define MacroCastNoOp(outtype,intype)		\
  template <>					\
  inline outtype CastConvert(intype val) {	\
    return outtype(val);			\
  }

#define MacroCastNoLimit(outtype,intype)		\
  template <>						\
  inline outtype CastConvert(intype val) {		\
    return outtype(val);				\
  }

MacroCastNoOp(uint64,uint64);
MacroCastUpLimit(uint32,uint64,uint32(4294967295U));			
MacroCastUpLimit(uint16,uint64,uint16(65535));			
MacroCastUpLimit(uint8,uint64,uint8(255));				
MacroCastUpLimit(int64,uint64,Q_INT64_C(9223372036854775807)); 
MacroCastUpLimit(int32,uint64,int32(2147483647));	
MacroCastUpLimit(int16,uint64,int16(32767));		
MacroCastUpLimit(int8,uint64,int8(127));

MacroCastNoLimit(uint64,uint32);
MacroCastNoOp(uint32,uint32);
MacroCastUpLimit(uint16,uint32,uint16(65535));			
MacroCastUpLimit(uint8,uint32,uint8(255));				
MacroCastNoLimit(int64,uint32);
MacroCastUpLimit(int32,uint32,int32(2147483647));	
MacroCastUpLimit(int16,uint32,int16(32767));		
MacroCastUpLimit(int8,uint32,int8(127));

MacroCastNoLimit(uint64,uint16);
MacroCastNoLimit(uint32,uint16);
MacroCastNoOp(uint16,uint16);
MacroCastUpLimit(uint8,uint16,uint8(255));				
MacroCastNoLimit(int64,uint16);
MacroCastNoLimit(int32,uint16);
MacroCastUpLimit(int16,uint16,int16(32767));		
MacroCastUpLimit(int8,uint16,int8(127));

MacroCastNoLimit(uint64,uint8);
MacroCastNoLimit(uint32,uint8);
MacroCastNoLimit(uint16,uint8);
MacroCastNoOp(uint8,uint8);
MacroCastNoLimit(int64,uint8);
MacroCastNoLimit(int32,uint8);
MacroCastNoLimit(int16,uint8);
MacroCastUpLimit(int8,uint8,int8(127));

MacroCastLimit(uint64,int64,0,Q_INT64_C(9223372036854775807));	
MacroCastLimit(uint32,int64,0,uint32(4294967295U));			
MacroCastLimit(uint16,int64,0,uint16(65535));			
MacroCastLimit(uint8,int64,0,uint8(255));				
MacroCastNoOp(int64,int64);
MacroCastLimit(int32,int64,int32(-2147483647),int32(2147483647));	
MacroCastLimit(int16,int64,int16(-32767),int16(32767));		
MacroCastLimit(int8,int64,int8(-127),int8(127));

MacroCastPosLimit(uint64,int32);	
MacroCastPosLimit(uint32,int32);
MacroCastLimit(uint16,int32,0,uint16(65535));			
MacroCastLimit(uint8,int32,0,uint8(255));				
MacroCastNoLimit(int64,int32);
MacroCastNoOp(int32,int32);
MacroCastLimit(int16,int32,int16(-32767),int16(32767));		
MacroCastLimit(int8,int32,int8(-127),int8(127));

MacroCastPosLimit(uint64,int16);	
MacroCastPosLimit(uint32,int16);			
MacroCastPosLimit(uint16,int16);
MacroCastLimit(uint8,int16,0,uint8(255));				
MacroCastNoLimit(int64,int16);
MacroCastNoLimit(int32,int16);
MacroCastNoOp(int16,int16);
MacroCastLimit(int8,int16,int8(-127),int8(127));

MacroCastPosLimit(uint64,int8);
MacroCastPosLimit(uint32,int8);
MacroCastPosLimit(uint16,int8);
MacroCastPosLimit(uint8,int8);
MacroCastNoLimit(int64,int8);
MacroCastNoLimit(int32,int8);
MacroCastNoLimit(int16,int8);
MacroCastNoOp(int8,int8);

MacroCastNoLimit(float,double);
MacroCastNoLimit(double,float);

MacroCastNoLimit(float,int8);
MacroCastNoLimit(float,int16);
MacroCastNoLimit(float,int32);
MacroCastNoLimit(float,int64);

MacroCastNoLimit(float,uint8);
MacroCastNoLimit(float,uint16);
MacroCastNoLimit(float,uint32);
MacroCastNoLimit(float,uint64);

MacroCastNoLimit(double,int8);
MacroCastNoLimit(double,int16);
MacroCastNoLimit(double,int32);
MacroCastNoLimit(double,int64);

MacroCastNoLimit(double,uint8);
MacroCastNoLimit(double,uint16);
MacroCastNoLimit(double,uint32);
MacroCastNoLimit(double,uint64);

MacroCastNoOp(double,double);
MacroCastNoOp(float,float);
MacroCastNoOp(QChar,QChar);
MacroCastNoOp(bool,bool);

template <>
inline bool CastConvert(QChar val) {
  return (val != QChar(0));
}

template <>
inline QChar CastConvert(bool val) {
  if (val) return QChar(1); 
  return QChar(0);
}

template <>
inline uint8 CastConvert(QChar val) {
  return uint8(val.toAscii());
}

template <>
inline int8 CastConvert(QChar val) {
  return int8(val.toAscii());
}

#define MacroCastFromQChar(outtype)			\
  template <>						\
  inline outtype CastConvert(QChar val) {		\
    return outtype(val.unicode());			\
  }

MacroCastFromQChar(int16);
MacroCastFromQChar(int32);
MacroCastFromQChar(int64);
MacroCastFromQChar(uint16);
MacroCastFromQChar(uint32);
MacroCastFromQChar(uint64);
MacroCastFromQChar(float);
MacroCastFromQChar(double);

#define MacroCastToQChar(intype)		\
  template <>					\
  inline QChar CastConvert(intype val) {	\
    return QChar(val);				\
  }

MacroCastToQChar(int8);
MacroCastToQChar(int16);
MacroCastToQChar(int32);
MacroCastToQChar(uint8);
MacroCastToQChar(uint16);
MacroCastToQChar(uint32);

#define MacroCastToQCharViaInt(intype)		\
  template <>					\
  inline QChar CastConvert(intype val) {	\
    return QChar(uint32(val));			\
  }

MacroCastToQCharViaInt(float);
MacroCastToQCharViaInt(double);
MacroCastToQCharViaInt(int64);
MacroCastToQCharViaInt(uint64);

#endif
