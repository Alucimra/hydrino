#include <config.h>
#include <motor.h>
#include <clock.h>
#include <battery.h>

#if(LOG_ENABLE)
#ifndef HYDRINO_LOG
#define HYDRINO_LOG

uint32_t lastLogAt = 0;
uint16_t currentLogPos = 0;

struct log_element {
  uint32_t unixtime;
  rtc_temp_t temp;
  uint16_t power;
  uint8_t cycles;
};

typedef struct log_element log_element_t;

union log_entry {
  struct log_element entry;
  uint8_t bytes[sizeof(log_element_t)];
};

typedef union log_entry log_entry_t;

void setLogPosition(){
  uint16_t check = LOG_MARKER_START - 1;
  bool isZero = false;

  #if DEBUG_DEEP
    Serial.println(F("Setting log position..."));
  #endif

  while(!isZero && check < LOG_MARKER_STOP){
    isZero = (EEPROM.read(++check) == 0);
  }

  if(!isZero){
    currentLogPos = LOG_MARKER_START;
  } else {
    currentLogPos = check;
  }

  #if DEBUG_DEEP
    Serial.print(F("\t-> currentLogPos: "));
    Serial.println(currentLogPos);
  #endif
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes
void driveWritePage(uint16_t index, uint8_t data[], uint8_t offset, uint8_t length){
  Wire.beginTransmission(DRIVE_ID);
  if(Wire.endTransmission() == 0){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write((uint8_t)(index >> 8)); // MSB
    Wire.write((uint8_t)(index & 0xFF)); // LSB
    uint8_t *addr = data+offset;
    Wire.write(addr, length);
    Wire.endTransmission();
    delay(20);
  }
}

// AT24C32 (and most EEPROMs) stores data in 32 pages.
// If (index+length)/32 != (index/32) under integer math, then we need to split
// the write into their corresponding pages.
// If we don't, it'll overflow, loop around and overwrite other bytes in the page!
void driveWrite(uint16_t index, uint8_t data[], uint8_t length){
  int offsetPointer = 0;
  int offsetPage;
  int nextCount = 0;
  while(length > 0){
    offsetPage = index % 32;
    nextCount = min(min(length, 30), 32 - offsetPage);
    driveWritePage(index, data, offsetPointer, nextCount);
    length -= nextCount;
    offsetPointer += nextCount;
    index += nextCount;
  }
}

// not more than 30 or 32 bytes at a time!
void driveRead(uint16_t index, uint8_t buffer[], uint8_t length){
  Wire.beginTransmission(DRIVE_ID);
  if(Wire.endTransmission() == 0){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write((uint8_t)(index >> 8)); // MSB
    Wire.write((uint16_t)(index & 0xFF)); // LSB
    if(Wire.endTransmission() == 0){
      Wire.requestFrom(DRIVE_ID, (uint16_t)length);
      uint8_t c = 0;
      while(Wire.available() && c < length){
        buffer[c] = (uint8_t)Wire.read();
        c++;
      }
    }
  }
}


void writeLog(uint16_t logPos, log_entry_t* data){
  EEPROM.update(logPos, data->entry.cycles);
  if(logPos < LOG_MARKER_STOP){
    EEPROM.update(logPos+1, 0);
  } else {
    EEPROM.update(LOG_MARKER_START, 0);
  }

  uint16_t drive_index = (logPos - LOG_MARKER_START);

  driveWrite(drive_index * sizeof(log_element_t), data->bytes, sizeof(log_element_t));
}

log_entry_t readLog(uint16_t logPos){
  log_entry_t data;
  uint16_t drive_index = logPos - LOG_MARKER_START;

  driveRead(drive_index * sizeof(log_element_t), data.bytes, sizeof(log_element_t));

  return data;
}


void saveLogEntry(){
  log_entry_t data;
  rtc_datetime_t now = getTime();
  data.entry.unixtime = dateToUnixTimestamp(&now);
  data.entry.power = currentBatteryCharge();
  data.entry.temp = getTemperature();
  data.entry.cycles = onCycleCount == 0 ? 1 : onCycleCount;

  // outside of this range means setLogPosition did not function properly
  if(currentLogPos >= LOG_MARKER_START && currentLogPos <= LOG_MARKER_STOP){
    #if DEBUG_DEEP
      Serial.println(F("Saving log... lastLogAt -> currentLogPos - cycles"));
      Serial.print(F("\t"));
      Serial.print(lastLogAt);
      Serial.print(F(" -> "));
      Serial.print(currentLogPos);
      Serial.print(F(" - "));
      Serial.print(data.entry.cycles);
      Serial.println();
    #endif
    writeLog(currentLogPos, &data);
    if(currentLogPos < LOG_MARKER_STOP){
      currentLogPos++;
    } else {
      currentLogPos = LOG_MARKER_START;
    }
    lastLogAt = millis();
    onCycleCount = 0;
    #if DEBUG_DEEP
    Serial.println(F("Saved log. lastLogAt -> (new)currentLogPos"));
    Serial.print(F("\t"));
    Serial.print(lastLogAt);
    Serial.print(F(" -> "));
    Serial.print(currentLogPos);
    Serial.println();
    #endif
  }
  #if DEBUG_DEEP
  else {
    Serial.print(F("saveLogEntry() failed. currentLogPos out of bounds"));
    Serial.println();
    Serial.print(F("\tcurrent\tmin\tmax"));
    Serial.println();
    Serial.print(F("\t"));
    Serial.print(currentLogPos);
    Serial.print(F("\t"));
    Serial.print(LOG_MARKER_START);
    Serial.print(F("\t"));
    Serial.print(LOG_MARKER_STOP);
    Serial.println();
  }
  #endif
}

void logLoop(){
  if(isMotorOn){ return; }

  uint32_t now = millis();
  #if DEBUG_DEEP
    Serial.println(F("logLoop\tnow\tlastLogAt\tLOG_EVERY"));
    Serial.print(now > (lastLogAt + LOG_EVERY) && ((lastLogAt + LOG_EVERY) > lastLogAt || now < lastLogAt));
    Serial.print(F("\t"));
    Serial.print(now);
    Serial.print(F("\t"));
    Serial.print(lastLogAt);
    Serial.print(F("\t"));
    Serial.print(LOG_EVERY);
    Serial.println();
  #endif
  if(now > (lastLogAt + LOG_EVERY) && ((lastLogAt + LOG_EVERY) > lastLogAt || now < lastLogAt)){
    saveLogEntry();
  }
}

#endif
#endif
