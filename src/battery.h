#include <config.h>

#ifndef HYDRINO_BATTERY
#define HYDRINO_BATTERY

uint16_t currentBatteryCharge(){

  #if DEBUG
    Serial.println(F("currentBatteryCharge: "));
    Serial.println(analogRead(BATTERY_SENSE));
  #endif

  return analogRead(BATTERY_SENSE);
}

#endif
