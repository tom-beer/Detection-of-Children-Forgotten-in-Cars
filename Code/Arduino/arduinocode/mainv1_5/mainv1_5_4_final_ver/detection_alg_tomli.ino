//#define NUM_OF_COEFFS 250
#define NUM_OF_COEFFS 150

bool child_detect_tomli(int sensor1[])
{
  float normSig[FFT_SIZE] = {0};
  dataConversion(sensor1, normSig);
  float allWaveletCoeffs[FFT_SIZE]; // 4096
  float waveletCoeffs[NUM_OF_COEFFS] = { 0.0 }; // only first 250

  int i = 0;
  float ldaRes = 0.0;
  daub4_transform(normSig, allWaveletCoeffs);
  for (i = 0; i < NUM_OF_COEFFS; i++)
  {
    waveletCoeffs[i] = allWaveletCoeffs[i];
  }
  ldaRes = ldaCoeffs[NUM_OF_COEFFS];
  for (i = 0; i < NUM_OF_COEFFS; i++)
  {
    ldaRes = ldaRes + ldaCoeffs[i] * waveletCoeffs[i];
  }
  if (ldaRes > 0)
    return true;
  else return false;
}


/*************************************************************************/
/*function name : daub4_transform
   inputs :  input_signal - raw data from sensor
             output_coeffs - array for storing result
   output : none
   description : this function calculates the wavelet transform coefficients
 **************************************************************************/
void daub4_transform (float input_signal[FFT_SIZE], float output_coeffs[FFT_SIZE])
{
  float z[FFT_SIZE] = { 0.0 };
  float c[4] = {
    0.4829629131445341,
    0.8365163037378079,
    0.2241438680420133,
    -0.1294095225512603
  };
  int i;
  int j;
  int j0;
  int j1;
  int j2;
  int j3;
  int m;
  float temp;
  for ( i = 0; i < FFT_SIZE; i++ )
  {
    output_coeffs[i] = input_signal[i];
  }

  m = FFT_SIZE;
  while ( 4 <= m )
  {
    i = 0;
    for ( j = 0; j < m - 1; j = j + 2 )
    {
      j0 = i4_wrap ( j,     0, m - 1 );
      j1 = i4_wrap ( j + 1, 0, m - 1 );
      j2 = i4_wrap ( j + 2, 0, m - 1 );
      j3 = i4_wrap ( j + 3, 0, m - 1 );

      z[i]     = c[0] * output_coeffs[j0] + c[1] * output_coeffs[j1] + c[2] * output_coeffs[j2] + c[3] * output_coeffs[j3];
      z[i + m / 2] = c[3] * output_coeffs[j0] - c[2] * output_coeffs[j1] + c[1] * output_coeffs[j2] - c[0] * output_coeffs[j3];
      i = i + 1;
      //Serial.println(z[i]);
    }

    for ( i = 0; i < m; i++ )
    {
      //temp = z[i]; // the only assignment that works
      //Serial.println(m);
      output_coeffs[i] = z[i]; // what we actually need
      //Serial.println(temp);
      //Serial.println(z[i]);
    }
    m = m / 2;
  }
}

/*************************************************************************/
/*function name : i4_wrap
   inputs : ival  - input number
            ilo   - lower bound
            jlo   - upper bound
   output : value - wrapped version of ival
   description : This function wraps input to lie between specified bounds
 **************************************************************************/
int i4_wrap ( int ival, int ilo, int ihi )

{
  int jhi;
  int jlo;
  int value;
  int wide;

  jlo = min( ilo, ihi );
  jhi = max( ilo, ihi );

  wide = jhi + 1 - jlo;

  if ( wide == 1 ) {
    value = jlo;
  }
  else             {
    value = jlo + ((ival - jlo) % wide);
  }

  return value;
}

/*************************************************************************/
/* takes sensor1 data, and performs a simple manipulation on it:
   1. convert to float
   2. normalize - substract mean and divide by max value
**************************************************************************/
void dataConversion(int sensor1[], float normSig[])
{
  float maxVal = 0;
  float meanVal = 0;
  float sum = 0;

  for ( int i = 0; i < FFT_SIZE; i++ )
  {
    normSig[i] = (float)sensor1[i];
    sum += normSig[i];

    if ( abs(normSig[i]) > maxVal )
      maxVal = abs(normSig[i]);
  }

  meanVal = sum / FFT_SIZE;
  for ( int i = 0; i < FFT_SIZE; i++ )
  {
    normSig[i] = (normSig[i] - meanVal) / maxVal;
  }
}
