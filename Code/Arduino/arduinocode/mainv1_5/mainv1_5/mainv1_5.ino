#include <SplitRadixRealP.h>

#define channel 0 /*sample channels AN2 AN3*/
//#define channel 1 /*sample channels AN0 AN1*/

#define   SMP_RATE          1000UL  // samp rate =1khz
#define   CLK_MAIN       84000000UL
#define   TMR_CNTR       CLK_MAIN / (2 *SMP_RATE)

// FFT_SIZE IS DEFINED in Header file Radix4.h
// #define   FFT_SIZE           4096

#define   MIRROR        FFT_SIZE / 2
#define   INP_BUFF      FFT_SIZE   // 2 channels * FFT_SIZE/2 buffer each time 
#define   THRESHOLD     1.3
#define   HISTLVL       5
#define   f_final       100 // we sum energy until 20hz
#define   nCS           36
#define   ENGINE_ON     38
#define   DOOR_CLOSE    40
#define   DOOR_LOCKED   42

volatile   uint16_t   sptr      = 0;
volatile    int16_t   flag      = 0;
volatile    short  overload_cnt = 0;
int sensor1[FFT_SIZE] = {0};
int sensor2[FFT_SIZE] = {0};
uint16_t  inp[2][INP_BUFF]  = {0};     // DMA likes ping-pongs buffer
uint8_t          print_inp  =  0 ;     // print switch
//bool card = true;

SplitRadixRealP     radix;

void setup()
{
  Serial.begin (115200) ;
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(ENGINE_ON, INPUT);
  pinMode(DOOR_CLOSE, INPUT);
  pinMode(DOOR_LOCKED, INPUT);
  digitalWrite(13, LOW);
  digitalWrite(8, LOW);
  adc_setup ();
  tmr_setup ();
  pinMode( 2, INPUT); //
//  if (!SD.begin(nCS)) {
//    Serial.println("Card failed, or not present");
//    card = false;
//  }
//  else Serial.println("card initialized.");
}

