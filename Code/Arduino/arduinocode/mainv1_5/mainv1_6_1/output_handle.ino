/*event output handle all functions in this file make use of DueTimer libarary */
/*********************************************************************************/
/* Functio name : make_action
   input bool alert
   output None
   this function handle the the digital switching in case that alert flag rise and
   terminate event when the alert is off
 *********************************************************************************/
void make_action(bool alert) {
  static bool warn_act = false;
  if (alert) {
    digitalWrite(LIGHTS_RELAY, HIGH);
    if (engine) {
      /*do nothing*/
    }
    else {//engine is off
      if (door_open) { 
        if (door_locked) start_event();
        else if (!warn_act) {
          warn_act = true;
          Timer2.attachInterrupt(isr_warning_horn).setPeriod(500000).start();
        }
      }
    }
  }
  else { // no kid detected
    digitalWrite(LIGHTS_RELAY, LOW);
    if (warn_act) {
      stop_warning(&warn_act);;
    }
    if (event) {
      stop_event(); //Door unlocked
    }
  }
}

/*******************************************************************************/
/* Functio name : stop_event
   input        : none
   output       : none
   this function stop timer2 and timer3 and reinitiate timer3 to close windows
 *******************************************************************************/
void stop_event() {
  Timer2.stop();
  Timer3.stop();
  pin = WINDOW_REAR2;
  Timer3.attachInterrupt(isr_window);
  Timer3.start(2000000);
  digitalWrite(HORN_RELAY, LOW);
  digitalWrite(WINDOW_REAR1, LOW);
  event = false;
}
/*******************************************************************************/
/* Functio name : start_event
   input        : none
   output       : none
   this function start timer2 for horn and lights handler and timer3 for window open handler
 ********************************************************************************/
void start_event() {
  Timer2.attachInterrupt(isr_alert_horn);
  Timer2.start(100000);
  pin = WINDOW_REAR1;
  digitalWrite(WINDOW_REAR2, LOW);
  Timer3.attachInterrupt(isr_window);
  Timer3.start(2000000);
  event = true;
}
/***********************************************************************************
   Function name : isr_door_open
   input         : none
   output        : none
   this ISR check DOOR_CLOSE pin for changes in its logic level door_open = '0' car's door is open
   else car's door close. when input rise the function enable the write to SD card function
   in falling edge it will start a counter of 10 min until the write to SD will be disabled.
   (in future version this function will wake up the arduino processor)
 **********************************************************************************/
void isr_door_open() {
  door_open = digitalRead(DOOR_CLOSE); /* door open = '0' door close = '1'*/
  if (door_open) { /* if doors are closed start write to SD card */
    if (sdinit)  {
      start_log = true;/*may need to disable interrupt to adc while updating filename*/
      datalogname = get_next_file_number(datalogname);
    //write time and date from rtc
    }
  }
  else if (!engine){
    wait_time = millis();
    start_log = false;
  }
}
/*******************************************************************************/
/* Function name : isr_engine_on
   input         : none
   output        : none
   this functio is an interrupt service function . the function is called when there is
   a change is ENGINE_ON  pin and it attach or deattach the interrupt on door_loocked pin
***********************************************************************************/
void isr_engine_on () {
  engine = digitalRead(ENGINE_ON);
  if (engine) {
    detachInterrupt(digitalPinToInterrupt(DOOR_LOCKED));/*if engine = '1' disable the door locked inspection*/
  }
  else {
    attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
  }
}

/********************************************************************************/
/* function name : isr_window
    input : none
    output : none
    description : this function open / close car's windows in event case.
    it is called using interrupt caused by timer3 and it change the time period of timer3
    the sequencre is 4 2 sec pulses in 10 swc cycles given to windows motor causing them
    get open or closeed . after 4 pulses the function disable timer3 and deattach all
    interrupt.
 ********************************************************************************/
