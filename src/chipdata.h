#include <config.h>

#ifndef HYDRINO_CHIPDATA
#define HYDRINO_CHIPDATA

void loadLogPosition(){
  // TODO: No longer keep logPos in EEPROM. Saving eats up write cycles too quickly.
  // You should probably clearLogs() before using it the first time
  // Go into debug mode and send '0' (the string) to console
  uint8_t high_byte = EEPROM.read(0);
  uint8_t low_byte = EEPROM.read(1);
  logPos = high_byte * 256 + low_byte;
  if(logPos < logStart){ logPos = logStart; }
}

void clearLogs(){
  for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}



// TODO: See loadLogPosition() Keep logPos in ram only to save EEPROM cycles
/**
 * We separate out the drive write to save us some power. the TWI (I2C) clock
 * was disabled in setup() so if we wanted to write directly to the drive EEPROM
 * then we would need to keep the TWI clock on. It's a small amount, but not 0.
 * I'm writing to the chip EEPROM first, and then transferring in chunks of 1020
 * bytes (+4 bytes for current time). This saves power at the expense of more
 * chip EEPROM cycles.
 */
void saveCycle(uint8_t power, uint8_t cycle_time){
  // don't save data when we're debugging
  if(isDebugging){ return; }

  // we save the power on odd byte, then cycle+cycle_time next (even)
  // NOTE: power has been scaled down from 1023 to 255 to save space, lost res

  // this should not happen...we should always be on an odd byte when called
  if(logPos % 2 == 0){ logPos++; }

  // we will overflow, cycle back around to the start
  // this shouldn't happen with the extra drive backup in place
  if((logPos + 2) >= EEPROM.length()){ logPos = logStart; }

  uint8_t cycling = (cycle_time << 2) + cycle;
  EEPROM.write(logPos++, power);
  EEPROM.write(logPos++, cycling);
  delay(500);
  EEPROM.write(0, (uint8_t)(logPos >> 8));
  EEPROM.write(1, (uint8_t)(logPos));
  delay(500);

  #if(HAS_DRIVE)
  if(logPos == SAVE_TO_DRIVE_AT+logStart){
    // we wrote 1000 bytes (500 cycles) save to drive
    saveToDrive();
    // and then cycle back to the beginning for the logs
    logPos = logStart;
  }
  #endif
}

#endif
