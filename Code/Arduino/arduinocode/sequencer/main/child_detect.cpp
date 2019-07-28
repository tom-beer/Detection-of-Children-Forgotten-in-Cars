#include "child_detect.h" 
#include <math.h>
#include <fft.h>
bool child_detection(int sensor1[], int sensor2[], double* rf, COMPLEX w[]) {
	float fener1, fener2;
	double temp;
	short abs_freq[FFT_SIZE];
	peak_removal((float*)sensor1);
	peak_removal((float*)sensor2);
	fener1 = FFT((short*)sensor1, abs_freq, 0, w);
	fener2 = FFT((short*)sensor2, abs_freq, 0, w);
	*rf = (double)fener1 / fener2;
	if (*rf < THRESHOLD) return false;
	return true; 
}