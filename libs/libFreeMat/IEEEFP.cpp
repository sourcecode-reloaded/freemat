/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "IEEEFP.hpp"

static bool endianDetected = false;
static bool bigEndian = false;

union lc_t {
  long l;
  char c[sizeof (long)];
} u;

void CheckBigEndian() {
  u.l = 1;
  endianDetected = true;
  bigEndian = (u.c[sizeof(long) - 1] == 1);
  
}

bool IsInfinite(float t) {
  union {
    float f;
    unsigned int i;
  } u;             
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) == 0))
    return true;
  return false;
}

bool IsInfinite(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;
  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) == 0) && (u.i[0] == 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) == 0) && (u.i[1] == 0)))
      return true;
  }
  return false;

}

bool IsInfinite(int8) {return false;}
bool IsInfinite(int16) {return false;}
bool IsInfinite(int32) {return false;}
bool IsInfinite(int64) {return false;}
bool IsInfinite(uint8) {return false;}
bool IsInfinite(uint16) {return false;}
bool IsInfinite(uint32) {return false;}
bool IsInfinite(uint64) {return false;}

bool IsFinite(int8) {return true;}
bool IsFinite(int16) {return true;}
bool IsFinite(int32) {return true;}
bool IsFinite(int64) {return true;}
bool IsFinite(uint8) {return true;}
bool IsFinite(uint16) {return true;}
bool IsFinite(uint32) {return true;}
bool IsFinite(uint64) {return true;}

bool IsNaN(int32 t) {
  return true;
}

bool IsNaN(int64 t) {
  return true;
}

bool IsNaN(uint32 t) {
  return true;
}

bool IsNaN(uint64 t) {
  return true;
}

bool IsNaN(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) != 0))
    return true; 
  return false;
}

bool IsNaN(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;

  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) != 0) || (u.i[0] != 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) != 0) || (u.i[1] != 0)))
      return true;
  }
  return false;
}

bool IsFinite(float t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}

bool IsFinite(double t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}

double NaN() {
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7fC00000;
  return u.f;
}

double Inf() {
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7f800000;
  return u.f;
}

QString ToHexString(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  u.f = t;
  return QString("%1").arg(uint(u.i),int(8),int(16),QChar('0'));
}

QString ToHexString(double t) {
  union {
    double f;
    unsigned int i[2];
  } u;
  u.f = t;
  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) 
    return QString("%1%2").arg(uint(u.i[1]),int(8),int(16),QChar('0')).arg(uint(u.i[0]),int(8),int(16),QChar('0'));
  else
    return QString("%1%2").arg(uint(u.i[0]),int(8),int(16),QChar('0')).arg(uint(u.i[1]),int(8),int(16),QChar('0'));
}


#if defined(_MSC_VER )

#include <math.h>

extern "C" { 
__declspec( dllexport ) double rint (double x)
{
  __asm{
	  fld x
	  frndint
	  fstp x
  };
  return x;
}

//VC doesn't define log1p so we borrow the definition from libm (mingw)
__declspec( dllexport ) double log1p(double x){

	const double limit = 0.29;
	const double one = 1.0;

	__asm{
	/* BASED ON log1p.S from mingw
	 * Written by J.T. Conklin <jtc@netbsd.org>.
	 * Public domain.
	 * Removed header file dependency for use in libmingwex.a by
	 *   Danny Smith <dannysmith@users.sourceforge.net>
	 */

			/* The fyl2xp1 can only be used for values in
			   -1 + sqrt(2) / 2 <= x <= 1 - sqrt(2) / 2
			   0.29 is a safe value.
			 */
			fldln2
			fld     x //  [esp+4]
			fxam
			fnstsw	ax
			fld        st
			sahf
			jc        l3        // in case x is NaN or �Inf

	l4:      
			fabs
			fcomp        limit
			fnstsw	ax
			sahf
			jc        l2
			fadd        one
			fyl2x
			fstp x
			jmp l5

	l2:      
			fyl2xp1
			fstp x
			jmp l5

	l3:      
			jp        l4        // in case x is �Inf
			fstp        st(1)
			fstp        st(1)
	l5:
	};
return x;
}

const long double LOGE2L  = 6.9314718055994530941723E-1L;
const long double LOG2EL  = 1.4426950408889634073599E0L;
/* BASED ON log1p.S from mingw
 * Written 2005 by Gregory W. Chicares <chicares@cox.net>.
 * Adapted to double by Danny Smith <dannysmith@users.sourceforge.net>. 
 * Public domain.
 */
__declspec( dllexport ) double expm1 (double x)
{
  if (fabs(x) < LOGE2L)
    {
      x *= LOG2EL;
	  __asm{ 
		  fld x
		  f2xm1 
		  fstp x
	  };

      return x;
    }
  else
    return exp(x) - 1.0;
}

    __declspec( dllexport ) float nextafterf (float x, float y)
    {
      union
      {
	float f;
	unsigned int i;
      } u;
      if (_isnan (y) || _isnan (x))
	return x + y;
      if (x == y )
	 /* nextafter (0.0, -O.0) should return -0.0.  */
	 return y;
      u.f = x; 
      if (x == 0.0F)
	{
	  u.i = 1;
	  return y > 0.0F ? u.f : -u.f;
	}
      if (((x > 0.0F) ^ (y > x)) == 0)
	u.i++;
      else
	u.i--;
      return u.f;
    }
}
#endif 
