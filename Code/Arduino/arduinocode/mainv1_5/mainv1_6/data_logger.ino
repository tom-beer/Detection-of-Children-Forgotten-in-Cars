/* functions in this file make use of  SD libarary and SPI libarary ********************/
/**************************************************************************************/
/* function name : datalogger
 *  inputs : buffer index 
 *  output : none 
 *  description : this function write buffer data to SD card 2 samples in a line 
 *  and then print new line caracter
 **************************************************************************************/
void datalogger(unsigned int buff,bool log_flag) {
  if (!log_flag) return; 
  File dataFile = SD.open(datalogname, FILE_WRITE);
  if (dataFile) {
    for (int i = 0; i < INP_BUFF ; i += 2) {
      dataFile.println(String(inp[buff][i]) + "," + String(inp[buff][i + 1]));
    }
    dataFile.close();
  }
}
/***************************************************************************************/
/* function name : decision_logger
 *  input : bool with the algoritm desision 
 *  output : none
 *  description : this function write the algorithm desicion to a file on SD card
 ***************************************************************************************/
void decision_logger(bool decision, bool log_flag) {
  if(!log_flag) return;
  File alertFile = SD.open(alertlogname, FILE_WRITE);
  if (alertFile) {
    alertFile.println(String(decision));
    alertFile.close();
  }
}
/***************************************************************************************/
/* function name : get_next_file_number 
 *  input : string object with file name 
 *  output : new string object with vacant file name 
 *  this function check if a file name is exist on SD card
 **************************************************************************************/
String get_next_file_number(String filename){
  int i = 1;
  while (SD.exists(filename)){
    filename.replace(String(i-1), String(i));
  }
  return filename;
}

/***********************************************************************************
 * Function name : sdinit 
 * input a pointer to flag that indicate if SD card init sucessful 
 * output : none
 ***********************************************************************************/
void sdinit(bool* write2sd) {
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    *write2sd = false;
  }
  Serial.println("card initialized.");
}
