/* functions in this file make use of  SD libarary and SPI libarary ********************/
/**************************************************************************************/
/* function name : datalogger
    inputs : buffer index
    output : none
    description : this function write buffer data to SD card 2 samples in a line
    and then print new line caracter
 **************************************************************************************/
void datalogger(unsigned int buff, const bool log_flag) {
  if (log_flag) {
    File dataFile = SD.open(datalogname, FILE_WRITE);
    if (dataFile) {
      for (int i = 0; i < INP_BUFF ; i += 2) {
        dataFile.println(String(inp[buff][i]) + "," + String(inp[buff][i + 1]));
      }
      dataFile.close();
    }
  }
}
/***************************************************************************************/
/* function name : get_next_file_number
    input : string object with file name
    output : new string object with vacant file name
    this function search for a number in filename and increase that number in filename
 **************************************************************************************/
String get_next_file_number(String filename) {
  int i = 1;
  while (SD.exists(filename)) {
    filename.replace(String(i - 1), String(i));
    Serial.println(filename);
    i++;
  }

  return filename;
}
String get_next_file_number2(String filename) {
  int index = 0;
  int num = 0;
  char* temp_buff = (char*)malloc(filename.length() * sizeof(char));
  filename.toCharArray(temp_buff, filename.length());
  while (temp_buff[index] != 0) {
    if (isDigit(temp_buff[index])) {
      num = 10 * num + (temp_buff[index] - '0');
    }
  }
  filename.replace(String(num), String(num + 1));
  free(temp_buff);
  return filename;
}
/***********************************************************************************
   Function name : sdinit
   input a pointer to flag that indicate if SD card init sucessful
   output : none
 ***********************************************************************************/
void sdinit(bool* write2sd) {
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    *write2sd = false;
    digitalWrite(WINDOW_FRONT, HIGH);
  }
  else {
    Serial.println("card initialized.");
    *write2sd = true;
    digitalWrite(WINDOW_FRONT, LOW);
  }
}

/************************************************************************************
   Function name : logging_check
   input         : bool alert
   output        : bool
   description   : this function check if possible to stop logging to SD card. if past 10 min
   after door lock or door close and alert is false (no kid detected then sd_write_enable is false
 * *********************************************************************************/
bool logging_check(const bool alert) {
  if (start_log && sdinit) return true;
  if (((millis() - wait_time) / 1000 > WAIT2STOP) && (!alert)) return false;
}
/************************************************************************************
   Function name : write_time
   input         : String filename
   output        : none
   description   : this function read time, date and temeperature from DS3231n RTC module 
   and write it to file designated by filename on SD CARD
 * *********************************************************************************/
void write_time(String filename) {
  File FileID = SD.open(filename, FILE_WRITE);
  if (FileID) {
    FileID.println("sample reate = 1khz");
    FileID.println("measurement created in date:");
    read_RTC_time(FileID);
    FileID.close();
  }
}
/***********************************************************************************/
