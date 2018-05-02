#include <config.h>
#include <chipdata.h>
#include <drive.h>

#ifndef HYDRINO_DEBUG
#define HYDRINO_DEBUG

void printVoltageLevels(){
  // TODO: Printing out this json is costing us over 900 bytes of program space.
  // If it becomes a problem, we'll have to just switch to delimiters
  /*
  Serial.print(F(":: Voltage Levels :> {\"overcharge\":"));
  Serial.print(OVERCHARGE);
  Serial.print(F(",\"solar\":"));
  Serial.print(SOLAR);
  Serial.print(F(",\"full\":"));
  Serial.print(FULL);
  Serial.print(F(",\"charged\":"));
  Serial.print(CHARGED);
  Serial.print(F(",\"nominal\":"));
  Serial.print(NOMINAL);
  Serial.print(F(",\"drained\":"));
  Serial.print(DRAINED);
  Serial.print(F(",\"cutoff\":"));
  Serial.print(CUTOFF);
  Serial.print(F(",\"tolerance\":"));
  Serial.print(TOLERANCE);
  Serial.print(F(",\"multiplier\":"));
  Serial.print(VOLTAGE_MULTIPLIER);
  Serial.print(F("}"));
  Serial.println();
  */
}

void printPins(){
  Serial.print(F(":: Pins :> {\"cycle_length\":"));
  Serial.print(CYCLE_LENGTH);
  Serial.print(F(",\"motorA\":"));
  Serial.print(MOTOR_A);
  Serial.print(F(",\"motorB\":"));
  Serial.print(MOTOR_B);
  Serial.print(F(",\"powerCheck\":"));
  Serial.print(POWER_CHECK);
  Serial.print(F(",\"powerActivate\":"));
  Serial.print(POWER_ACTIVATE);
  Serial.print(F(",\"debugPin\":"));
  Serial.print(DEBUG_PIN);

  #if(HAS_DRIVE)
  Serial.print(F(",\"driveId\":"));
  Serial.print(DRIVE_ID);
  #endif

  #if(HAS_CLOCK)
  Serial.print(F(",\"clockId\":"));
  Serial.print(CLOCK_ID);
  #endif

  Serial.print(F("}"));
  Serial.println();
}

void printMotorLevels(){
  /*
  Serial.print(F(":: Motor Power Levels :> {\"motorStartWait\":"));
  Serial.print(MOTOR_START_WAIT);
  Serial.print(F(",\"max\":"));
  Serial.print(MAX);
  Serial.print(F(",\"strong\":"));
  Serial.print(STRONG);
  Serial.print(F(",\"weak\":"));
  Serial.print(WEAK);
  Serial.print(F(",\"disabled\":"));
  Serial.print(DISABLED);
  Serial.print(F("}"));
  Serial.println();
  // Hard coded for now, since it's tricky to do it otherwise
  Serial.print(F(":: Motor Timing :> {\""));
  Serial.print(SOLAR+TOLERANCE);
  Serial.print(F("\":\"AB6 AB6 AB6 AB6 100\",\""));
  Serial.print(FULL+TOLERANCE);
  Serial.print(F("\":\"*5 A4 *5 B4 44\",\""));
  Serial.print(CHARGED+TOLERANCE);
  Serial.print(F("\":\"*3 a2 *3 b2 40\",\""));
  Serial.print(NOMINAL-TOLERANCE);
  Serial.print(F("\":\"*3 a *3 b 25\",\""));
  Serial.print(DRAINED-TOLERANCE);
  Serial.print(F("\":\"*15 a *15 b 6\",\""));
  Serial.print(CUTOFF);
  Serial.print(F("\":\"*25 a *25 b 4\",\""));
  Serial.print(F("0\":\"*31 *31 *31 *31 0\""));
  Serial.print(F("}"));
  Serial.println();
  */
}

void printDriveConfig(){
  Serial.print(F(":: Drive Config :> {\"space\":"));
  Serial.print(DRIVE_SPACE);
  Serial.print(F(",\"driveId\":"));
  Serial.print(DRIVE_ID);
  Serial.print(F(",\"drivePos\":"));
  Serial.print(drivePos);
  Serial.print(F(",\"eepromSize\":"));
  Serial.print(EEPROM.length());
  Serial.print(F(",\"writeLimit\":"));
  Serial.print(DRIVE_WRITE_LIMIT);
  Serial.print(F(",\"markerByte\":"));
  Serial.print(DRIVE_MARKER_BYTE);
  Serial.print(F("}"));
  Serial.println();
}

void clearSerialBuffer(){
  while(Serial.available() > 0){
    Serial.read();
  }
}

void printLogEntry(unsigned int i, uint8_t value){
  if(i % EEPROM.length() < logStart){
    Serial.print(i);
    Serial.print(F("\t"));
    Serial.print(value);
    Serial.print(F("\t"));
    Serial.print("reserved");
    Serial.println();
    return;
  }

  Serial.print(i);
  Serial.print(F("\t"));
  if(i % 2 == 0){
    // even numbered sequences are cycles
    Serial.print(value % 4);
    Serial.print(F("\t"));
    Serial.print(value >> 2);
  } else  {
    // odd number sequences is the power
    Serial.print(value);
    Serial.print(F("\t"));
    Serial.print(VOLTAGE_MULTIPLIER*value);
  }
  Serial.println();
}

