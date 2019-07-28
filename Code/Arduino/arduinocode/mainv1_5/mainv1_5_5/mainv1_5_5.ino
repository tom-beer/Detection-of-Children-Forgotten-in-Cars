#include <DueTimer.h>
#include <SplitRadixRealP.h>

#define channel 0 /*sample channels AN2 AN3*/ /* odd channel number are for kid sensor!*/ 
//#define channel 1 /*sample channels AN0 AN1*/

#define   SMP_RATE          1000UL  // samp rate =1khz
#define   CLK_MAIN       84000000UL
#define   TMR_CNTR       CLK_MAIN / (2 *SMP_RATE)

// FFT_SIZE IS DEFINED in Header file Radix4.h
// #define   FFT_SIZE           4096

#define   MIRROR        FFT_SIZE / 2
#define   INP_BUFF      FFT_SIZE   // 2 channels * FFT_SIZE/2 buffer each time 
#define   THRESHOLD     4 
#define   HISTLVL       5
#define   nCS           36
#define   ENGINE_ON     50
#define   DOOR_CLOSE    48
#define   DOOR_LOCKED   46
#define   HORN_RELAY    32
#define   LIGHTS_RELAY  30
#define   WINDOW_REAR2  28
#define   WINDOW_REAR1  24
#define   WINDOW_FRONT  22
#define   LED           53  /*yellow blink led for system function*/
#define   LED2          13  /* it for me!*/

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
volatile  short  overload_cnt = 0;
volatile  bool  door_locked = false;
volatile  bool door_timeout = false;
volatile  int door_cnt = 0;
volatile  bool door_count = false;
volatile  bool engine = false;
volatile  bool door_open = false;
volatile int warning_cnt = 0;
volatile int warning_state = 0;
volatile int window_state = 0;
volatile int alert_state = 0;
volatile int window_cnt = 0;
volatile int pin;
volatile bool event = false;




SplitRadixRealP     radix;

void setup()
{
  /* start serial communication to PC*/
  Serial.begin (115200) ;
  Serial.println("\n  BabyWatch SW ver 1.5.5 \n");
  /* define digital I\Os from and to car*/
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(WINDOW_FRONT, OUTPUT);
  pinMode(WINDOW_REAR1, OUTPUT);
  pinMode(WINDOW_REAR2, OUTPUT);
  pinMode(LIGHTS_RELAY, OUTPUT);
  pinMode(HORN_RELAY, OUTPUT);
  pinMode(ENGINE_ON, INPUT);
  pinMode(DOOR_CLOSE, INPUT);
  pinMode(DOOR_LOCKED, INPUT);
  /* define pin for ADC triger*/
  pinMode( 2, INPUT); 
  /* init led output to low*/
  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  /* init ADC functions*/ 
  adc_setup ();
  tmr_setup ();
  /* start interrupts to engin status and door status*/ 
  attachInterrupt(digitalPinToInterrupt(ENGINE_ON), isr_engine_on, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
}

void loop()
{
  short indx_a = flag - 1;
  static short kid_cnt = 0;
  static bool overload = false;
  bool kidincar;
  static bool alert = false;
  if ( flag )
  {
    overload_cnt--;
    load_data(sensor2, sensor1, indx_a);
    kidincar = child_detect(sensor1, sensor2);   /*this line activate freqency domain analysis*/
//    kidincar = child_detect_over_t(sensor1,sensor2);  /* this line activate time domain analysis*/
    Serial.println(kid_cnt);
    if (kidincar) {
      kid_cnt++;
    }
    else {
      kid_cnt--;
    }
    histersys_check(&kid_cnt, &alert);
    make_action(alert);
        if (alert) {
      digitalWrite(51, HIGH);
    }
    else {
      digitalWrite(51,LOW);
    }
    if (overload_cnt < 0) {
      overload_cnt = 0;
      overload = false;
    }
    else if (overload_cnt > 1) {  if (alert) {
      digitalWrite(51, HIGH);
    }
    else {
      digitalWrite(51,LOW);
    }
      overload = true;
      Serial.println("system is overloaded");
    }
    flag = 0; //turn off flag
  }
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
    in_array1[i + pivot] = inp[buff][2 * i]-2047;
    in_array2[i + pivot] = inp[buff][2 * i + 1]-2047;
  }
}
/***********************************************************************************/



