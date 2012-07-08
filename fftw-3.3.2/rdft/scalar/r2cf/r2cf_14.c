/*
 * Copyright (c) 2003, 2007-11 Matteo Frigo
 * Copyright (c) 2003, 2007-11 Massachusetts Institute of Technology
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sat Apr 28 11:02:39 EDT 2012 */

#include "codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_r2cf.native -fma -reorder-insns -schedule-for-pipeline -compact -variables 4 -pipeline-latency 4 -n 14 -name r2cf_14 -include r2cf.h */

/*
 * This function contains 62 FP additions, 36 FP multiplications,
 * (or, 32 additions, 6 multiplications, 30 fused multiply/add),
 * 45 stack variables, 6 constants, and 28 memory accesses
 */
#include "r2cf.h"

static void r2cf_14(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP900968867, +0.900968867902419126236102319507445051165919162);
     DK(KP692021471, +0.692021471630095869627814897002069140197260599);
     DK(KP801937735, +0.801937735804838252472204639014890102331838324);
     DK(KP974927912, +0.974927912181823607018131682993931217232785801);
     DK(KP356895867, +0.356895867892209443894399510021300583399127187);
     DK(KP554958132, +0.554958132087371191422194871006410481067288862);
     {
	  INT i;
	  for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	       E TN, T3, TG, TQ, Tx, To, TH, Td, TD, TO, Tw, Ta, TL, Ty, TT;
	       E TI, Tg, Tr, Te, Tf, TP, TJ;
	       {
		    E Tl, TE, Tk, Tm;
		    {
			 E T1, T2, Ti, Tj;
			 T1 = R0[0];
			 T2 = R1[WS(rs, 3)];
			 Ti = R0[WS(rs, 3)];
			 Tj = R1[WS(rs, 6)];
			 Tl = R0[WS(rs, 4)];
			 TN = T1 + T2;
			 T3 = T1 - T2;
			 TE = Ti + Tj;
			 Tk = Ti - Tj;
			 Tm = R1[0];
		    }
		    {
			 E T7, TC, T6, T8;
			 {
			      E T4, T5, TF, Tn;
			      T4 = R0[WS(rs, 1)];
			      T5 = R1[WS(rs, 4)];
			      T7 = R0[WS(rs, 6)];
			      TF = Tl + Tm;
			      Tn = Tl - Tm;
			      TC = T4 + T5;
			      T6 = T4 - T5;
			      TG = TE - TF;
			      TQ = TE + TF;
			      Tx = Tn - Tk;
			      To = Tk + Tn;
			      T8 = R1[WS(rs, 2)];
			 }
			 {
			      E Tb, Tc, TB, T9;
			      Tb = R0[WS(rs, 2)];
			      Tc = R1[WS(rs, 5)];
			      Te = R0[WS(rs, 5)];
			      TB = T7 + T8;
			      T9 = T7 - T8;
			      TH = Tb + Tc;
			      Td = Tb - Tc;
			      TD = TB - TC;
			      TO = TC + TB;
			      Tw = T6 - T9;
			      Ta = T6 + T9;
			      Tf = R1[WS(rs, 1)];
			 }
		    }
	       }
	       TL = FNMS(KP554958132, TG, TD);
	       Ty = FNMS(KP554958132, Tx, Tw);
	       TT = FNMS(KP356895867, TO, TQ);
	       TI = Te + Tf;
	       Tg = Te - Tf;
	       Tr = FNMS(KP356895867, Ta, To);
	       TP = TH + TI;
	       TJ = TH - TI;
	       {
		    E Th, Tv, TK, TM;
		    Th = Td + Tg;
		    Tv = Tg - Td;
		    TK = FMA(KP554958132, TJ, TG);
		    TM = FMA(KP554958132, TD, TJ);
		    Ci[WS(csi, 6)] = KP974927912 * (FNMS(KP801937735, TL, TJ));
		    {
			 E TR, TV, TU, Tz;
			 TR = FNMS(KP356895867, TQ, TP);
			 TV = FNMS(KP356895867, TP, TO);
			 TU = FNMS(KP692021471, TT, TP);
			 Cr[0] = TN + TO + TP + TQ;
			 Tz = FMA(KP554958132, Tv, Tx);
			 Ci[WS(csi, 1)] = KP974927912 * (FNMS(KP801937735, Ty, Tv));
			 {
			      E TA, Ts, Tt, Tp;
			      TA = FMA(KP554958132, Tw, Tv);
			      Ts = FNMS(KP692021471, Tr, Th);
			      Tt = FNMS(KP356895867, Th, Ta);
			      Tp = FNMS(KP356895867, To, Th);
			      Cr[WS(csr, 7)] = T3 + Ta + Th + To;
			      Ci[WS(csi, 2)] = KP974927912 * (FMA(KP801937735, TK, TD));
			      Ci[WS(csi, 4)] = KP974927912 * (FNMS(KP801937735, TM, TG));
			      {
				   E TS, TW, Tu, Tq;
				   TS = FNMS(KP692021471, TR, TO);
				   TW = FNMS(KP692021471, TV, TQ);
				   Cr[WS(csr, 2)] = FNMS(KP900968867, TU, TN);
				   Ci[WS(csi, 5)] = KP974927912 * (FMA(KP801937735, Tz, Tw));
				   Ci[WS(csi, 3)] = KP974927912 * (FNMS(KP801937735, TA, Tx));
				   Cr[WS(csr, 5)] = FNMS(KP900968867, Ts, T3);
				   Tu = FNMS(KP692021471, Tt, To);
				   Tq = FNMS(KP692021471, Tp, Ta);
				   Cr[WS(csr, 4)] = FNMS(KP900968867, TS, TN);
				   Cr[WS(csr, 6)] = FNMS(KP900968867, TW, TN);
				   Cr[WS(csr, 1)] = FNMS(KP900968867, Tu, T3);
				   Cr[WS(csr, 3)] = FNMS(KP900968867, Tq, T3);
			      }
			 }
		    }
	       }
	  }
     }
}

