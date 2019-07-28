/*
 Name:		main.ino
 Created:	8/8/2016 1:26:22 PM
 Author:	idans
*/

// the setup function runs once when you press reset or power the board

#include <Experiment_42_Sensor1.h>
#include <Experiment_42_Sensor2.h>
#include <child_detect.h>
#include <fft.h>
#include <math.h>
#define SEGMENT_Len 1024
// #define FFT_SIZE 4096 should be defined in SplitRadixRealP.h
#define WinLen 4*SEGMENT_Len
#define HYSTLVL 5

COMPLEX W[FFT_SIZE];
void setup() {
	fft_init(W);
	Serial.begin(115200); 
	pinMode(22, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(28, OUTPUT);
	pinMode(30, OUTPUT);
	pinMode(32, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	static bool kidincar = false;
	bool flag;
	static int kidincnt = 0;
	double rf = 0;
	int sensor1[FFT_SIZE], sensor2[FFT_SIZE];
	load_data(sensor1, CH1);
	load_data(sensor2, CH2);
	flag = child_detection(sensor1, sensor2, &rf, W);
	if (flag) kidincnt++;
	else kidincnt--;
	if (kidincnt < 0) {
		kidincnt = 0;
		kidincar = false;
	}
	else if (kidincnt > HYSTLVL) {
		kidincnt = HYSTLVL;
		kidincar = true;
	}
	make_action(kidincar);

}

void load_data(int dest[], double source[]) {
	static int i = 0;
	double factor = 16383.0;
	for (int j = 0; j < (FFT_SIZE / 2 ); j++) {
		dest[j] = dest[j + FFT_SIZE / 2];
	}
	for (int j = 0; j < FFT_SIZE/2; j++) {
		dest[j + (FFT_SIZE / 2)] = factor*source[i + j];
	}
}

void make_action(bool act) {
	digitalWrite(22, act);
	digitalWrite(24, act);
	digitalWrite(28, act);
	digitalWrite(30, act);
	digitalWrite(32, act);
}
