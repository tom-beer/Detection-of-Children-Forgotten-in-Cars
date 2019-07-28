/*
 *  original Fortran code by Sorensen; published in H.V. Sorensen, D.L. Jones,
 * M.T. Heideman, C.S. Burrus(1987)Real-valued fast fourier transform
 * algorithms.  IEEE Trans on Acoustics, Speech, & Signal Processing, 35,
 * 849-863.  Adapted to C by Bill Simpson, 1995  wsimpson@uwinnipeg.ca
 * derived from: static char RCSreal2herm_c[] =
 * "$Id: real2herm.c,v 1.1 1996/09/02 01:47:12 wedgingt Beta $";
 * further optimizations by Joerg Arndt  arndt@jjj.de
 * Adapted and optimized for Arduino DUE (32-bit cortex-3 uCPU w/o 
 * floating point coprocessor) by Anatoly Kuzmenko  k_anatoly@hotmail.com
 */

/*
 **************************   Split Radix Real FFT LIBRARY      *************************
 * http://coolarduino.wordpress.com
 *
 * Created for Arduino DUE & like boards, word size optimized for 12-bits data input.
 * 
 * FFT takes as input ANY size of inputs array 8, 16, 32, 64, 128, 256, 512, 1024, 2048.
 * Max. size 2048 defined by LUT. 
 *
 * Library may run on different platform, only variable declaration
 * type may need to be adjusted accordingly.
 *
 * Algorithm tested on Arduino DUE and IDE 1.5.6-r2 (Tested on Linux OS only).
 *
 * Timing results, in usec:
 * fft.2048
 * Hamng 905	Revb 765	SplitRR 4630	GainR 467	Sqrt 5414	Sqrt2 404
 * fft.1024
 * Hamng 453	Revb 383	SplitRR 2115	GainR 234	Sqrt 3208	Sqrt2 203
 *
 * 29-09-2014 One more level of optimization:
 *
 * fft.2048
 * Hamng 904	Revb 765	SplitRR 3479	GainR 465	Sqrt 6423	Sqrt2 404
 * fft.1024
 * Hamng 453	Revb 383	SplitRR 1589	GainR 234	Sqrt 3430	Sqrt2 204
 * 
 * There is two subfunctions for magnitude calculation, as you can see second one runs 
 * more than 10x times faster, but it is less accurate, error in worst case scenario
 * may reach 5 %. Approximation based on
 *
 * http://www.dspguru.com/dsp/tricks/magnitude-estimator
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * Copyright (C) 2014 Anatoly Kuzmenko.
 * All Rights Reserved.
 * 25 Sept. 2014 
 * k_anatoly@hotmail.com
 *********************************************************************************************
 */

#include "SplitRadixRealP.h"

SplitRadixRealP::SplitRadixRealP() {
}

inline int SplitRadixRealP::mult_shft( int a, int b)  
{
  return (( a  *  b )  >> 12);      
}

void SplitRadixRealP::get_Magnit1(int *fr, int *fo)
{	
   for (int  i = 1; i < MIRROR; i++) {
        int real = fr[i];
        int imag = 0;
            if (i) imag = fr[FFT_SIZE -i];
        int Magnit = (int) (sqrt((real * real) + (imag * imag)) + 0.5);
        fo[i] = Magnit;
     }
   fo[0] = fr[0];
}

void SplitRadixRealP::get_Magnit2(int *f_r, int *f_o)
{	
   const int16_t  alpha =  3881, beta = 1608;   
   for (int  i = 1; i < MIRROR; i++) {
      int tmp_M;
      int abs_R = abs(f_r[i]);
      int abs_I = 0;
          if (i) abs_I = abs(f_r[FFT_SIZE -i]);

      if (abs_R > abs_I) tmp_M = mult_shft( alpha, abs_R) + mult_shft( beta, abs_I);
      else               tmp_M = mult_shft( alpha, abs_I) + mult_shft( beta, abs_R);

      f_o[i] = tmp_M;
     }
   f_o[0] = f_r[0];
}

void SplitRadixRealP::gain_Reset(int *fr, int r_bit)
{
    for (int  i = 0; i < FFT_SIZE; i++) {
        int delit = fr[i];
        if ( delit  <  0 ) delit = ((delit >> r_bit) +1);
        else delit = delit >> r_bit;
        fr[i] = delit;
    }
}

