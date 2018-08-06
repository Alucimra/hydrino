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
  uint8_t bytes[10];
};

typedef union log_entry log_entry_t;

void setLogPosition(){
  uint16_t check = LOG_MARKER_START - 1;
  bool isZero = false;

  #if DEBUG
    Serial.println(F("Setting log position..."));
  #endif

  while(!isZero && check <= LOG_MARKER_STOP){
    isZero = (EEPROM.read(++check) == 0);
  }

  if(!isZero){
    currentLogPos = LOG_MARKER_START;
  } else {
    currentLogPos = check;
  }

  #if DEBUG
    Serial.print(F("\t-> currentLogPos: "));
    Serial.println(currentLogPos);
  #endif
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes
void driveWrite(uint16_t index, uint8_t data[], uint8_t length){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write((uint8_t)(index >> 8)); // MSB
    Wire.write((uint8_t)(index & 0xFF)); // LSB
    uint8_t c;
    for ( c = 0; c < length; c++ )
      Wire.write(data[c]);
    Wire.endTransmission();
}

// not more than 30 or 32 bytes at a time!
void driveRead(uint16_t index, uint8_t buffer[], uint8_t length){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write((uint8_t)(index >> 8)); // MSB
    Wire.write((uint16_t)(index & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(DRIVE_ID, (uint16_t)length);
    uint8_t c = 0;
    for ( c = 0; c < length; c++ )
      if (Wire.available()) buffer[c] = Wire.read();
}


void writeLog(uint8_t logPos, log_entry_t* data){
  EEPROM.write(logPos, data->entry.cycles);
  if(logPos < LOG_MARKER_STOP){
    EEPROM.write(logPos+1, 0);
  } else {
    EEPROM.write(LOG_MARKER_START, 0);
  }

  uint16_t drive_index = (logPos - LOG_MARKER_START);

  uint8_t bytes[9];
  bytes[0] = data->bytes[0];
  bytes[1] = data->bytes[1];
  bytes[2] = data->bytes[2];
  bytes[3] = data->bytes[3];
  bytes[4] = data->bytes[5]; // Skips 5th byte of rtc_time, the useless dow
  bytes[5] = data->bytes[6];
  bytes[6] = data->bytes[7];
  bytes[7] = data->bytes[8];
  bytes[8] = data->bytes[9];

  driveWrite(drive_index * 9, bytes, 9);
}

log_entry_t readLog(uint8_t logPos){
  log_entry_t data;
  uint16_t drive_index = logPos - LOG_MARKER_START;

  uint8_t bytes[9];
  driveRead(drive_index * 9, bytes, 9);

  data.bytes[0] = bytes[0];
  data.bytes[1] = bytes[1];
  data.bytes[2] = bytes[2];
  data.bytes[3] = bytes[3];
  data.bytes[4] = 0;
  data.bytes[5] = bytes[4];
  data.bytes[6] = bytes[5];
  data.bytes[7] = bytes[6];
  data.bytes[8] = bytes[7];
  data.bytes[9] = bytes[8];

  return data;
}


void saveLogEntry(){
  log_entry_t data;
  rtc_datetime_t now = getTime();
  data.entry.unixtime = dateToUnixTimestamp(&now);
  data.entry.power = currentBatteryCharge();
  data.entry.temp = getTemperature();
  data.entry.cycles = onCycleCount;

  // outside of this range means setLogPosition did not function properly
  if(currentLogPos >= LOG_MARKER_START && currentLogPos <= LOG_MARKER_STOP){
    writeLog(currentLogPos, &data);
    if(currentLogPos < LOG_MARKER_STOP){
      currentLogPos++;
    } else {
      currentLogPos = LOG_MARKER_START;
    }
    lastLogAt = millis();
    Serial.print(F("Saved log: "));
    Serial.print(lastLogAt);
    Serial.println();
  }
  #if DEBUG
  else {
    Serial.print(F("saveLogEntry() failed. currentLogPos out of bounds"));
    Serial.println();
    Serial.print(F("\tcurrent\tmin\tmax"));
    Serial.println();
    Serial.print(currentLogPos);
    Serial.print(F("\t"));
    Serial.print(LOG_MARKER_START);
    Serial.print(F("\t"));
    Serial.print(LOG_MARKER_STOP);
    Serial.println();
  }
  #endif
  currentLogPos++;
}

void logLoop(){
  if(isMotorOn){ return; }

  uint32_t now = millis();
  if(now > (lastLogAt + LOG_EVERY) && ((lastLogAt + LOG_EVERY) > lastLogAt || now < lastLogAt)){
    saveLogEntry();
  }
}

#endif
#endif