void loop()
{
  short indx_a = flag - 1;
  static short kid_cnt = 0;
  static bool overload = false;
  bool kidincar;
  static bool alert = false;
  static short kidcnt = 0;
  if ( flag )
  {
    overload_cnt--;
    digitalWrite(8, HIGH);
    load_data(sensor2, sensor1, indx_a);
    digitalWrite(8, LOW);
    kidincar = child_detect(sensor1, sensor2);
    Serial.println(kid_cnt);
    if (kidincar) {
      kid_cnt++;
    }
    else {
      kid_cnt --;
    }
    histogram_check(&kid_cnt, &alert);
    make_action(alert);

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
  //  print_inp = check_serial_port();
  //  if ( print_inp ) {
  //    Serial.print("\n\tBuffer: ");
  //    Serial.print(indx_a, DEC);
  //    prnt_out2( sensor1, FFT_SIZE);
  //    prnt_out2( sensor2, FFT_SIZE);
  //    print_inp =  0;
  //  }
}
//void datalogger(bool card){
//  File dataFile = SD.open("datalog.txt", FILE_WRITE);
//  if (dataFile) {
//    dataFile.println("Alert : child detected!" );
//    dataFile.close();
//  }
//}
/*event output handle */
void make_action(bool act) {
  make_event(act);
  //  if (digitalRead(ENGINE_ON)) {
  //    //light green led
  //  }
  //  else {
  //    if (digitalRead(DOOR_CLOSE)) {
  //      if (digitalRead(DOOR_LOCKED)) make_event(false);
  //      else warning_horn = true;
  //    }
  //  }
}

void make_event(bool on) {
  digitalWrite(22, on);
  digitalWrite(24, on);
  digitalWrite(28, on);
  digitalWrite(30, on);
  digitalWrite(32, on);
}

void histogram_check(short *cnt, bool *state) {
  if (*cnt < 0) {
    *cnt = 0;
    *state = false;
  }
  else if (*cnt > HISTLVL) {
    *cnt = HISTLVL;
    *state = true;
  }
}

int check_serial_port() {
  int print_inp = 0;
  while (Serial.available()) {
    uint8_t input = Serial.read();
    switch (input) {
      case 'x':
        print_inp = 1;
        break;
      case '?':
      case 'h':
        cmd_print_help();
        break;
      default: // -------------------------------
        Serial.print("Unexpected: ");
        Serial.print((char)input);
        cmd_print_help();
    }
    Serial.print("> ");
  }
  return print_inp;
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
/**********************************************************************************/
void load_data(int in_array1[] , int in_array2[], int buff) {
  int i, pivot = FFT_SIZE / 2;
  for (i = 0; i < pivot; i++) {
    in_array1[i] = in_array1[i + pivot];
    in_array2[i] = in_array2[i + pivot];
  }
  for (i = 0; i < pivot ; i++) {
    in_array1[i + pivot] = inp[buff][2 * i];
    in_array2[i + pivot] = inp[buff][2 * i + 1];
  }
}
/***********************************************************************************/

void prnt_out2( int *array, int dlina)
{
  Serial.print("\n\t");
  for ( uint32_t i = 0; i < dlina; i++)
  {
    Serial.print(array[i]);
    Serial.print("\t");
    if ((i + 1) % 16 == 0) Serial.print("\n\t");
  }
  Serial.println("\n\t");
}

void cmd_print_help(void)
{
  Serial.println("\n  Listing of all available CLI Commands\n");
  Serial.println("\t\"?\" or \"h\": print this menu");
  Serial.println("\t\"x\": print out adc array");
  Serial.println("\t\"f\": print out fft array");
  Serial.println("\t\"o\": print out magnitude array");
  Serial.println("\t\"m\": print out time measurements results");
}
/**************************************************************************************/
void pio_TIOA0 ()  // Configure Ard pin 2 as output from TC0 channel A (copy of trigger event)
{
  PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
  PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
  PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral
}

void tmr_setup ()
{
  pmc_enable_periph_clk(TC_INTERFACE_ID + 0 * 3 + 0); // clock the TC0 channel 0

  TcChannel * t = &(TC0->TC_CHANNEL)[0] ;            // pointer to TC0 registers for its channel 0
  t->TC_CCR = TC_CCR_CLKDIS ;                        // disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF ;                           // disable interrupts
  t->TC_SR ;                                         // read int status reg to clear pending
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |           // use TCLK1 (prescale by 2, = 42MHz)
              TC_CMR_WAVE |                          // waveform mode
              TC_CMR_WAVSEL_UP_RC |                  // count-up PWM using RC as threshold
              TC_CMR_EEVT_XC0 |     // Set external events from XC0 (this setup TIOB as output)
              TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
              TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR ;

  t->TC_RC = TMR_CNTR;              // counter resets on RC, so sets period in terms of 42MHz clock
  t->TC_RA = TMR_CNTR / 2;          // roughly square wave
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.
}

void adc_setup ()
{
  pmc_enable_periph_clk(ID_ADC);
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  NVIC_EnableIRQ (ADC_IRQn);               // enable ADC interrupt vector

  adc_disable_all_channel(ADC);
  adc_enable_interrupt(ADC, ADC_IER_RXBUFF);

  ADC->ADC_RPR  =  (uint32_t)  inp[0];      // DMA buffer
  ADC->ADC_RCR  =  INP_BUFF;
  ADC->ADC_RNPR =  (uint32_t)  inp[1];      // next DMA buffer
  ADC->ADC_RNCR =  INP_BUFF;
  ADC->ADC_PTCR =  1;

  adc_set_bias_current(ADC, 0x01);
  //  adc_enable_tag(ADC);
  if (channel){
    adc_enable_channel(ADC, ADC_CHANNEL_7);  // AN0
    adc_enable_channel(ADC, ADC_CHANNEL_6);  // AN1
  }
  else {
    adc_enable_channel(ADC, ADC_CHANNEL_5);  // AN0
    adc_enable_channel(ADC, ADC_CHANNEL_4);  // AN1 
  }
  adc_configure_trigger(ADC, ADC_TRIG_TIO_CH_0, 0);
  adc_start(ADC);
}

void ADC_Handler (void)
{
  static bool done = false;
  if ((adc_get_status(ADC) & ADC_ISR_RXBUFF) ==  ADC_ISR_RXBUFF) {
    overload_cnt++;
    flag = ++sptr;
    sptr &=  0x01;
    ADC->ADC_RNPR  =  (uint32_t)  inp[sptr];
    ADC->ADC_RNCR  =  INP_BUFF;
    done = !done;
  }
  digitalWrite(13, done);
}

