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

