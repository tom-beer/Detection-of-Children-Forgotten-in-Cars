#include "fft.h"



#define len 4096
struct COMPLEX w[len];
void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8,LOW); 
  Serial.begin(115200);
  fft_init(w);
}

void loop() {
  static int flag = 1;
  short vector[len] = {0};
  short FFT_O[len] = {0};
  float s_energy;
  int time0, duration;
  float dt=1/len;
  if (flag) {
    for (int i = 0; i < len; i++) {
      vector[i] =1024+short( 1024.0 * sin(2.0 * PI * 4.0 *i/1000.0 ));
      delay(1);
    }
    digitalWrite(8,HIGH); 
    s_energy = FFT(vector, FFT_O, 1, w);
    digitalWrite(8,LOW); 
    for (int i = 0; i < len; i++) {
      Serial.println(String(vector[i])+"  "+String(FFT_O[i]));
    }
    Serial.println("signal energy=" + String(s_energy));
    Serial.println("duration = " + String(duration) + "usec");
  }
  flag =0;
}

