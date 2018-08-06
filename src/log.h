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
  rtc_datetime_t datetime;
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

  while(!isZero && check <= LOG_MARKER_STOP){
    isZero = (EEPROM.read(++check) == 0);
  }

  if(!isZero){
    currentLogPos = LOG_MARKER_START;
  } else {
    currentLogPos = check;
  }
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

  driveWrite(drive_index * sizeof(log_entry_t), data->bytes, sizeof(log_entry_t));
}

log_entry_t readLog(uint8_t logPos){
  log_entry_t data;
  uint16_t drive_index = logPos - LOG_MARKER_START;

  driveRead(drive_index * sizeof(log_entry_t), data.bytes, sizeof(log_entry_t));
  return data;
}


void saveLogEntry(){
  log_entry_t data;
  data.entry.datetime = getTime();
  data.entry.power = currentBatteryCharge();
  data.entry.temp = getTemperature();
  data.entry.cycles = onCycleCount;

  writeLog(currentLogPos, &data);
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
