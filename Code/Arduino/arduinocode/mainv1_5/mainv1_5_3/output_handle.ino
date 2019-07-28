/*event output handle */
void make_action(bool alert) {
  static bool warn_act = false;
  door_open = digitalRead(DOOR_CLOSE);
  door_open = digitalRead(DOOR_CLOSE);
  if (door_locked) Serial.println("door locked");
  else Serial.println("door unlocked");
  if (door_open) Serial.println("door open");
  else Serial.println("door close");
  if (engine) Serial.println("engine on");
  else Serial.println("engine off");
  if (alert) {
    if (engine) {
      //digitalWrite(WINDOW_FRONT, HIGH);
    }
    else {//engine is off
      if (!door_open) {
        if (door_locked) start_event();
        else if (!warn_act) {
          warn_act = true;
          digitalWrite(LIGHTS_RELAY, HIGH);
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
  if (first) {
    Timer2.stop().detachInterrupt();
    Timer3.stop().detachInterrupt();
    pin = WINDOW_REAR2;
    Timer3.attachInterrupt(isr_window);
    Timer3.start(2000000);
    first = false;
  }
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
  first = true;
}

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
    attachInterrupt(digitalPinToInterrupt(DOOR_LOCKED), isr_door_locked_rise, RISING);
    //attachInterrupt(digitalPinToInterrupt(DOOR_CLOSE), isr_door_close, CHANGE);
  }
}

/********************************************************************************/
void isr_window() {
  if (pin == WINDOW_REAR2) Serial.println("close window");
  else Serial.println("open window");
  Serial.print("window cnt=");
  Serial.println(window_cnt);
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
}
/*************************************************/
void isr_warning_horn() {
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

/***********************************************************************/
void isr_door_locked_timeout() {
  Timer4.stop();
  door_count = false;
  if (period > 350) { // if door_cnt =1 one pulse door locked
    door_locked = false;
  }
  else door_locked = true; //if door_cnt >1 door unlocked
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

