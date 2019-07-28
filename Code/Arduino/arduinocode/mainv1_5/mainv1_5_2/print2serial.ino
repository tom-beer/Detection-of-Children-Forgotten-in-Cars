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
/**************************************************************************/
void cmd_print_help(void)
{
  Serial.println("\n  Listing of all available CLI Commands\n");
  Serial.println("\t\"?\" or \"h\": print this menu");
  Serial.println("\t\"x\": print out adc array");
  Serial.println("\t\"f\": print out fft array");
  Serial.println("\t\"o\": print out magnitude array");
  Serial.println("\t\"m\": print out time measurements results");
}

/************************************************************************/
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
