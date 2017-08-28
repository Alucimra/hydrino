#include <config.h>
#include <chipdata.h>

#ifndef HYDRINO_DEBUG
#define HYDRINO_DEBUG

void printVoltageLevels(){
  // TODO: Printing out this json is costing us over 900 bytes of program space.
  // If it becomes a problem, we'll have to just switch to delimiters
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
  Serial.print(F("}"));
  Serial.println();
}

void printConfig(){
  Serial.print(F(":: Config :> {\"cycle_length\":"));
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
  Serial.print(F("}"));
  Serial.println();
}

void printMotorLevels(){
  Serial.print(F(":: Motor Power Levels :> {\"motorStartWait\":"));
  Serial.print(MOTOR_START_WAIT);
  Serial.print(F(",\"max\":"));
  Serial.print(MAX);
  Serial.print(F(",\"strong\":"));
  Serial.print(STRONG);
  Serial.print(F(",\"weak\":"));
  Serial.print(WEAK);
  Serial.print(F(",\"off\":"));
  Serial.print(OFF);
  Serial.print(F("}"));
  Serial.println();
  // Hard coded for now, since it's tricky to do it otherwise
  Serial.print(F(":: Motor Timing :> {\""));
  Serial.print(SOLAR+TOLERANCE);
  Serial.print(F("\":\"AB6 AB6 AB6 AB6\",\""));
  Serial.print(FULL+TOLERANCE);
  Serial.print(F("\":\"*3 A4 *7 B4\",\""));
  Serial.print(CHARGED+TOLERANCE);
  Serial.print(F("\":\"*5 a *5 b\",\""));
  Serial.print(NOMINAL-TOLERANCE);
  Serial.print(F("\":\"*9 a *9 b\",\""));
  Serial.print(DRAINED-TOLERANCE);
  Serial.print(F("\":\"*19 a *19 b\",\""));
  Serial.print(CUTOFF);
  Serial.print(F("\":\"*59 a *59 b\",\""));
  Serial.print(F("0\":\"*21 *21 *21 *21\""));
  Serial.print(F("}"));
  Serial.println();
}

void printLogEntry(unsigned int i, uint8_t value){
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
    Serial.print(0.016*value);
  }
  Serial.println();
}


void readLogs(){
  unsigned int i = 0;
  //unsigned int value = 0;
  printConfig();
  printVoltageLevels();
  printMotorLevels();

  Serial.print(F(":) Reading Logs "));
  Serial.println();

  Serial.print(F(":: Reserved Bytes :> {\"lastPos\":"));
  Serial.print(logPos);
  Serial.print(F(",\"logStart\":"));
  Serial.print(logStart);
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

  // We're using a pre-processor directive below to skip calling the function
  // if we don't have a drive in place.
  #if(HAS_DRIVE)
  readDrive();
  #endif
}

void readDrive(){
  unsigned int bytes_to_read = SAVE_TO_DRIVE_AT*EEPROM.read(2);
  unsigned int read = 0;
  power_twi_enable();
  delay(1000);

  Serial.print(F(":: Drive Config :> {\"logStart\":"));
  Serial.print(logStart);
  Serial.print(F(",\"driveId\":"));
  Serial.print(DRIVE_ID);
  Serial.print(F(",\"driveSpace\":"));
  Serial.print(DRIVE_SPACE);
  Serial.print(F(",\"saveToDriveAt\":"));
  Serial.print(SAVE_TO_DRIVE_AT);
  Serial.print(F(",\"driveWriteLimit\":"));
  Serial.print(DRIVE_WRITE_LIMIT);
  Serial.print(F("}"));
  Serial.println();
  Serial.print(F(":) Reading from Drive"));
  Serial.println();

  while(read < bytes_to_read){
    //TODO: Do we need to begin a transmission before making request?
    Wire.beginTransmission(DRIVE_ID);
    Wire.write(read >> 8);
    Wire.write(read & 0xFF);
    Wire.endTransmission();
    delay(10);
    Wire.requestFrom(DRIVE_ID, 30); // only read 30 bytes at a time
    while(Wire.available()){
      printLogEntry(read, Wire.read());
      read++;
    }
    delay(10);
  }
  power_twi_disable();
}

void debugLoop(){
  if(Serial.available() > 0){
    switch(Serial.read()){
      case 13 : // return
        readLogs();
        break;
      case ' ' :
        Serial.print(F(":) Yes, we are debugging."));
        Serial.println();
        break;
      case '0' :
        Serial.print(F(":) Clearing Logs...please wait..."));
        clearLogs();
        loadLogPosition();
        Serial.print(F("Done."));
        Serial.println();
        break;
    }
  }
  delay(50);
}

#endif