#if(HAS_DRIVE)
void driveZero(){
  Serial.print(F(":) Clearing drive. This will take a long time. Please Wait..."));
  clearDrive();
  Serial.print(F("Done!"));
  Serial.println();
}

void readDrive(){
  unsigned int bytes_to_read = drivePos*EEPROM.length();
  unsigned int read = 0;

  if(drivePos == 0){
    Serial.print(F(":) There is no data worth reading. drivePos = 0;"));
    Serial.println();
    return;
  }

  Serial.print(F(":) Reading drive data. This might take a while...please wait..."));
  Serial.println();
  Serial.print(F(":: Bytes to read: "));
  Serial.print(bytes_to_read);
  Serial.println();

  power_twi_enable();
  delay(1000);

  while(read < bytes_to_read){
    //TODO: Do we need to begin a transmission before making request?
    Wire.beginTransmission(DRIVE_ID);
    Wire.write(read >> 8);
    Wire.write(read & 0xFF);
    Wire.endTransmission();
    delay(20);
    Wire.requestFrom(DRIVE_ID, DRIVE_WRITE_LIMIT);
    while(Wire.available()){
      printLogEntry(read, Wire.read());
      read++;
    }
    delay(20);
  }
  power_twi_disable();
}
#else
void driveZero(){
  Serial.print(F(":( Drive disabled or unavailable, can not clear"));
  Serial.println();
}

void readDrive(){
  Serial.print(F(":( Drive disabled or unavailable, can not read"));
  Serial.println();
}
#endif


void readLogs(){
  unsigned int i = 0;
  //unsigned int value = 0;

  Serial.print(F(":) Reading Logs "));
  Serial.println();

  Serial.print(F(":: Reserved Bytes :> {\"lastPos\":"));
  Serial.print(logPos);
  Serial.print(F(",\"logStart\":"));
  Serial.print(logStart);
  Serial.print(F(",\"drivePos\":"));
  Serial.print(drivePos);
  Serial.print(F(",\"eepromSize\":"));
  Serial.print(EEPROM.length());
  Serial.print(F(",\"reservedBytes\":["));
  while(i < logStart){
    Serial.print(EEPROM.read(i++));
    if(i < logStart){ Serial.print(F(",")); }
  }
  Serial.print(F("]}"));
  Serial.println();

  /* Log data in tsv format.
   * Format is:
   * <even_number>\t<cycle>\t<cycle_time>
   * <odd_number>\t<aRead>\t<voltage>
   * odds: i  aRead/4  voltage
   * evens: i  cycle  cycle_time
   * There's the expectation that odd cycle_time (except 1) means an off cycle
   * TODO: We can remove this expectation by adding a column (calculate below)
   */
  while(i < logPos){
    printLogEntry(i, EEPROM.read(i));
    i++;
  }
  Serial.print(F(":) Reading leftovers"));
  Serial.println();

  while(i < EEPROM.length()){
    printLogEntry(i, EEPROM.read(i));
    i++;
  }

  Serial.print(F(":) Done"));
  Serial.println();
}

void dataDump(){
  Serial.print(F(":) Get ready for all the data! This will take a while..."));
  Serial.println();

  printPins();
  printVoltageLevels();
  printMotorLevels();
  printDriveConfig();
  readLogs();
  readDrive();

  Serial.print(F(":) Done."));
  Serial.println();
}

void displayTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year) {
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(F(":"));
  if (minute<10) { Serial.print(F("0")); }
  Serial.print(minute, DEC);
  Serial.print(F(":"));
  if (second<10) { Serial.print("0"); }
  Serial.print(second, DEC);
  Serial.print(F(" "));
  Serial.print(dayOfMonth, DEC);
  Serial.print(F("/"));
  Serial.print(month, DEC);
  Serial.print(F("/"));
  Serial.print(year, DEC);
  Serial.print(F(" "));
  Serial.print(F("Weekday (1=Sun, 7=Sat): "));
  Serial.print(dayOfWeek);
}

#if(HAS_CLOCK)
void getTime(){
  uint8_t second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  Serial.print(F(":) Reading the time: "));
  displayTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
  Serial.println();
}
#else
void getTime(){
  Serial.print(F(":( Clock disabled or unavailable, can't get time"));
  Serial.println();
}
#endif

