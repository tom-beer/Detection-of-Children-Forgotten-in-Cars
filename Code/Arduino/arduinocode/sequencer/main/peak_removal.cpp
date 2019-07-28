#include "child_detect.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h> 

float avrage(float input[], int size);

void peak_removal(float* in_array) {
	float mean, amplimit;
	mean = avrage(in_array, FFT_SIZE);
	// subtract mean !!!!!!!!!
	amplimit = 10 * median(in_array);
	for (int i = 0; i < FFT_SIZE; i++) {
		if (in_array[i] > amplimit) in_array[i] = amplimit;
		else if (in_array[i] < -1 * amplimit) in_array[i] = -1 * amplimit;
	}
}


float avrage(const float input[], int size) {
	// 1d vector avraging 
	float sum, avg;
	for (int i = 0; i < size; i++) {
		sum  = sum + input[i];
	}
	avg = sum / size;
	return(avg);
}

float median(const float in_array[], const int size) {
	float* values = (float*)malloc((size*sizeof(float)));
	float median;
	for (int i = 0; i < FFT_SIZE; i++){
	values[i] = abs(in_array[i]);
}
	qsort(values , size, sizeof(float), cmpfunc);// check qsort definition
	median = values[size / 2];
	free(values);
	return median;
}

int cmpfunc(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}
