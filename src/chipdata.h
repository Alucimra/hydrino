#include <config.h>
#include <drive.h>
#include <clock.h>

#ifndef HYDRINO_CHIPDATA
#define HYDRINO_CHIPDATA

void loadDrivePosition(){
  drivePos = EEPROM.read(4);
}

void saveReserveData(){
  uint8_t second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  uint32_t bitwiseDate = dateToBitwiseDate(second, minute, hour, dayOfMonth, month, year);
  uint8_t a, b, c, d;
  splitBitwiseDate(bitwiseDate, &a, &b, &c, &d);
  EEPROM.write(0, a);
  EEPROM.write(1, b);
  EEPROM.write(2, c);
  EEPROM.write(3, d);

  // NOTE: Make sure you load the drivePos before saving if you haven't
  EEPROM.write(4, drivePos);

  EEPROM.write(5, 0);
}

void clearLogs(){
  for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  logPos = logStart;
  drivePos = 0;
}

/**
 * We separate out the drive write to save us some power. the TWI (I2C) clock
 * was disabled in setup() so if we wanted to write directly to the drive EEPROM
 * then we would need to keep the TWI clock on. It's a small amount, but not 0.
 * I'm writing to the chip EEPROM first, and then transferring in chunks of 1020
 * bytes (+4 bytes for current time). This saves power at the expense of more
 * chip EEPROM cycles.
 */
#define CYCLE_SIZE 0x2
void saveCycle(uint8_t power, uint8_t cycle_time){
  // don't save data when we're debugging
  if(isDebugging){ return; }

  // FIXME: theoretically, this should not ever shappen...
  //if(logPos % CYCLE_SIZE != logStart % CYCLE_SIZE){ logPos++; }

  /**
   * logPos at index 1023 means we're at the 1024th byte (EEPROM.length())
   * We need to make sure there are at least 2 bytes available to write.
   * eg, at 1021+2 >= 1024 (f) but 1022+2 >= 1024 (t)
   * This should be avoided since we have the drive backup in place.
   */
  if((logPos + CYCLE_SIZE) >= EEPROM.length()){ logPos = logStart; }

  /**
   * The power on odd byte, then cycle+cycle_time next (even).
   * The conditional is there to make sure that is always the case, even if more
   * or less bytes get reserved in the future.
   * NOTE: power has been scaled down from 1023 to 255 to save space.
   */
  uint8_t cycling = (cycle_time << 2) + cycle;
  if(logStart % 2 == 0){
    EEPROM.write(logPos++, cycling);
    EEPROM.write(logPos++, power);
  } else {
    EEPROM.write(logPos++, power);
    EEPROM.write(logPos++, cycling);
  }
  delay(500);

  /**
   * Write zeros to keep our position since we no longer write logPos.
   * Obviously, if we're already at the end, then we can't write ahead 2 bytes.
   * NOTE: This conditional should be *inverted* of logPos reset check above
   */
  if((logPos + CYCLE_SIZE) < EEPROM.length()){
    EEPROM.write(logPos, 0);
    EEPROM.write(logPos + 1, 0);
    delay(500);
  }

  #if(HAS_DRIVE)
  /**
   * If we're at the end, then save to drive.
   * NOTE: This conditional should be the same as the logPos reset check above.
   */
  if((logPos + CYCLE_SIZE) >= EEPROM.length()){
    saveToDrive();
    drivePos++;
    saveReserveData();
    logPos = logStart;
  }
  #endif
}

#endif
