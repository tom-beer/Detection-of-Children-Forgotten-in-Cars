/**********************************************************************************/
/* Function name : child detect over time
 *  inputs : 2 integer arrays with 2 sensors data one for child and one for car  
 *  output : bool withg the algorithm result '1' if kid was detected and '0' if 
 *  kid was naot not detected. 
 *  Description : the time domain algorithm calc energy in all sensor arrays then 
 *  the algorithm compare the energy levels  . if energies ratios is above treshold 
 *  return true otherwise false. no filtering is applay in time domain algorithm!
 **********************************************************************************/
bool child_detect_over_t(int kid_sensor[], int car_sensor[]) {
  float energy_kid = 0;
  float energy_car = 0;
  double ratio;
  energy_kid = calc_energy(0, 4096, kid_sensor);
  if (energy_kid < 50) return false;
  energy_car = calc_energy(0, 4096, car_sensor);
  if (energy_car == 0) energy_car = 1; // in case energy=0
  if ((energy_kid < ZERO_ENERGY) || (energy_car < ZERO_ENERGY)) {
    digitalWrite(WINDOW_FRONT,HIGH); 
  }
  else {
    digitalWrite(WINDOW_FRONT,LOW);
  }
  ratio = energy_kid / energy_car;
  if (ratio > THRESHOLD ) return true;
  else return false;
}