void printHelp() {
  Serial.print(F(":) List of Debug Commands"));
  Serial.println();
  Serial.print(F(":) (empty) or ?\tThis thing you're reading."));
  Serial.println();
  Serial.print(F(":) dump\tprint out all the config and data in one big dump"));
  Serial.println();
  Serial.print(F(":) readLogs\tread the eeprom logs"));
  Serial.println();
  Serial.print(F(":) clearLogs\tclear the eeprom logs with zeros"));
  Serial.println();

  #if(HAS_DRIVE)
  Serial.print(F(":) readDrive\tread the eeprom logs"));
  Serial.println();
  Serial.print(F(":) clearDrive\tclear the eeprom logs with zeros"));
  Serial.println();
  #endif

  #if(HAS_CLOCK)
  Serial.print(F(":) getTime\tread the current time"));
  Serial.println();
  Serial.print(F(":) getTime10\tread the current time 10 times (ticking seconds)"));
  Serial.println();
  Serial.print(F(":) setTime\tset the current time"));
  Serial.println();
  #endif

  Serial.print(F(":) config:voltage\tprint voltage levels for battery"));
  Serial.println();
  Serial.print(F(":) config:pins\tprint pin designations and i2c devices"));
  Serial.println();
  Serial.print(F(":) config:motors\tprint motor power levels"));
  Serial.println();

  #if(HAS_DRIVE)
  Serial.print(F(":) config:drive\tprint drive configuration"));
  Serial.println();
  #endif
}

#if(HAS_CLOCK)
/**
 * There's a little problem with this. Serial baud of 9600 is a little slow and
 * we can't quite pin down the exact timing of the seconds.
 * We can get close by setting a time ahead, and waiting until a second before
 * to press y and start the save. But it could still be about ~1-2s off
 * Can't trust Serial printing either, because there's still the delay.
 */
void setClockTime(){
  uint8_t second, minute, hour, dayOfWeek, dayOfMonth, month, year, accept;
  Serial.setTimeout(60000);
  // give up to 60 seconds to enter the input

  Serial.print(F(":) Year (0 to 99)"));
  Serial.println();
  Serial.print(F(":] "));
  year = (uint8_t)Serial.parseInt() % 100;
  Serial.print(year);
  Serial.println();

  Serial.print(F(":) Month (1 to 12)"));
  Serial.println();
  Serial.print(F(":] "));
  month = (uint8_t)Serial.parseInt() % 13;
  Serial.print(month);
  Serial.println();

  Serial.print(F(":) Day of Month (1 to 31)"));
  Serial.println();
  Serial.print(F(":] "));
  dayOfMonth = (uint8_t)Serial.parseInt() % 32;
  Serial.print(dayOfMonth);
  Serial.println();

  Serial.print(F(":) Day of Week (1 to 7, Sun to Sat)"));
  Serial.println();
  Serial.print(F(":] "));
  dayOfWeek = (uint8_t)Serial.parseInt() % 8;
  Serial.print(dayOfWeek);
  Serial.println();

  Serial.print(F(":) Hour (0 to 23)"));
  Serial.println();
  Serial.print(F(":] "));
  hour = (uint8_t)Serial.parseInt() % 24;
  Serial.print(hour);
  Serial.println();

  Serial.print(F(":) Minute (0 to 59)"));
  Serial.println();
  Serial.print(F(":] "));
  minute = (uint8_t)Serial.parseInt() % 60;
  Serial.print(minute);
  Serial.println();

  Serial.print(F(":) Second (0 to 59)"));
  Serial.println();
  Serial.print(F(":] "));
  second = (uint8_t)Serial.parseInt() % 60;
  Serial.print(second);
  Serial.println();
  Serial.setTimeout(1000);

  clearSerialBuffer();

  Serial.print(F(":) Setting time to ("));
  displayTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
  Serial.print(F("). Press y to continue, or anything else to cancel."));
  Serial.println();
  Serial.print(F(":] "));
  while(Serial.available() <= 0){
    // wait
  }
  accept = Serial.read();
  Serial.print(accept);
  if(accept == 'y'){
    Serial.print(F(":) Setting the time...please wait..."));
    setTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
    delay(500);
    Serial.print(F("Done!"));
    Serial.println();
    getTime();
  } else {
    Serial.print(F(":( Canceled by user."));
    Serial.println();
  }
}
#else
void setClockTime(){
  Serial.print(F(":( Clock disabled or unavailable, can't set time"));
  Serial.println();
}
#endif

void debugLoop(){
  if(Serial.available() == 0){ return; }
  Serial.print(F(":] "));
  String cmd = Serial.readStringUntil(13);
  Serial.print(cmd);
  Serial.println();

  if(cmd == "readLogs"){ readLogs(); }
  if(cmd == "" or cmd == "?"){
    printHelp();
  }
  if(cmd == "clearLogs"){
    clearLogs();
    Serial.print(F(":) Done."));
    Serial.println();
  }
  if(cmd == "getTime"){ getTime(); }
  if(cmd == "getTime10"){
    for(uint8_t i=0; i<10; i++){
      getTime();
    }
  }
  if(cmd == "setTime"){ setClockTime(); }

  if(cmd == "config:voltage"){ printVoltageLevels(); }
  if(cmd == "config:pins"){ printPins(); }
  if(cmd == "config:motors"){ printMotorLevels(); }
  if(cmd == "config:drive"){ printDriveConfig(); }

  if(cmd == "dump"){ dataDump(); }

  if(cmd == "readDrive"){ readDrive(); }
  if(cmd == "clearDrive"){ driveZero(); }

  delay(50);
}

#endif
