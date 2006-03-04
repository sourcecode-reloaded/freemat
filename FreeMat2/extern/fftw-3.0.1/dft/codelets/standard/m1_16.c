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
/* Generated on Sat Jul  5 21:29:37 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_notw_noinline -compact -variables 4 -n 16 -name m1_16 -include n.h */

/*
 * This function contains 144 FP additions, 24 FP multiplications,
 * (or, 136 additions, 16 multiplications, 8 fused multiply/add),
 * 49 stack variables, and 64 memory accesses
 */
/*
 * Generator Id's : 
 * $Id$
 * $Id$
 * $Id$
 */

#include "n.h"

static void m1_16_0(const R *ri, const R *ii, R *ro, R *io, stride is, stride os)
{
     DK(KP382683432, +0.382683432365089771728459984030398866761344562);
     DK(KP923879532, +0.923879532511286756128183189396788286822416626);
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     {
	  E T7, T1R, T25, TC, TN, T1x, T1H, T1l, Tt, T22, T2h, T1b, T1g, T1E, T1Z;
	  E T1D, Te, T1S, T26, TJ, TQ, T1m, T1n, TT, Tm, T1X, T2g, T10, T15, T1B;
	  E T1U, T1A;
	  {
	       E T3, TL, Ty, T1k, T6, T1j, TB, TM;
	       {
		    E T1, T2, Tw, Tx;
		    T1 = ri[0];
		    T2 = ri[WS(is, 8)];
		    T3 = T1 + T2;
		    TL = T1 - T2;
		    Tw = ii[0];
		    Tx = ii[WS(is, 8)];
		    Ty = Tw + Tx;
		    T1k = Tw - Tx;
	       }
	       {
		    E T4, T5, Tz, TA;
		    T4 = ri[WS(is, 4)];
		    T5 = ri[WS(is, 12)];
		    T6 = T4 + T5;
		    T1j = T4 - T5;
		    Tz = ii[WS(is, 4)];
		    TA = ii[WS(is, 12)];
		    TB = Tz + TA;
		    TM = Tz - TA;
	       }
	       T7 = T3 + T6;
	       T1R = T3 - T6;
	       T25 = Ty - TB;
	       TC = Ty + TB;
	       TN = TL - TM;
	       T1x = TL + TM;
	       T1H = T1k - T1j;
	       T1l = T1j + T1k;
	  }
	  {
	       E Tp, T17, T1f, T20, Ts, T1c, T1a, T21;
	       {
		    E Tn, To, T1d, T1e;
		    Tn = ri[WS(is, 15)];
		    To = ri[WS(is, 7)];
		    Tp = Tn + To;
		    T17 = Tn - To;
		    T1d = ii[WS(is, 15)];
		    T1e = ii[WS(is, 7)];
		    T1f = T1d - T1e;
		    T20 = T1d + T1e;
	       }
	       {
		    E Tq, Tr, T18, T19;
		    Tq = ri[WS(is, 3)];
		    Tr = ri[WS(is, 11)];
		    Ts = Tq + Tr;
		    T1c = Tq - Tr;
		    T18 = ii[WS(is, 3)];
		    T19 = ii[WS(is, 11)];
		    T1a = T18 - T19;
		    T21 = T18 + T19;
	       }
	       Tt = Tp + Ts;
	       T22 = T20 - T21;
	       T2h = T20 + T21;
	       T1b = T17 - T1a;
	       T1g = T1c + T1f;
	       T1E = T1f - T1c;
	       T1Z = Tp - Ts;
	       T1D = T17 + T1a;
	  }
	  {
	       E Ta, TP, TF, TO, Td, TR, TI, TS;
	       {
		    E T8, T9, TD, TE;
		    T8 = ri[WS(is, 2)];
		    T9 = ri[WS(is, 10)];
		    Ta = T8 + T9;
		    TP = T8 - T9;
		    TD = ii[WS(is, 2)];
		    TE = ii[WS(is, 10)];
		    TF = TD + TE;
		    TO = TD - TE;
	       }
	       {
		    E Tb, Tc, TG, TH;
		    Tb = ri[WS(is, 14)];
		    Tc = ri[WS(is, 6)];
		    Td = Tb + Tc;
		    TR = Tb - Tc;
		    TG = ii[WS(is, 14)];
		    TH = ii[WS(is, 6)];
		    TI = TG + TH;
		    TS = TG - TH;
	       }
	       Te = Ta + Td;
	       T1S = TF - TI;
	       T26 = Td - Ta;
	       TJ = TF + TI;
	       TQ = TO - TP;
	       T1m = TR - TS;
	       T1n = TP + TO;
	       TT = TR + TS;
	  }
	  {
	       E Ti, T11, TZ, T1V, Tl, TW, T14, T1W;
	       {
		    E Tg, Th, TX, TY;
		    Tg = ri[WS(is, 1)];
		    Th = ri[WS(is, 9)];
		    Ti = Tg + Th;
		    T11 = Tg - Th;
		    TX = ii[WS(is, 1)];
		    TY = ii[WS(is, 9)];
		    TZ = TX - TY;
		    T1V = TX + TY;
	       }
	       {
		    E Tj, Tk, T12, T13;
		    Tj = ri[WS(is, 5)];
		    Tk = ri[WS(is, 13)];
		    Tl = Tj + Tk;
		    TW = Tj - Tk;
		    T12 = ii[WS(is, 5)];
		    T13 = ii[WS(is, 13)];
		    T14 = T12 - T13;
		    T1W = T12 + T13;
	       }
	       Tm = Ti + Tl;
	       T1X = T1V - T1W;
	       T2g = T1V + T1W;
	       T10 = TW + TZ;
	       T15 = T11 - T14;
	       T1B = T11 + T14;
	       T1U = Ti - Tl;
	       T1A = TZ - TW;
	  }
	  {
	       E Tf, Tu, T2j, T2k;
	       Tf = T7 + Te;
	       Tu = Tm + Tt;
	       ro[WS(os, 8)] = Tf - Tu;
	       ro[0] = Tf + Tu;
	       T2j = TC + TJ;
	       T2k = T2g + T2h;
	       io[WS(os, 8)] = T2j - T2k;
	       io[0] = T2j + T2k;
	  }
	  {
	       E Tv, TK, T2f, T2i;
	       Tv = Tt - Tm;
	       TK = TC - TJ;
	       io[WS(os, 4)] = Tv + TK;
	       io[WS(os, 12)] = TK - Tv;
	       T2f = T7 - Te;
	       T2i = T2g - T2h;
	       ro[WS(os, 12)] = T2f - T2i;
	       ro[WS(os, 4)] = T2f + T2i;
	  }
	  {
	       E T1T, T27, T24, T28, T1Y, T23;
	       T1T = T1R + T1S;
	       T27 = T25 - T26;
	       T1Y = T1U + T1X;
	       T23 = T1Z - T22;
	       T24 = KP707106781 * (T1Y + T23);
	       T28 = KP707106781 * (T23 - T1Y);
	       ro[WS(os, 10)] = T1T - T24;
	       io[WS(os, 6)] = T27 + T28;
	       ro[WS(os, 2)] = T1T + T24;
	       io[WS(os, 14)] = T27 - T28;
	  }
	  {
	       E T29, T2d, T2c, T2e, T2a, T2b;
	       T29 = T1R - T1S;
	       T2d = T26 + T25;
	       T2a = T1X - T1U;
	       T2b = T1Z + T22;
	       T2c = KP707106781 * (T2a - T2b);
	       T2e = KP707106781 * (T2a + T2b);
	       ro[WS(os, 14)] = T29 - T2c;
	       io[WS(os, 2)] = T2d + T2e;
	       ro[WS(os, 6)] = T29 + T2c;
	       io[WS(os, 10)] = T2d - T2e;
	  }
	  {
	       E TV, T1r, T1p, T1v, T1i, T1q, T1u, T1w, TU, T1o;
	       TU = KP707106781 * (TQ - TT);
	       TV = TN + TU;
	       T1r = TN - TU;
	       T1o = KP707106781 * (T1m - T1n);
	       T1p = T1l - T1o;
	       T1v = T1l + T1o;
	       {
		    E T16, T1h, T1s, T1t;
		    T16 = FMA(KP923879532, T10, KP382683432 * T15);
		    T1h = FNMS(KP923879532, T1g, KP382683432 * T1b);
		    T1i = T16 + T1h;
		    T1q = T1h - T16;
		    T1s = FNMS(KP923879532, T15, KP382683432 * T10);
		    T1t = FMA(KP382683432, T1g, KP923879532 * T1b);
		    T1u = T1s - T1t;
		    T1w = T1s + T1t;
	       }
	       ro[WS(os, 11)] = TV - T1i;
	       io[WS(os, 11)] = T1v - T1w;
	       ro[WS(os, 3)] = TV + T1i;
	       io[WS(os, 3)] = T1v + T1w;
	       io[WS(os, 15)] = T1p - T1q;
	       ro[WS(os, 15)] = T1r - T1u;
	       io[WS(os, 7)] = T1p + T1q;
	       ro[WS(os, 7)] = T1r + T1u;
	  }
	  {
	       E T1z, T1L, T1J, T1P, T1G, T1K, T1O, T1Q, T1y, T1I;
	       T1y = KP707106781 * (T1n + T1m);
	       T1z = T1x + T1y;
	       T1L = T1x - T1y;
	       T1I = KP707106781 * (TQ + TT);
	       T1J = T1H - T1I;
	       T1P = T1H + T1I;
	       {
		    E T1C, T1F, T1M, T1N;
		    T1C = FMA(KP382683432, T1A, KP923879532 * T1B);
		    T1F = FNMS(KP382683432, T1E, KP923879532 * T1D);
		    T1G = T1C + T1F;
		    T1K = T1F - T1C;
		    T1M = FNMS(KP382683432, T1B, KP923879532 * T1A);
		    T1N = FMA(KP923879532, T1E, KP382683432 * T1D);
		    T1O = T1M - T1N;
		    T1Q = T1M + T1N;
	       }
	       ro[WS(os, 9)] = T1z - T1G;
	       io[WS(os, 9)] = T1P - T1Q;
	       ro[WS(os, 1)] = T1z + T1G;
	       io[WS(os, 1)] = T1P + T1Q;
	       io[WS(os, 13)] = T1J - T1K;
	       ro[WS(os, 13)] = T1L - T1O;
	       io[WS(os, 5)] = T1J + T1K;
	       ro[WS(os, 5)] = T1L + T1O;
	  }
     }
}

static void m1_16(const R *ri, const R *ii, R *ro, R *io, stride is, stride os, int v, int ivs, int ovs)
{
     int i;
     for (i = v; i > 0; i -= 1) {
	  m1_16_0(ri, ii, ro, io, is, os);
	  ri += ivs;
	  ii += ivs;
	  ro += ovs;
	  io += ovs;
     }
}

static const kdft_desc desc = { 16, "m1_16", {136, 16, 8, 0}, &GENUS, 0, 0, 0, 0 };
void X(codelet_m1_16) (planner *p) {
     X(kdft_register) (p, m1_16, &desc);
}
