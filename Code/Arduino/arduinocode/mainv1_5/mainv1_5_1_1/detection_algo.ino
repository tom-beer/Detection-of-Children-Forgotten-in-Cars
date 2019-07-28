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

bool child_detect(int kid_sensor[], int car_sensor[]) {
  int mag_kid[MIRROR], mag_car[MIRROR];
  float energy_kid = 0;
  float energy_car = 0;
  int indx_f = 20 * FFT_SIZE / 1000 + 1;
  double ratio;
  calc_fft(kid_sensor, mag_kid);
  calc_fft(car_sensor, mag_car);
  for (int i = 1; i < indx_f; i++) { // i=0 is dc we dont need it.
    energy_kid += mag_kid[i];
    energy_car += mag_car[i];
  }
  Serial.println(energy_kid, DEC);
  Serial.println(energy_car, DEC);
  if (energy_car == 0) energy_car = 0.1; // in case energy=0
  ratio = energy_kid / energy_car;
  Serial.println(ratio, DEC);
  if (ratio > THRESHOLD) return true;
  else return false;
}

void calc_fft(int sensor[], int mag[]) {
  mult_Hamming(sensor);
  radix.rev_bin( sensor, FFT_SIZE);
  radix.fft_split_radix_real( sensor, LOG2_FFT);
  radix.gain_Reset( sensor, LOG2_FFT - 1);
  radix.get_Magnit1(sensor , mag);
}

void mult_Hamming(int in_array[]) {
  for (int i = 0; i < FFT_SIZE; i++) {
    in_array[i] = mult_shft12((in_array[i] - 2047), Hamming[i]);
  }
}

inline int mult_shft12( int a, int b)
{
  return (( a  *  b )  >> 12);
}
