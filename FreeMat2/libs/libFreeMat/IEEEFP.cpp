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
