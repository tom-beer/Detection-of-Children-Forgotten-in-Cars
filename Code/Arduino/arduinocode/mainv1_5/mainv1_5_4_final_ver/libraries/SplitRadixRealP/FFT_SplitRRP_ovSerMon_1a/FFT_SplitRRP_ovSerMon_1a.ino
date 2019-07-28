#include <SplitRadixRealP.h>

#define   SMP_RATE          48000UL 
#define   CLK_MAIN       84000000UL
#define   TMR_CNTR       CLK_MAIN / (2 *SMP_RATE)

// FFT_SIZE IS DEFINED in Header file Radix4.h 
// #define   FFT_SIZE           2048

#define   MIRROR         FFT_SIZE / 2
#define   INP_BUFF       FFT_SIZE 
         
volatile   uint16_t   sptr              =   0 ;
volatile    int16_t   flag              =   0 ;

           uint16_t  inp[2][INP_BUFF]   = { 0};     // DMA likes ping-pongs buffer

            int         f_r[FFT_SIZE]   = { 0};
            int         out1[MIRROR]    = { 0};     // Magnitudes
            int         out2[MIRROR]    = { 0};     // Magnitudes
const       int         dc_offset       = 2047; 

            uint8_t          print_inp  =    0;     // print switch

unsigned long time_start;
unsigned int  time_hamng, time_revbn, time_radix, time_gainr, time_sqrtl, time_sqrtl2;
                 
            SplitRadixRealP     radix;
 
void setup()
{
  Serial.begin (115200) ; 
  adc_setup ();         
  tmr_setup ();         

  pinMode( 2, INPUT); //
}

inline int mult_shft12( int a, int b)  
{
  return (( a  *  b )  >> 12);      
}

void loop() 
{
  if ( flag )
  {   
   uint16_t indx_a = flag -1;
   uint16_t indx_b = 0;

   time_start = micros();
   
   for ( uint16_t i = 0, k = (NWAVE / FFT_SIZE); i < FFT_SIZE; i++ ) 
   {  
      uint16_t windw = Hamming[i * k];
      f_r[i] = mult_shft12((inp[indx_a][indx_b++] - dc_offset), windw);
   }
   time_hamng  = micros() - time_start;

   if( print_inp ){
     Serial.print("\n\tBuffer: ");    
     Serial.print(indx_a, DEC);       
     prnt_out2( f_r, FFT_SIZE);
     print_inp =  0;
     }
   time_start = micros();
   radix.rev_bin( f_r, FFT_SIZE);
   time_revbn  = micros() - time_start;

   time_start = micros();
   radix.fft_split_radix_real( f_r, LOG2_FFT);
   time_radix  = micros() - time_start;

   time_start = micros();
   radix.gain_Reset( f_r, LOG2_FFT -1); 
   time_gainr  = micros() - time_start;

   time_start = micros();
   radix.get_Magnit1( f_r, out1);
   time_sqrtl  = micros() - time_start;

   time_start = micros();
   radix.get_Magnit2( f_r, out2);
   time_sqrtl2  = micros() - time_start;

   while (Serial.available()) {
        uint8_t input = Serial.read();
        switch(input) {
        case '\r':
            break;           
        case 'x':
            print_inp = 1;
            break;
        case 'f':
            Serial.print("\n\tReal: ");    
            prnt_out2( f_r, MIRROR);
            break;
        case 'o':
            Serial.print("\n\tMagnitudes: ");    
            prnt_out2( out1, MIRROR);
            prnt_out2( out2, MIRROR);
            break;
        case 'm':        // Measurements
            Serial.print("\tHamng ");
            Serial.print(time_hamng);
            Serial.print("\tRevb ");
            Serial.print(time_revbn);
            Serial.print("\tSplitRR ");
            Serial.print(time_radix);
            Serial.print("\tGainR ");
            Serial.print(time_gainr);
            Serial.print("\tSqrt ");
            Serial.print(time_sqrtl);
            Serial.print("\tSqrt2 ");
            Serial.println(time_sqrtl2);
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
   flag = 0;
   }   
}

void prnt_out2( int *array, int dlina) 
{
  Serial.print("\n\t");      
     for ( uint32_t i = 0; i < dlina; i++)
     {
       Serial.print(array[i]);       
       Serial.print("\t");    
       if ((i+1)%16 == 0) Serial.print("\n\t");
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

void pio_TIOA0 ()  // Configure Ard pin 2 as output from TC0 channel A (copy of trigger event)
{
  PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
  PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
  PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral
}
 
void tmr_setup ()
{
  pmc_enable_periph_clk(TC_INTERFACE_ID + 0 *3 + 0); // clock the TC0 channel 0

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
  t->TC_RA = TMR_CNTR /2;           // roughly square wave
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
  adc_enable_channel(ADC, ADC_CHANNEL_7);  // AN0
  adc_configure_trigger(ADC, ADC_TRIG_TIO_CH_0, 0);
  adc_start(ADC); 
}

void ADC_Handler (void)
{
  if((adc_get_status(ADC) & ADC_ISR_RXBUFF) ==	ADC_ISR_RXBUFF) {
    flag = ++sptr; 
    sptr &=  0x01;
    ADC->ADC_RNPR  =  (uint32_t)  inp[sptr];
    ADC->ADC_RNCR  =  INP_BUFF;
    }
}

