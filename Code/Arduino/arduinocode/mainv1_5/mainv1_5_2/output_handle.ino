/*event output handle */
void make_action(bool alert) {
  static bool warn_act = false;
  door_open = digitalRead(DOOR_CLOSE);
  Serial.println("make_action");
  Serial.println(door_locked);
  Serial.println(door_open);
  Serial.println(engine);
  if (alert) {
    if (engine) {
      //digitalWrite(WINDOW_FRONT, HIGH);
    }
    else {//engine is off
      if (!door_open) {
        if (door_locked) start_event();
        else if (!warn_act) {
          warn_act = true;
          Timer2.attachInterrupt(isr_warning_horn).setPeriod(1000000).start();
        }
      }
    }
  }
  else { // no kid detected
    digitalWrite(LIGHTS_RELAY, LOW);
    //digitalWrite(WINDOW_FRONT, HIGH);
    warn_act = false;
    stop_event(); //Door unlocked
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
  digitalWrite(LED,LOW);
  Serial.println("stop event");
}
/*******************************************************************************/
/* Functio name : start_event
   input        : none
   output       : none
   this function start timer2 for horn and lights handler and timer3 for window open handler
 ********************************************************************************/
void start_event() {
  digitalWrite(13, HIGH); 
  Timer2.attachInterrupt(isr_alert_horn);
  Timer2.start(1000000);
  pin = WINDOW_REAR1;
  Timer3.attachInterrupt(isr_window);
  Timer3.start(2000000);
  digitalWrite(LED,HIGH);
  Serial.println("start event");
}
/********************************************************************************/
/* Function Name : isr_doorlocked
   input         : none
   output        : none
   this function is an interrupt service function. the function is called when pin
   DOOR_LOCKED voltage rises ('0' to '1'). when called the function advanced the door
   lock pulse counter and when counter is 1 it activate the a timeout timer4 in interrupt mode
*/
void isr_door_locked() {
  if (!engine) { //only when engin is off!!!
    door_cnt++;
    Serial.println(door_cnt);
    if (!door_count) {
      Timer4.attachInterrupt(isr_door_locked_timeout);
      Timer4.start(500000);
      door_count = true;
    }
  }
}
/*********************************************************************************/
/* Function name : isr_door_locked_timeout
   input         : none
   output        : none
   this function is an interrupt service function . the function is called when
   timeout flag set by  rise isr_door_locked , rise . it check the door lock counter counter
   if counter = 1 then door is loced otherwise unlocked, then it re zero door lock pulses counter
   and bool, stop timer4 and deattach the interrupt .
*/
void isr_door_locked_timeout() {
  Timer4.stop();
  Timer4.detachInterrupt();
  Serial.println("isr door locked timeout 32");
  if (door_cnt == 1) { // if door_cnt =1 one pulse door locked
    door_locked = true;
  }
  else door_locked = false; //if door_cnt >1 door unlocked
  door_count = false;
  door_cnt = 0;
}
/*******************************************************************************/
/* Function name : isr_door_close


*/
//void isr_door_close() {
//  door_close = digitalRead(DOOR_CLOSE);
//}
/*******************************************************************************/
/* Function name : isr_engine_on
   input         : none
   output        : none
   this functio is an interrupt service function . the function is called when there is
   a change is ENGINE_ON  pin and it attach or deattach the interrupt on door_loocked pin
*/
void isr_engine_on () {
  engine = digitalRead(ENGINE_ON);
  if (engine) {
    detachInterrupt(digitalPinToInterrupt(DOOR_LOCKED));
    //detachInterrupt(digitalPinToInterrupt(DOOR_CLOSE));
  }
  else {
    attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked, RISING);
    //attachInterrupt(digitalPinToInterrupt(DOOR_CLOSE), isr_door_close, CHANGE);
  }
}

/********************************************************************************/
void isr_window() {
  Serial.println("window");
  Serial.println(window_cnt, DEC);
  if (window_cnt < 4) {
    switch (window_state) {
      case 0 :
        digitalWrite(pin, HIGH);
        window_state = 1;
        break;
      case 1 :
        digitalWrite(pin, LOW);
        Timer3.stop();
        Timer3.attachInterrupt(isr_window).setPeriod(8000000).start();
        window_state = 0;
        window_cnt++;
        break;
      default:
        break;
    }
  }
  else {
    Timer3.stop();
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
  if (door_open) {
    warning_cnt = 0;
    warning_state = 0;
  }
  if (warning_cnt < 4) {
    switch (warning_state) {
      case 0 :
        digitalWrite(HORN_RELAY, HIGH);
        Timer2.stop();
        Timer2.setPeriod(1000000).start();
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
        Timer2.setPeriod(30000000).start();
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
