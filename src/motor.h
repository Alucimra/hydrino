#include <config.h>

#ifndef HYDRINO_MOTOR
#define HYDRINO_MOTOR

bool isMotorOn = false;

// TODO: Move the cycling code to a separate cycling.h (like we had before)
// TODO: Review the old cycle timing math, maybe cut down to just
//       OVERCHARGE, STANDARD, LOW options
unsigned long currentCycleStart = 0;
unsigned long currentCycleStop = 0;
unsigned long lastMotorLoopAt = 0;
bool loopAround = false;

#if DEBUG
  const unsigned long onCycleTime = 5000;
  const unsigned long offCycleTime = 20000;
#else
  const unsigned long onCycleTime = 30000;
  const unsigned long offCycleTime = 330000;
#endif

void startMotor(){
  digitalWrite(MOTOR_ON, LOW);
  isMotorOn = true;
  digitalWrite(LED_BUILTIN, HIGH);
  #if DEBUG
    Serial.println(F("call startMotor() completed"));
  #endif
}

void stopMotor(){
  digitalWrite(MOTOR_ON, HIGH);
  isMotorOn = false;
  digitalWrite(LED_BUILTIN, LOW);
  #if DEBUG
    Serial.println(F("call stopMotor() completed"));
  #endif
}


void motorLoop(){
  lastMotorLoopAt = millis();

  #if DEBUG
    Serial.println(F("inside motorLoop()"));
    Serial.print(F("\tlastMotorLoopAt = "));
    Serial.println(lastMotorLoopAt);
    Serial.print(F("\tloopAround = "));
    Serial.println(loopAround);
    Serial.print(F("\tcurrentCycleStart = "));
    Serial.println(currentCycleStart);
    Serial.print(F("\tcurrentCycleStop = "));
    Serial.println(currentCycleStop);
    Serial.println();
  #endif

  if(((!loopAround && lastMotorLoopAt > currentCycleStart) || (loopAround && lastMotorLoopAt < currentCycleStart)) && lastMotorLoopAt > currentCycleStop){
    if(isMotorOn){
      stopMotor();
      currentCycleStart = lastMotorLoopAt;
      currentCycleStop = currentCycleStart + offCycleTime;
    } else {
      startMotor();
      currentCycleStart = lastMotorLoopAt;
      currentCycleStop = currentCycleStart + onCycleTime;
    }
    loopAround = (currentCycleStop < currentCycleStart);
  }
  #if DEBUG
    else {
      Serial.println(F("\t...not ready."));
    }
  #endif

  #if DEBUG
    Serial.println(F("done. Sleeping..."));
  #endif

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode();
  sleep_disable();

}

#endif
