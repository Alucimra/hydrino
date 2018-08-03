#include <config.h>

#ifndef HYDRINO_DEBUG_STEPPING
#define HYDRINO_DEBUG_STEPPING

uint16_t stepCounter = 0;
bool autoStep = false;
bool isStepping = false;

void debugStepping(){
  Serial.print(F(":) How many sleep cycles to step? (up to 32,767 or 0 for unlimited, negative to cancel) :] "));
  stepCounter = Serial.parseInt();
  if(stepCounter < 0){
    isStepping = false;
    Serial.print(F(":| Cancelled."))
  } else if(stepCounter == 0){
    isStepping = true;
    autoStep = true;
    Serial.print(F(":) Ok, exiting debug command console"));
  } else {
    isStepping = true;
    autoStep = false;
    Serial.print(F(":) Ok, stepping for "));
    Serial.print(stepCounter);
    Serial.print(F(" cycles"))
    Serial.println();
  }
}

#endif
