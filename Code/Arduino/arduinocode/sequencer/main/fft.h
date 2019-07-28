
#ifndef fft_H
#define fft_H

#include "Arduino.h"
//#include "hamming.h"
#include <math.h>


#define BUFFER_SIZE 256
//#define PI 3.14159265358979

struct COMPLEX { float real, imag; };

/*float iobuffer[BUFFER_SIZE];   		    //as input and output buffer
float x1[BUFFER_SIZE];         		    //intermediate buffer                 
short buffercount = 0;     	    //number of new samples in iobuffer         
short flag = 0;*/        		    //set to 1 by ISR when iobuffer full   
//struct COMPLEX w[BUFFER_SIZE];       		    //twiddle constants stored in w 
//struct COMPLEX samples[BUFFER_SIZE];   //primary working buffer  

void fft_init(COMPLEX W[]);
float FFT(short real_in[], short abs_freq[], short win,COMPLEX w[]);	    //FFT prototype

#endif                                      