void isr_window() {
  if (window_cnt < 6) {
    switch (window_state) {
      case 0 :
        digitalWrite(pin, HIGH);
        //        Timer3.stop();
        //        Timer3.attachInterrupt(isr_window);
        Timer3.start(2000000);
        window_state = 1;
        break;
      case 1 :
        digitalWrite(pin, LOW);
        //        Timer3.stop();
        //        Timer3.attachInterrupt(isr_window);
        Timer3.start(8000000);
        window_state = 0;
        window_cnt++;
        break;
      default:
        break;
    }
  }
  else {
    Timer3.stop();
    Timer3.detachInterrupt();
    digitalWrite(pin, LOW);
    window_cnt = 0;
    window_state = 0;
  }
}

/**************************************************/
/* function name : isr_alert_horn
    input : none
    output : none
    description this function turn on and off car's horn
    in alert mode . it is called usinf interrupt cased by timer2
 ***************************************************/
void isr_alert_horn() {
  static bool horn = true;
  digitalWrite(HORN_RELAY, horn);
  horn = !horn;
}
/*************************************************************************/
/* function : warning_horn_isr
    input : none
    output : none
    description : this function inplement alert horn process
    using state machine . it give 4 0.1 sec pulse horn in 10 sec cycle
    after 4 cycles it move to state alert . if door is opened or kid is taken the
    function stop all timer and counting.
 *****************************************************************************/
void isr_warning_horn() {
  if (!door_open) {/* open door when child is seated just reinit counters for warning*/
    warning_cnt = 0;
    warning_state = 0;
  }
  if (warning_cnt < 4) {
    switch (warning_state) {
      case 0 :
        digitalWrite(HORN_RELAY, HIGH);
        Timer2.stop();
        Timer2.setPeriod(100000).start();
        warning_state = 1;
        break;
      case 1 :
        digitalWrite(HORN_RELAY, LOW);
        Timer2.stop();
        Timer2.setPeriod(1000000).start();
        warning_state = 2;
        break;
      case 2:
        digitalWrite(HORN_RELAY, HIGH);
        Timer2.stop();
        Timer2.setPeriod(100000).start();
        warning_state = 3;
        break;
      case 3 :
        Timer2.stop();
        Timer2.setPeriod(10000000).start();
        digitalWrite(HORN_RELAY, LOW);
        warning_cnt++;
        warning_state = 0;
        break;
      default:
        break;
    }
  }
  else {
    warning_cnt = 0;
    warning_state = 0;
    Timer2.stop();
    digitalWrite(HORN_RELAY, LOW);
    start_event();
  }
}
/********************************************************************************/
/* Function name : stop_warning
   input : bool *warn_on
   output : none
   this function stop warning process when chils is taken from seat (alert = '0')
   it re init warning counter and state and stop timer2
 ********************************************************************************/
void stop_warning(bool *warn_on) {
  warning_cnt = 0;
  warning_state = 0;
  Timer2.stop();
  Timer2.detachInterrupt();
  digitalWrite(HORN_RELAY, LOW);
  *warn_on = false;
}
/********************************************************************************/
/* Function name : isr_door_locked_timeout
   input : none
   output : none
    description : this function determine if cae is being locked or unloacked using
    interrupt on pin 46 . 2 pulses mean door unlocked 1 pulse door locked the interval
    between pulses is 400 msec , first we look for rising edge using isr_door_locked_rise
    when first rise edge is detected we look for another rise edge and simulanusly we
    start timeout interrupt set 1.2sec if in this period another rising edge is detected
    door unlocked, if timeout occuer door is locked
 *********************************************************************************/
void isr_door_locked_timeout() {
  Timer4.stop();
  door_count = false;
  if (period > 350) { // if door_cnt =1 one pulse door locked
    door_locked = false;
    
  }
  else {
    door_locked = true;
    wait_time=millis();
    start_log = false;
  }
  period = 0;
}
void isr_door_locked_rise() {
  //only when engin is off!!!
  start_time = millis();
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_fall, FALLING);
  if (!door_count) {
    Timer4.attachInterrupt(isr_door_locked_timeout);
    Timer4.start(1200000);
    door_count = true;
  }
}
void isr_door_locked_fall() {
  period += (millis() - start_time);
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
}

