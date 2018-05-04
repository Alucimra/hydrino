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

bool MOTOR_DEBUG = true;
const unsigned long onCycleTime = MOTOR_DEBUG ? 5000 : 30000;
const unsigned long offCycleTime = MOTOR_DEBUG ? 20000 : 330000;


void startMotor(){
  digitalWrite(MOTOR_ON, HIGH);
  isMotorOn = true;
  digitalWrite(LED_BUILTIN, HIGH);
}

void stopMotor(){
  digitalWrite(MOTOR_ON, LOW);
  isMotorOn = false;
  digitalWrite(LED_BUILTIN, LOW);
}


void motorLoop(){
  lastMotorLoopAt = millis();

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

  sleep_mode();
}

#endif
