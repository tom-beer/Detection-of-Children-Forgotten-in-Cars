//fft.c  C callable FFT function in C

#include "fft.h"
#include "hamming.h"
#include <math.h>



void fft_init(COMPLEX w[]) {
	short i;
	//struct COMPLEX w[BUFFER_SIZE];
	for (i = 0; i<BUFFER_SIZE; i++) {	    // set up twiddle constants in w 
		w[i].real = cos(2 * PI*i / 256.0); //Re component of twiddle constants
		w[i].imag = -sin(2 * PI*i / 256.0); //Im component of twiddle constants
	}
}




float FFT(short real_in[], short abs_freq[], short win,COMPLEX w[])      //input array of real samples   
{
	struct COMPLEX temp1, temp2, Y[256];             //temporary storage variables          
	int i, j, k;                       //loop counter variables               
	int upper_leg, lower_leg;   	    //index of upper/lower butterfly leg   
	int leg_diff;                    //difference between upper/lower leg   
	int num_stages = 0;              //number of FFT stages (iterations)
	int index, step_num;
	float Sig_en = 0;                 //index/step through twiddle constant

	for (i = 0; i < BUFFER_SIZE; i++) {
		if (win == 1) real_in[i] = real_in[i] * Wind[i];   			//convert input to float
		(Y[i]).real = (float)(real_in[i]);
		(Y[i]).imag = 0;

	}

	i = 1;                           //log(base2) of N points= # of stages  
	do
	{
		num_stages += 1;
		i = i * 2;
	} while (i != BUFFER_SIZE);	  // possible bug if BUFFER_SIZE is not radix-2
								  /* i=log10(BUFFER_SIZE)/log10(2);*/

	leg_diff = BUFFER_SIZE / 2; 		     	    //difference between upper&lower legs
	step_num = 256 / BUFFER_SIZE;   		     	    //step between values in twiddle.h              
	for (i = 0; i < num_stages; i++)  			//for (BUFFER_SIZE) -point FFT                 
	{
		index = 0;
		for (j = 0; j < leg_diff; j++)
		{
			for (upper_leg = j; upper_leg < BUFFER_SIZE; upper_leg += (2 * leg_diff))
			{
				lower_leg = upper_leg + leg_diff;
				temp1.real = (Y[upper_leg]).real + (Y[lower_leg]).real;
				temp1.imag = (Y[upper_leg]).imag + (Y[lower_leg]).imag;
				temp2.real = (Y[upper_leg]).real - (Y[lower_leg]).real;
				temp2.imag = (Y[upper_leg]).imag - (Y[lower_leg]).imag;
				(Y[lower_leg]).real = temp2.real*(w[index]).real
					- temp2.imag*(w[index]).imag;
				(Y[lower_leg]).imag = temp2.real*(w[index]).imag
					+ temp2.imag*(w[index]).real;
				(Y[upper_leg]).real = temp1.real;
				(Y[upper_leg]).imag = temp1.imag;
			}
			index += step_num;
		}
		leg_diff = leg_diff / 2;
		step_num *= 2;
	}
	j = 0;
	for (i = 1; i < (BUFFER_SIZE - 1); i++)     //bit reversal for resequencing data
	{
		k = BUFFER_SIZE / 2;
		while (k <= j)
		{
			j = j - k;
			k = k / 2;
		}
		j = j + k;
		if (i<j)
		{
			temp1.real = (Y[j]).real;
			temp1.imag = (Y[j]).imag;
			(Y[j]).real = (Y[i]).real;
			(Y[j]).imag = (Y[i]).imag;
			(Y[i]).real = temp1.real;
			(Y[i]).imag = temp1.imag;
		}
	}
	for (i = 0; i < BUFFER_SIZE; i++) {    			//compute magnitude
		abs_freq[i] = (short)(sqrt(Y[i].real*Y[i].real
			+ Y[i].imag*Y[i].imag) / 12); //alon why divide by12?
		Sig_en += (abs_freq[i]);
	}
	return Sig_en;
}


