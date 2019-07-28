/***********************************************************************/
/* all function in this file  run the frequency domain algorithm *******/
/* function name : child_detect
    inputs 2 integer 1-D array that contain samples from adc of 2 sensors
    output : a booleane var that indicate id a kid was detected
    description : this function calc fft for each sensor array when overlapping
    is half of window size (2048 samples)
    then energy is calculateed for 0.75<f<12.5hz (bins 3-50) and the energy levels
    between car sensor and kid sensor is compeared and the energy levels in kid
    sensor between low frequency 0.75- 2.7hz and high frequencies (6-12.5hz)
    if both ratios are higher then thersholds then return 1 otherwise return 0
*************************************************************************/
bool child_detect(int kid_sensor[], int car_sensor[]) {
  int mag_kid[MIRROR], mag_car[MIRROR];
  float energy_kid = 0;
  float energy_car = 0;
  float kid_low, kid_high;
  double ratio, bands_ratio;

  calc_fft(kid_sensor, mag_kid);
  calc_fft(car_sensor, mag_car);
  energy_kid = calc_energy(3, 50, mag_kid);
  energy_car = calc_energy(3, 50, mag_car);
  kid_low = calc_energy(3, 11, mag_kid);
  kid_high = calc_energy(26, 50, mag_kid);
  kid_low /= 9;
  kid_high /= 25;
  if (kid_high == 0) kid_high = 1;
  bands_ratio = kid_low / kid_high;
  sensor_err_check(energy_car, energy_kid);
  Serial.println(energy_kid);
  Serial.println(energy_car);
  ratio = energy_kid / energy_car;
  if (ratio > THRESHOLD ) return true;
  else return false;
}
/**************************************************************************/
/* function name : calc_energy
    input : interger array and two indexes
    output : calculated energy
    description : this function calculating signal energy over 1-D attary
    in a for loop between 2 indexes
    formula is Energy = sum(array[i]^2)
    this function is used by time domain and frequency domain algorithms
 *************************************************************************/
float calc_energy(int index_Low, int index_High, int mag[]) {
  float energy = 0;
  for (int i = index_Low; i <= index_High; i++) { // i=0 is dc we dont need it.
    energy += mag[i] * mag[i];
  }
  return energy;
}
/*************************************************************************/
/*function name : calc_fft
   inputs : 2 integer array one for sensor redout and one for returning magnitude
   output : none
   description : this function calc frequency magnitude using SplitRadixRealP libarary
   the fft is calc in integer form for input buffer.
 **************************************************************************/
void calc_fft(int sensor[], int mag[]) {
  int temp_array[FFT_SIZE] = {0} ;
  mult_Hamming(sensor, temp_array);
  radix.rev_bin( temp_array, FFT_SIZE);
  radix.fft_split_radix_real( temp_array, LOG2_FFT);
  radix.gain_Reset( temp_array, LOG2_FFT - 1);
  radix.get_Magnit1(temp_array , mag);
}

void mult_Hamming(int in_array[], int out_array[]) {
  for (int i = 0; i < FFT_SIZE; i++) {
    out_array[i] = mult_shft12(in_array[i], Hamming[i]);
  }
}

inline int mult_shft12( int a, int b)
{
  return (( a  *  b )  >> 12);
}

void histersys_check(short *cnt, bool *state) {
  if (*cnt < 0) {
    *cnt = 0;
    *state = false;
  }
  else if (*cnt > HISTLVL) {
    *cnt = HISTLVL;
    *state = true;
  }
}

/*****************************************************************************/
/* Function name



 *****************************************************************************/
void sensor_err_check(float ener_sensor1, float ener_sensor2) {
  static short sensors_error_cnt = 0;
  static bool sens_err = false;

  if ((ener_sensor1 < ZERO_ENERGY) || (ener_sensor2 < ZERO_ENERGY)) {
    sensors_error_cnt ++;
  }
  else {
    sensors_error_cnt --;
  }
  histersys_check(&sensors_error_cnt, &sens_err);
  if (sens_err) {
    digitalWrite(WINDOW_FRONT, HIGH);
  }
  else {
    digitalWrite(WINDOW_FRONT, LOW);
  }
}

