/*event output handle */
/*********************************************************************************/
/* Functio name : make_action
   input bool alert
   output None
   this function handle the the digital switching in case that alert flag rise and
   terminate event when the alert is off
 *********************************************************************************/
void make_action(bool alert) {
  static bool warn_act = false;
  door_open = digitalRead(DOOR_CLOSE); /* door open = '0' door close = '1'*/
  if (door_locked) Serial.println("door locked");
  else Serial.println("door unlocked");
  if (door_open) Serial.println("door close");
  else Serial.println("door open");
  if (engine) Serial.println("engine on");
  else Serial.println("engine off");
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
//    if (warn_act) {
//      stop_warning(&warn_act);;
//    }
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
  Serial.println("stop event");
  digitalWrite(HORN_RELAY, LOW);
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
  Timer2.start(1000000);
  pin = WINDOW_REAR1;
  Timer3.attachInterrupt(isr_window);
  Timer3.start(2000000);
  Serial.println("start event");
  event = true;
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
void isr_window() {
  Serial.println("window");
  Serial.println(window_cnt, DEC);
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
    window_cnt = 0;
    window_state = 0;
  }
}

/**************************************************/
void isr_alert_horn() {
  static bool horn = true;
  digitalWrite(HORN_RELAY, horn);
  horn = !horn;
  Serial.println("alert horn");
}
/*************************************************/
void isr_warning_horn() {
  Serial.println("warning horn");
  Serial.println(warning_cnt, DEC);
  if (!door_open) {/* open door when child is seated just reinit counters for warning*/
    warning_cnt = 0;
    warning_state = 0;
  }
  if (warning_cnt < 4) {
    switch (warning_state) {
      case 0 :
        digitalWrite(HORN_RELAY, HIGH);
        Timer2.stop();
        Timer2.setPeriod(500000).start();
        warning_state = 1;
        break;
      case 1 :
        digitalWrite(HORN_RELAY, LOW);
        warning_state = 2;
        break;
      case 2:
        digitalWrite(HORN_RELAY, HIGH);
        warning_state = 3;
        break;
      case 3 :
        Timer2.stop();
        Timer2.setPeriod(15000000).start();
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
    start_event();
  }
}
/********************************************************************************/
/* Function name : stop_warning 
 * input : bool *warn_on 
 * output : none
 * this function stop warning process when chils is taken from seat (alert = '0')
 * it re init warning counter and state and stop timer2 
 ********************************************************************************/
void stop_warning(bool *warn_on) {
  warning_cnt = 0;
  warning_state = 0;
  Timer2.stop();
  Timer2.detachInterrupt();
  *warn_on = false;
}
/********************************************************************************/
/* Function name : 
 *  
 *  
 *  
 *  
 *  
 *********************************************************************************/
void isr_door_locked_timeout() {
  Timer4.stop();
  door_count = false;
  if (period > 350) { // if door_cnt =1 one pulse door locked
    door_locked = false;
  }
  else door_locked = true; //if door_cnt >1 door unlocked
  Serial.println(period);
  period = 0;
}
void isr_door_locked_rise() {
  //only when engin is off!!!
  start_time = millis();
  Serial.println("isr door locked rise");
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_fall, FALLING);
  if (!door_count) {
    Timer4.attachInterrupt(isr_door_locked_timeout);
    Timer4.start(1200000);
    door_count = true;
  }
}
void isr_door_locked_fall() {
  period += (millis() - start_time);
  Serial.println("isr door locked fall");
  attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
}

