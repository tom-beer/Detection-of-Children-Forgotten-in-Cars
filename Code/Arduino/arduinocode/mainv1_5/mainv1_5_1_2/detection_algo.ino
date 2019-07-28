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
  float kid_low, kid_high;
  double ratio, bands_ratio;
  calc_fft(kid_sensor, mag_kid);
  calc_fft(car_sensor, mag_car);
  energy_kid=calc_energy(3, 50, mag_kid); 
  energy_car=calc_energy(3, 50, mag_car);
  kid_low= calc_energy(3, 11, mag_kid);
  kid_high = calc_energy(26, 50, mag_kid);
  kid_low/=9;
  kid_high/=25;
  if (kid_high == 0) kid_high = 0.1; 
  bands_ratio=kid_low/kid_high;
  Serial.println(energy_kid, DEC);
  Serial.println(energy_car, DEC);
  if (energy_car == 0) energy_car = 0.1; // in case energy=0
  ratio = energy_kid / energy_car;
  Serial.println(ratio, DEC);
  Serial.println(bands_ratio, DEC);
  if (ratio > THRESHOLD && bands_ratio > 1) return true;
  else return false;
}
float calc_energy(int index_Low, int index_High, int mag[]){
    float energy = 0;
    for (int i = index_Low; i <= index_High; i++) { // i=0 is dc we dont need it.
    energy += mag[i];
  }
  return energy;
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
