#include <config.h>
#include <debug/clock.h>
#include <debug/settings.h>
#include <debug/drive.h>
#include <debug/stepping.h>
#include <motor.h>

#ifndef HYDRINO_DEBUG
#define HYDRINO_DEBUG
#if(DEBUG)


void printHelp() {
  Serial.print(F(":) List of Debug Commands"));
  Serial.println();
  Serial.print(F(":) (empty) or ?\tThis thing you're reading."));
  Serial.println();
  Serial.print(F(":) dump\tprint out all the config and data in one big dump"));
  Serial.println();
  Serial.print(F(":) step\tStep into the motorLoop"));

  #if(DRIVE_ID)
  Serial.print(F(":) readLogs\tread the eeprom logs"));
  Serial.println();
  Serial.print(F(":) clearLogs\tclear the eeprom logs with zeros"));
  Serial.println();
  #endif

  #if(DS3231_ID)
  Serial.print(F(":) readTime\tread the current time"));
  Serial.println();
  Serial.print(F(":) readTime10\tread the current time 10 times (ticking seconds)"));
  Serial.println();
  Serial.print(F(":) writeTime\tset the current time"));
  Serial.println();
  #endif

  Serial.print(F(":) config:all\tprint all settings"));
  Serial.println();
  Serial.print(F(":) config:pins\tprint pin designations and i2c devices"));
  Serial.println();
  Serial.print(F(":) config:battery\tprint battery levels"));
  Serial.println();
  Serial.print(F(":) config:drive\tprint drive config"));
  Serial.println();
  Serial.print(F(":) config:clock\tprint clock config"));
  Serial.println();
  Serial.print(F(":) config:temp\tprint temperature sensor config"));
  Serial.println();
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

void debugLoop(){
  if(isStepping){
    if(autoStep || (--stepCounter > 0)){
      logLoop();
      motorLoop();
    }
    return;
  }

  if(Serial.available() == 0){ return; }
  Serial.print(F(":] "));
  String cmd = Serial.readStringUntil(13);
  Serial.print(cmd);
  Serial.println();

  if(cmd == "dump"){ dataDump(); }

  if(cmd == "" or cmd == "?"){ printHelp(); }

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

  if(cmd == "readLogs"){ readDriveLogs(); }
  if(cmd == "clearLogs"){ clearDriveLogs(); }

  if(cmd == "step"){ debugStepping(); }

  delay(50);
}

#endif
#endif
