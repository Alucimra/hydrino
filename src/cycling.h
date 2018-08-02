#include <config.h>
#include <battery.h>

#ifndef HYDRINO_CYCLING
#define HYDRINO_CYCLING

uint32_t currentCycleStart = 0;
uint32_t currentCycleStop = 0;
uint8_t thresholdJump = (BATTERY_MAX - BATTERY_FLEX) / CYCLE_FLEX_THRESHOLD;

uint32_t getOffCycleTime(uint32_t cycleStart){
  uint16_t charge = currentBatteryCharge();

  #if DEBUG
    Serial.println("getOffCycleTime: ");
  #endif

  if(charge >= BATTERY_OVERCHARGE){
    #if DEBUG
      Serial.println(cycleStart + 1);
    #endif
    return cycleStart + 1;
  }

  if(charge >= BATTERY_MAX){
    #if DEBUG
      Serial.println(cycleStart + CYCLE_ON_TIME);
    #endif
    return cycleStart + CYCLE_ON_TIME;
  }

  if(charge >= BATTERY_FLEX){
    #if DEBUG
      Serial.println(cycleStart + (
        (CYCLE_OFF_MULT - ((charge - BATTERY_FLEX) / thresholdJump) )
          * CYCLE_ON_TIME
        ));
    #endif
    return cycleStart + (
      (CYCLE_OFF_MULT - ((charge - BATTERY_FLEX) / thresholdJump) )
        * CYCLE_ON_TIME
      );
  }

  if(charge >= BATTERY_MIN){
    #if DEBUG
      Serial.println(cycleStart + (CYCLE_OFF_MULT * CYCLE_ON_TIME));
    #endif
    return cycleStart + (CYCLE_OFF_MULT * CYCLE_ON_TIME);
  }

  #if DEBUG
    Serial.println(cycleStart + (CYCLE_OFF_MULT * CYCLE_ON_TIME));
  #endif
  // We are below the battery minimum
  return cycleStart + (CYCLE_OFF_MULT * CYCLE_ON_TIME);
}

uint32_t getOnCycleTime(uint32_t cycleStart){
  return cycleStart + (currentBatteryCharge() <= BATTERY_MIN ? 1 : CYCLE_ON_TIME);
}

#endif
