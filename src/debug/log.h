#include <config.h>
#include <log.h>
#include <clock.h>

#ifndef HYDRINO_DEBUG_DRIVE
#define HYDRINO_DEBUG_DRIVE

void displayLog(log_entry_t *data){
  Serial.print(data->entry.unixtime);
  Serial.print(F("\t"));
  displayTemperature(&data->entry.temp);
  Serial.print(F("\t"));
  Serial.print(data->entry.power);
  Serial.print(F("\t"));
  Serial.print(data->entry.cycles);
  Serial.println();
}

void readDriveLogs(){
  Serial.println(F(":) Reading drive logs (tsv format) drive_logs:> "));
  Serial.println(F("Unixtime\tTemp\tPower\tCycles"));
  uint8_t marker = 0;
  log_entry_t entry;
  for(uint16_t i = LOG_MARKER_START; i <= LOG_MARKER_STOP; i++){
    marker = EEPROM.read(i);
    if(marker > 0){
      entry = readLog(i);
      displayLog(&entry);
    }
  }
  Serial.println();
  Serial.println(F(":) Done."));
}

void debugDriveLogs(){
  Serial.println(F(":) Debug drive logs (tsv format) drive_logs:> "));
  Serial.println(F("logPos\tMarker\tUnixtime\tTemp\tPower\tCycles"));
  uint8_t marker = 0;
  log_entry_t entry;
  for(uint16_t i = LOG_MARKER_START; i <= LOG_MARKER_STOP; i++){
    marker = EEPROM.read(i);
    Serial.print(i);
    Serial.print(F("\t"));
    Serial.print(marker);
    Serial.print(F("\t"));
    entry = readLog(i);
    displayLog(&entry);
  }
  Serial.println();
  Serial.println(F(":) Done."));
}

void clearDriveLogs(){
  Serial.println(F(":) Force clearing all drive logs markers... *! This is usually not necessary !*"));
  Serial.println(F(":} !*! CAUTION: This wears down the EEPROM! Do NOT use too often !*!"));
  for(uint16_t i = LOG_MARKER_START; i <= LOG_MARKER_STOP; i++){
    EEPROM.write(i, 0);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F(":) Done."));
}

#endif
