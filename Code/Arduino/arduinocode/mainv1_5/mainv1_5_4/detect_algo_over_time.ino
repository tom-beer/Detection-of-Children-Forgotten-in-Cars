bool child_detect_over_t(int kid_sensor[], int car_sensor[]) {
  float energy_kid = 0;
  float energy_car = 0;
  double ratio;
  energy_kid = calc_energy(0, 4096, kid_sensor);
  if (energy_kid < 50) return false;
  energy_car = calc_energy(0, 4096, car_sensor);
  Serial.println(energy_kid, DEC);
  Serial.println(energy_car, DEC);
  if (energy_car == 0) energy_car = 1; // in case energy=0
  ratio = energy_kid / energy_car;
  Serial.println(ratio, DEC);
  if (ratio > THRESHOLD ) return true;
  else return false;
}
