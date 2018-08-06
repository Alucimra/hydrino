#include <config.h>
#include <debug/clock.h>
#include <debug/settings.h>
#include <debug/log.h>
#include <debug/stepping.h>
#include <debug/motor.h>

#ifndef HYDRINO_DEBUG
#define HYDRINO_DEBUG
#if(DEBUG)


void printHelp() {
  Serial.println(F(":) List of Debug Commands"));
  Serial.println(F(":) (empty) or ?\tThis thing you're reading."));
  Serial.println(F(":) dump\tprint out all the config and data in one big dump"));
  Serial.println(F(":) step\tStep into the motorLoop"));

  #if(DRIVE_ID)
  Serial.println(F(":) readLogs\tread the eeprom logs"));
  Serial.println(F(":) debugLogs\tattempt to read all log entries regardless of its marker"));
  Serial.println(F(":) clearLogs\tclear the eeprom logs with zeros"));
  #endif

  #if(DS3231_ID)
  Serial.println(F(":) readTemp\tget the current temperature"));
  Serial.println(F(":) readBattery\tget the current battery charge"));
  Serial.println(F(":) readTime\tread the current time"));
  Serial.println(F(":) readTime10\tread the current time 10 times (ticking seconds)"));
  Serial.println(F(":) writeTime\tset the current time"));
  #endif

  Serial.println(F(":) motor:on\tturn the motor on with startMotor()"));
  Serial.println(F(":) motor:off\tturn the motor off with startMotor()"));
  Serial.println(F(":) motor\toutput motor status"));

  Serial.println(F(":) config:all\tprint all settings"));
  Serial.println(F(":) config:pins\tprint pin designations and i2c devices"));
  Serial.println(F(":) config:battery\tprint battery levels"));
  Serial.println(F(":) config:drive\tprint drive config"));
  Serial.println(F(":) config:clock\tprint clock config"));
  Serial.println(F(":) config:temp\tprint temperature sensor config"));
}

void dataDump(){
  readTime();
  Serial.println();
  Serial.print(F(":) About to dump all data...this could take a while."));
  printAllSettings();
  readDriveLogs();
  Serial.println();
  Serial.print(F(":) Done."));
}

void readBattery(){
  Serial.print(F(":) "));
  uint16_t charge = currentBatteryCharge();
  Serial.print(F(":: Battery charge:> "));
  Serial.println(charge);
}

void debugLoop(){
  if(isStepping){
    #if DEBUG_DEEP
      Serial.println();
      Serial.print(F(":) Stepping... autoStep / stepCounter"));
      Serial.print(autoStep);
      Serial.print(F(" / "));
      Serial.println(stepCounter);
      Serial.println(F("*****----------*****"));
    #endif
    if(autoStep || (--stepCounter > 0)){
      logLoop();
      motorLoop();
      return;
    } else {
      isStepping = false;
      Serial.println(F(":) Stepping complete."));
    }
  }

  if(Serial.available() == 0){ return; }
  Serial.print(F(":] "));
  String cmd = Serial.readStringUntil(13);
  Serial.print(cmd);
  Serial.println();

  if(cmd == "dump"){ dataDump(); }

  if(cmd == "" or cmd == "?"){ printHelp(); }

  if(cmd == "readBattery"){ readBattery(); }
  if(cmd == "readTemp"){ readTemperature(); }
  if(cmd == "readTime"){ readTime(); }
  if(cmd == "readTime10"){
    for(uint8_t i=0; i<10; i++){
      readTime();
    }
  }
  if(cmd == "writeTime"){ writeTime(); }

  if(cmd == "config:all"){ printAllSettings(); }
  if(cmd == "config:pins"){ printPins(); }
  if(cmd == "config:battery"){ printBatterySettings(); }
  if(cmd == "config:drive"){ printDriveSettings(); }
  if(cmd == "config:clock"){ printClockSettings(); }
  if(cmd == "config:temp"){ printTemperatureSettings(); }

  if(cmd == "motor"){ debugMotorStatus(); }
  if(cmd == "motor:on"){ debugMotorStart(); }
  if(cmd == "motor:off"){ debugMotorStop(); }

  if(cmd == "readLogs"){ readDriveLogs(); }
  if(cmd == "clearLogs"){ clearDriveLogs(); }
  if(cmd == "debugLogs"){ debugDriveLogs(); }

  if(cmd == "step"){ debugStepping(); }

  delay(50);
}

#endif
#endif
