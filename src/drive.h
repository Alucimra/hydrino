#include <config.h>

#ifndef HYDRINO_DRIVE
#define HYDRINO_DRIVE

void clearDrive(){
  power_twi_enable();
  delay(1000);
  unsigned int written = 0;

  while(written < DRIVE_SPACE){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write(written >> 8);
    Wire.write(written & 0xFF);
    for(uint8_t i=0; i < DRIVE_WRITE_LIMIT; i++){
      Wire.write(0);
      written++;
    }
    Wire.endTransmission();
    delay(20); // with the 20ms delay, it takes ~26.2 seconds to clear the drive
  }
  power_twi_disable();
  delay(50);
}


void saveToDrive(){
  power_twi_enable();
  delay(1000);
  uint8_t marker = EEPROM.read(2);

  if(SAVE_TO_DRIVE_AT*(marker+1) > DRIVE_SPACE){ return; } // the drive is full!

  unsigned int address = SAVE_TO_DRIVE_AT*marker;
  unsigned int written = 0;

  while(written < 1000){
    Wire.beginTransmission(DRIVE_ID);
    Wire.write((address+written) >> 8);
    Wire.write((address+written) & 0xFF);
    for(uint8_t i=0; i < DRIVE_WRITE_LIMIT; i++){
      Wire.write(EEPROM.read(written+logStart));
      written++;
    }
    // NOTE: i2c transmissions are limited to 32 bytes, first 2 is used for addr
    // so we can only write 30 bytes max in the loop above.
    Wire.endTransmission();
    delay(20);
  }
  EEPROM.write(2, marker+1);
  power_twi_disable();
}

#endif