static const kr2c_desc desc = { 14, "r2cf_14", {32, 6, 30, 0}, &GENUS };

void X(codelet_r2cf_14) (planner *p) {
     X(kr2c_register) (p, r2cf_14, &desc);
}

#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_r2cf.native -compact -variables 4 -pipeline-latency 4 -n 14 -name r2cf_14 -include r2cf.h */

/*
 * This function contains 62 FP additions, 36 FP multiplications,
 * (or, 38 additions, 12 multiplications, 24 fused multiply/add),
 * 29 stack variables, 6 constants, and 28 memory accesses
 */
#include "r2cf.h"

static void r2cf_14(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP900968867, +0.900968867902419126236102319507445051165919162);
     DK(KP222520933, +0.222520933956314404288902564496794759466355569);
     DK(KP623489801, +0.623489801858733530525004884004239810632274731);
     DK(KP433883739, +0.433883739117558120475768332848358754609990728);
     DK(KP974927912, +0.974927912181823607018131682993931217232785801);
     DK(KP781831482, +0.781831482468029808708444526674057750232334519);
     {
	  INT i;
	  for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	       E T3, TB, T6, Tv, Tn, Ts, Tk, Tt, Td, Ty, T9, Tw, Tg, Tz, T1;
	       E T2;
	       T1 = R0[0];
	       T2 = R1[WS(rs, 3)];
	       T3 = T1 - T2;
	       TB = T1 + T2;
	       {
		    E T4, T5, Tl, Tm;
		    T4 = R0[WS(rs, 2)];
		    T5 = R1[WS(rs, 5)];
		    T6 = T4 - T5;
		    Tv = T4 + T5;
		    Tl = R0[WS(rs, 6)];
		    Tm = R1[WS(rs, 2)];
		    Tn = Tl - Tm;
		    Ts = Tl + Tm;
	       }
	       {
		    E Ti, Tj, Tb, Tc;
		    Ti = R0[WS(rs, 1)];
		    Tj = R1[WS(rs, 4)];
		    Tk = Ti - Tj;
		    Tt = Ti + Tj;
		    Tb = R0[WS(rs, 3)];
		    Tc = R1[WS(rs, 6)];
		    Td = Tb - Tc;
		    Ty = Tb + Tc;
	       }
	       {
		    E T7, T8, Te, Tf;
		    T7 = R0[WS(rs, 5)];
		    T8 = R1[WS(rs, 1)];
		    T9 = T7 - T8;
		    Tw = T7 + T8;
		    Te = R0[WS(rs, 4)];
		    Tf = R1[0];
		    Tg = Te - Tf;
		    Tz = Te + Tf;
	       }
	       {
		    E Tp, Tr, Tq, Ta, To, Th;
		    Tp = Tn - Tk;
		    Tr = Tg - Td;
		    Tq = T9 - T6;
		    Ci[WS(csi, 1)] = FMA(KP781831482, Tp, KP974927912 * Tq) + (KP433883739 * Tr);
		    Ci[WS(csi, 5)] = FMA(KP433883739, Tq, KP781831482 * Tr) - (KP974927912 * Tp);
		    Ci[WS(csi, 3)] = FMA(KP433883739, Tp, KP974927912 * Tr) - (KP781831482 * Tq);
		    Ta = T6 + T9;
		    To = Tk + Tn;
		    Th = Td + Tg;
		    Cr[WS(csr, 3)] = FMA(KP623489801, Ta, T3) + FNMA(KP222520933, Th, KP900968867 * To);
		    Cr[WS(csr, 7)] = T3 + To + Ta + Th;
		    Cr[WS(csr, 1)] = FMA(KP623489801, To, T3) + FNMA(KP900968867, Th, KP222520933 * Ta);
		    Cr[WS(csr, 5)] = FMA(KP623489801, Th, T3) + FNMA(KP900968867, Ta, KP222520933 * To);
	       }
	       {
		    E Tu, TA, Tx, TC, TE, TD;
		    Tu = Ts - Tt;
		    TA = Ty - Tz;
		    Tx = Tv - Tw;
		    Ci[WS(csi, 2)] = FMA(KP974927912, Tu, KP433883739 * Tx) + (KP781831482 * TA);
		    Ci[WS(csi, 6)] = FMA(KP974927912, Tx, KP433883739 * TA) - (KP781831482 * Tu);
		    Ci[WS(csi, 4)] = FNMS(KP781831482, Tx, KP974927912 * TA) - (KP433883739 * Tu);
		    TC = Tt + Ts;
		    TE = Tv + Tw;
		    TD = Ty + Tz;
		    Cr[WS(csr, 6)] = FMA(KP623489801, TC, TB) + FNMA(KP900968867, TD, KP222520933 * TE);
		    Cr[WS(csr, 2)] = FMA(KP623489801, TD, TB) + FNMA(KP900968867, TE, KP222520933 * TC);
		    Cr[WS(csr, 4)] = FMA(KP623489801, TE, TB) + FNMA(KP222520933, TD, KP900968867 * TC);
		    Cr[0] = TB + TC + TE + TD;
	       }
	  }
     }
}

static const kr2c_desc desc = { 14, "r2cf_14", {38, 12, 24, 0}, &GENUS };

void X(codelet_r2cf_14) (planner *p) {
     X(kr2c_register) (p, r2cf_14, &desc);
}

#endif				/* HAVE_FMA */
