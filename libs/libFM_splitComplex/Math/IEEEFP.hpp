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
#ifndef __IEEEFP_HPP__
#define __IEEEFP_HPP__

#include <float.h>
#include <math.h>

#if defined(_MSC_VERPO )

extern "C"{
	__declspec( dllexport ) double rint (double x);
	__declspec( dllexport ) float rintf (float x);
	__declspec( dllexport ) double log1p(double x);
	__declspec( dllexport ) double expm1 (double x);
//        __declspec( dllexport ) double nextafter( double x, double y )  { return _nextafter( x, y ); }
	__declspec( dllexport ) float nextafterf (float x, float y);
	__declspec( dllexport ) float log1pf(float x);
	__declspec( dllexport ) float expm1f(float x);
	__declspec( dllexport ) float asinhf(float x);
	__declspec( dllexport ) double asinh(double x);
	__declspec( dllexport ) double acosh (double x);
	__declspec( dllexport ) float acoshf (float x);
	__declspec( dllexport ) double atanh(double x);
	__declspec( dllexport ) float atanhf (float x);
}
#define nextafter _nextafter
#endif

//returns distance to the next nearest long double value
inline long double fepsl(long double x) {
	if (x >= 0)
		return nextafterl(x, LDBL_MAX) - x;
	else
		return x - nextafterl(x, -LDBL_MAX);
}

//returns distance to the next nearest double value
inline double feps( double x ){
  if (x >= 0)
    return nextafter( x , DBL_MAX ) - x;
  else
    return x - nextafter( x, -DBL_MAX);
}

//returns distance to the next nearest float value
inline float fepsf( float x ){
  if (x >= 0)
    return nextafterf( x , FLT_MAX ) - x;
  else
    return x - nextafterf(x, -FLT_MAX);
}

#endif
