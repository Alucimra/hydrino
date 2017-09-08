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
    delay(50); // with a 20ms delay, it takes ~26.2 seconds to clear the drive
  }
  power_twi_disable();
  delay(50);
}


void saveToDrive(){
  power_twi_enable();
  delay(1000);
  unsigned int dataSize = EEPROM.length();

  // Checks if the drive is full, and don't do anything if it is.
  if(drivePos + dataSize > DRIVE_SPACE){ return; }

  unsigned int address = dataSize*drivePos;
  unsigned int written = 0;

  while(written < dataSize) {
    Wire.beginTransmission(DRIVE_ID);

    /**
     * Hack to make sure we have a handle on the EEPROM. Otherwise transmissions
     * might be ignored. (Ends up skipping a chunk of data.)
     */
    if(Wire.endTransmission() == 0){
      Wire.beginTransmission(DRIVE_ID);
      Wire.write((address+written) >> 8);
      Wire.write((address+written) & 0xFF);
      for(uint8_t i=0; i < DRIVE_WRITE_LIMIT; i++){
        Wire.write(EEPROM.read(written));
        written++;
      }
      // NOTE: i2c transmissions are limited to 32 bytes, first 2 is used for addr
      // so we can only write 30 bytes max in the loop above.
      Wire.endTransmission();
    }
    delay(50);
  }

  power_twi_disable();
}

#endif
