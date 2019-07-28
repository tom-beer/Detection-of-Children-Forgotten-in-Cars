# define _CRT_SECURE_NO_WARNINGS
# include "Header.h"
# include "Wavelet.h"
# include <stdio.h>


int main()
{
	#define WINDOW_LENGTH 4096  
	#define NUM_OF_COEFFS 250	
	double rawSignal[WINDOW_LENGTH] = { 0.0 };
	double ldaCoeffs[NUM_OF_COEFFS+1]  = { 0.0 };
	int i = 0;
	double *allWaveletCoeffs;
	double waveletCoeffs[NUM_OF_COEFFS+1] = { 0.0 };
	double ldaRes = 0.0;
	

    FILE *sigFile;
	sigFile = fopen("sigg.txt", "r");
	FILE *ldaFile;
	ldaFile = fopen("ldaCoeffs.txt", "r");


    for (i = 0; i < WINDOW_LENGTH; i++)
    {
        fscanf(sigFile, "%lf", &rawSignal[i]);
    }

	allWaveletCoeffs = daub4_transform(WINDOW_LENGTH, rawSignal);
	for (i = 0; i < NUM_OF_COEFFS; i++)
	{
		waveletCoeffs[i] = allWaveletCoeffs[i];
	}
	
	for (i = 0; i < NUM_OF_COEFFS + 1; i++)
	{
		fscanf(ldaFile, "%lf", &ldaCoeffs[i]);
	}

	ldaRes = ldaCoeffs[NUM_OF_COEFFS];
	for (i = 0; i < NUM_OF_COEFFS; i++)
	{
		ldaRes = ldaRes + ldaCoeffs[i] * waveletCoeffs[i];
	}
	printf("rawSignal[0] is: %f\n", rawSignal[0]);
	printf("waveletCoeffs[0] is: %f\n", waveletCoeffs[0]); 
	printf("ldaRes is: %f\n", ldaRes);
	


	if (ldaRes > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
