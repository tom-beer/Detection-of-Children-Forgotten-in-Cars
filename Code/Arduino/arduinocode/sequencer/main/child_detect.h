#pragma once
#ifndef child_detect_H
#define child_detect_H

#define FFT_SIZE 4096
#define THRESHOLD 5


bool child_detection(int sensor1[], int sensor2[], double* rf, COMPLEX w[]);
void peak_removal(float* in_array); 
#endif
