/*
 * Copyright (c) 2003 Matteo Frigo
 * Copyright (c) 2003 Massachusetts Institute of Technology
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

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sat Jul  5 21:40:28 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_notw_c -simd -compact -variables 4 -n 6 -name n2fv_6 -with-ostride 2 -include n2f.h */

/*
 * This function contains 18 FP additions, 4 FP multiplications,
 * (or, 16 additions, 2 multiplications, 2 fused multiply/add),
 * 19 stack variables, and 12 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "n2f.h"

static void n2fv_6(const R *ri, const R *ii, R *ro, R *io, stride is, stride os, int v, int ivs, int ovs)
{
     DVK(KP866025403, +0.866025403784438646763723170752936183471402627);
     DVK(KP500000000, +0.500000000000000000000000000000000000000000000);
     int i;
     const R *xi;
     R *xo;
     xi = ri;
     xo = ro;
     BEGIN_SIMD();
     for (i = v; i > 0; i = i - VL, xi = xi + (VL * ivs), xo = xo + (VL * ovs)) {
	  V T3, Td, T6, Te, T9, Tf, Ta, Tg, T1, T2;
	  T1 = LD(&(xi[0]), ivs, &(xi[0]));
	  T2 = LD(&(xi[WS(is, 3)]), ivs, &(xi[WS(is, 1)]));
	  T3 = VSUB(T1, T2);
	  Td = VADD(T1, T2);
	  {
	       V T4, T5, T7, T8;
	       T4 = LD(&(xi[WS(is, 2)]), ivs, &(xi[0]));
	       T5 = LD(&(xi[WS(is, 5)]), ivs, &(xi[WS(is, 1)]));
	       T6 = VSUB(T4, T5);
	       Te = VADD(T4, T5);
	       T7 = LD(&(xi[WS(is, 4)]), ivs, &(xi[0]));
	       T8 = LD(&(xi[WS(is, 1)]), ivs, &(xi[WS(is, 1)]));
	       T9 = VSUB(T7, T8);
	       Tf = VADD(T7, T8);
	  }
	  Ta = VADD(T6, T9);
	  Tg = VADD(Te, Tf);
	  ST(&(xo[6]), VADD(T3, Ta), ovs, &(xo[2]));
	  ST(&(xo[0]), VADD(Td, Tg), ovs, &(xo[0]));
	  {
	       V Tb, Tc, Th, Ti;
	       Tb = VFNMS(LDK(KP500000000), Ta, T3);
	       Tc = VBYI(VMUL(LDK(KP866025403), VSUB(T9, T6)));
	       ST(&(xo[10]), VSUB(Tb, Tc), ovs, &(xo[2]));
	       ST(&(xo[2]), VADD(Tb, Tc), ovs, &(xo[2]));
	       Th = VFNMS(LDK(KP500000000), Tg, Td);
	       Ti = VBYI(VMUL(LDK(KP866025403), VSUB(Tf, Te)));
	       ST(&(xo[4]), VSUB(Th, Ti), ovs, &(xo[0]));
	       ST(&(xo[8]), VADD(Th, Ti), ovs, &(xo[0]));
	  }
     }
     END_SIMD();
}

static const kdft_desc desc = { 6, "n2fv_6", {16, 2, 2, 0}, &GENUS, 0, 2, 0, 0 };
void X(codelet_n2fv_6) (planner *p) {
     X(kdft_register) (p, n2fv_6, &desc);
}
