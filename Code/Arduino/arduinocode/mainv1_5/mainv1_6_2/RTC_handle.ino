/******************************************************************************/
/* Function name : read_RTC_time
   input : file identifier
   output : none
   description : this function reads RTC time, date and temp from DS3201n moudle board
   and write it to file designated by identifier fid.
 *********************************************************************************/
void read_RTC_time(File FileID) {
  int second, minute, hour, date, month, year, temperature;
  bool h12, PM, Century;
  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();
  temperature = Clock.getTemperature();
  FileID.print(year, DEC);
  FileID.print('-');
  FileID.print(month, DEC);
  FileID.print('-');
  FileID.print(date, DEC);
  FileID.print(' ');
  FileID.print(hour, DEC);
  FileID.print(':');
  FileID.print(minute, DEC);
  FileID.print(':');
  FileID.print(second, DEC);
  if (h12) {
    if (PM)FileID.print("PM");
    else FileID.print("AM");
  }
  FileID.print('\n');
  FileID.print("Temperature=");
  FileID.print(temperature);
  FileID.print('\n');
}
/********************************************************************************/
/* Function name : set_RTC
    input : string contain the time and date sperated by comma in following
    order hour,minute,second,date,mounth,year,dow.
    output : none
    description : this function recieve a string contain time and date,
    extract data from string and set DS3201 rtc module
 *********************************************************************************/
void set_RTC(String timetoset) {
  int second, minute, hour, date, month, year, dow;
  // var order : hour,minute,second,date,mounth,year,dow
  search_and_replace(timetoset, &hour);
  search_and_replace(timetoset, &minute);
  search_and_replace(timetoset, &second);
  search_and_replace(timetoset, &date);
  search_and_replace(timetoset, &month);
  search_and_replace(timetoset, &year);
  search_and_replace(timetoset, &dow);
  Clock.setHour((byte)hour);
  Clock.setMinute((byte)minute);
  Clock.setSecond((byte)second);
  Clock.setDate((byte)date);
  Clock.setMonth((byte)month);
  Clock.setYear((byte)year);
  Clock.setDoW((byte)dow);
}
/***************************************************************************************/

void set_RTC_REG(){
  Clock.enableOscillator(false, false, 0);
  Clock.enable32kHz(false);
  Clock.setClockMode(false); // RTC is set to 24h mode
}

