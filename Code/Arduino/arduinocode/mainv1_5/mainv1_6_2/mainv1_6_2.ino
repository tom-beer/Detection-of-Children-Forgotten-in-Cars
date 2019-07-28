#include <DS3231.h>
#include <SD.h>
#include <SPI.h>
#include <DueTimer.h>
#include <SplitRadixRealP.h>

//#define channel 0 /*sample channels AN2 AN3*/ /* odd channel number are for kid sensor!*/
#define channel 1 /*sample channels AN0 AN1*/

#define   SMP_RATE          1000UL  // samp rate =1khz
#define   CLK_MAIN       84000000UL
#define   TMR_CNTR       CLK_MAIN / (2 *SMP_RATE)

// FFT_SIZE IS DEFINED in Header file Radix4.h
// #define   FFT_SIZE           4096

#define   MIRROR        FFT_SIZE / 2
#define   INP_BUFF      FFT_SIZE   // 2 channels * FFT_SIZE/2 buffer each time 
#define   THRESHOLD     4
#define   ZERO_ENERGY   2 //energy level at sensor, below this level sensor is assumed to be disconected!
#define   HISTLVL       4
#define   ENGINE_ON     50
#define   DOOR_CLOSE    48
#define   DOOR_LOCKED   46
#define   HORN_RELAY    32
#define   LIGHTS_RELAY  30
#define   WINDOW_REAR2  28
#define   WINDOW_REAR1  24
#define   WINDOW_FRONT  22
#define   LED           53  /*yellow blink led for system function*/
#define   LED2          13  /* this led is for me!*/
#define   chipSelect    36
#define   WAIT2STOP     600 /*wait 10 minutes until stop logging*/
/* system global variable*/
/* these global variables are for ADC handle*/
volatile  uint16_t  sptr      = 0;
volatile  int16_t   flag      = 0;
uint16_t  inp[2][INP_BUFF]  = {0};     // DMA likes ping-pongs buffer
int sensor1[FFT_SIZE] = {0};
int sensor2[FFT_SIZE] = {0};

/* these global variables are for output and digital inputs */
volatile int period = 0;
volatile int start_time = 0;
volatile int wait_time = 0;
volatile  short  overload_cnt = 0;
volatile  bool  door_locked = false;
volatile  bool door_timeout = false;
//volatile  int door_cnt = 0;
volatile  bool door_count = false;
volatile  bool engine = false;
volatile  bool door_open = false;
volatile bool start_log = false;
volatile int warning_cnt = 0;
volatile int warning_state = 0;
volatile int window_state = 0;
volatile int window_cnt = 0;
volatile int pin;
volatile bool event = false;
String datalogname = String("datalog0.txt");
bool write2sd = true;


SplitRadixRealP     radix;
DS3231 Clock;

void setup()
{
  /* start serial communication to PC*/
  Serial.begin (115200) ;
  Serial.println("\n  BabyWatch SW ver 1.6.2 \n");
  /* define digital I\Os from and to car*/
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(WINDOW_FRONT, OUTPUT);
  pinMode(WINDOW_REAR1, OUTPUT);
  pinMode(WINDOW_REAR2, OUTPUT);
  pinMode(LIGHTS_RELAY, OUTPUT);
  pinMode(HORN_RELAY, OUTPUT);
  pinMode(WINDOW_FRONT, OUTPUT);
  pinMode(ENGINE_ON, INPUT);
  pinMode(DOOR_CLOSE, INPUT);
  pinMode(DOOR_LOCKED, INPUT);
  /* define pin for ADC triger*/
  pinMode( 2, INPUT);
  /* init led output to low*/
  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(WINDOW_FRONT, LOW);
  /*SD card init */
  sdinit(&write2sd);
  if (write2sd) datalogname = get_next_file_number(datalogname);
  check_relay();
  set_RTC_REG();
  /* init ADC functions*/
  adc_setup ();
  tmr_setup ();
  /* start interrupts to engin status and door status*/
  attachInterrupt(digitalPinToInterrupt(ENGINE_ON), isr_engine_on, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
  attachInterrupt(digitalPinToInterrupt(DOOR_CLOSE), isr_door_open, CHANGE);
}
/* main loop function is called every iteration and together with setup()it replace in arduino enviroment main function
    in other environment . for more detail see arduino website https://www.arduino.cc/en/Tutorial/HomePage

*/
void loop()
{
  short indx_a = flag - 1;
  static short kid_cnt = 0;

  static bool overload = false;
  bool kidincar;
  static bool alert = false;
  static bool log_flag = true;

  if ( flag )
  {
    overload_cnt--;
    datalogger(indx_a, log_flag);
    load_data(sensor2, sensor1, indx_a);
    kidincar = child_detect(sensor1, sensor2);   /*this line activate freqency domain analysis*/
    //    kidincar = child_detect_over_t(sensor1,sensor2);  /* this line activate time domain analysis*/
    if (kidincar) {
      kid_cnt++;
    }
    else {
      kid_cnt--;
    }
    histersys_check(&kid_cnt, &alert);
    log_flag = logging_check(alert);
    make_action(alert);
    if (alert) {
      digitalWrite(51, HIGH);
    }
    else {
      digitalWrite(51, LOW);
    }
    if (overload_cnt < 0) {
      overload_cnt = 0;
      overload = false;
    }
    else if (overload_cnt > 1) {
      overload = true;
      Serial.println("system is overloaded");
    }
    flag = 0; //turn off flag
  }
  read_from_serial(&flag);
}




/**********************************************************************************/
/**********************************************************************************
   Function name: load_data
   inputs : 2 int arrays and buffer number to be read
   it read the data in sampling buffer and split it between sensor arrays
 **********************************************************************************/
void load_data(int in_array1[] , int in_array2[], int buff) {
  int i, pivot = FFT_SIZE / 2;
  for (i = 0; i < pivot; i++) {
    in_array1[i] = in_array1[i + pivot];
    in_array2[i] = in_array2[i + pivot];
  }
  for (i = 0; i < pivot ; i++) {
    in_array1[i + pivot] = inp[buff][2 * i] - 2047;
    in_array2[i + pivot] = inp[buff][2 * i + 1] - 2047;
  }

}
/***********************************************************************************/

void switch_relay(int relay) {
  digitalWrite(relay, HIGH);
  delay(400);
  digitalWrite(relay, LOW);
  delay(400);
}

void check_relay() {
  switch_relay(LIGHTS_RELAY);
  switch_relay(WINDOW_REAR2);
  switch_relay(WINDOW_REAR1);
  switch_relay(WINDOW_FRONT);
  switch_relay(HORN_RELAY);
}

/************************************************************************************/

void read_from_serial(volatile int16_t* flag) {
  if (Serial.available() > 0) {
    // msg format : <command: var,var2....,varn\n>
    String msg = Serial.readStringUntil('\n');
    int index = msg.indexOf(": ");
    String command = msg.substring(0, index);
    if (command.compareTo("set RTC") == 0) {
      String timetoset = msg.substring(index);
      set_RTC(timetoset);
    }
    *flag = 0;
  }
}
/**************************************************************************************/


/**************************************************************************************/
void search_and_replace(String str,int* num) {
  int index;
  index = str.indexOf(',');
  String temp = str.substring(0, index);
  *num = temp.toInt();
  str.remove(0, index + 1);
}