void SplitRadixRealP::rev_bin( int *fr, int16_t fft_n)
{
    int m, mr, nn, l;
    int tr;

    mr = 0;
    nn = fft_n - 1;

    for (m=1; m<=nn; ++m) {
            l = fft_n;
         do {
             l >>= 1;
            } while (mr+l > nn);

            mr = (mr & (l-1)) + l;

        if (mr <= m)
             continue;
            tr = fr[m];
            fr[m] = fr[mr];
            fr[mr] = tr;
    }
}

inline void SplitRadixRealP::cmplx_mult( int c, int s, int x, int y, int &u, int &v)
{
  u = ((x*c) - (y*s)) >> 12;
  v = ((y*c) + (x*s)) >> 12;
}

inline void SplitRadixRealP::sumdiff(int &a, int &b)
{ int t = a - b; a += b; b = t; }

inline void SplitRadixRealP::sumdiff_r(int &a, int &b)
{ int t = b - a; a += b; b = t; }

inline void SplitRadixRealP::sumdiff(int a, int b, int &s, int &d)
{ s = a + b; d = a - b; }

inline void SplitRadixRealP::sumdiff3(int &a, int b, int &d)
{ d = a - b; a += b; }

inline void SplitRadixRealP::diffsum3_r(int a, int &b, int &s)
{ s = a + b; b -= a; }

void SplitRadixRealP::fft_split_radix_real( int *x, int ldn)
{
  const uint16_t n = (1UL << ldn);
  int * fini = x + n;

  int  id = 4;
  for (uint16_t ix = 0;  ix < n;)
  {
   for ( int * i0 = x + ix; i0 < fini; i0 += id)
       sumdiff( i0[0], i0[1]);
   ix = 2 * (id - 1);
   id *= 4;
  }

  uint16_t n2 = 2;
  uint16_t nn = n >> 1;
  while ( nn >>= 1 )
  {
        n2 <<= 1;
        uint16_t id = n2 << 1;
        uint16_t n4 = n2 >> 2;
        uint16_t n8 = n2 >> 3;

        uint16_t e = NWAVE / n2;

        uint16_t ix = 0;
        do
        {
            for ( int * i0 = x + ix; i0 < fini; i0 += id)
            {
                int * i1 = i0;
                int * i2 = i1 + n4;
                int * i3 = i2 + n4;
                int * i4 = i3 + n4;

          int t1;
                diffsum3_r((*i3), (*i4), t1);
                sumdiff3((*i1), t1, (*i3));

                if ( n4 != 1 )
                {
                    i1 += n8;
                    i2 += n8;
                    i3 += n8;
                    i4 += n8;

              int t2;
                    sumdiff((*i3), (*i4), t1, t2);
                    t1 = (-t1 * 2896) >> 12;
                    t2 = ( t2 * 2896) >> 12;
                    sumdiff(t1, (*i2), (*i4), (*i3));
                    sumdiff3((*i1), t2, (*i2));
                }
            }
        ix = (id << 1) - n2;
        id <<= 2;
        }   while ( ix < n );

        if (n8 >1) { // Optimization
            ix = 0;
            id = n2 << 1;
            do {
                for ( int * i0 = x + ix; i0 < fini; i0 += id) {
                    for (uint16_t j = 1; j < n8; j++) {
                         uint16_t a = j * e;
                         int cc1, ss1, cc3, ss3;
                            ss1 = Sinewave[    a];
                            cc1 = Sinewave[    a + NQUAT];
                            ss3 = Sinewave[3 * a];
                            cc3 = Sinewave[3 * a + NQUAT];
		            int * i1 = i0 + j;
		            int * i2 = i1 + n4;
		            int * i3 = i2 + n4;
		            int * i4 = i3 + n4;
		            int * i5 = i0 + n4 - j;
		            int * i6 = i5 + n4;
		            int * i7 = i6 + n4;
		            int * i8 = i7 + n4;
		            int t1, t2;
	                          cmplx_mult(cc1, ss1, (*i7), (*i3), t2, t1);
	                    int t3, t4;
	                          cmplx_mult(cc3, ss3, (*i8), (*i4), t4, t3);
	                          sumdiff(t2, t4);
	                          sumdiff(t2, (*i6), (*i8), (*i3));
	                          sumdiff_r(t1, t3);
	                          sumdiff(t3, (*i2), (*i4), (*i7));
	                          sumdiff3((*i1), t1, (*i6));
	                          diffsum3_r(t4, (*i5), (*i2));
	                    }
	                }
            ix = (id << 1) - n2;
            id <<= 2;
            }   while ( ix < n );
        }
    }
}
