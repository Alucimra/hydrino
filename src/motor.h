#include <config.h>
#include <cycling.h>

#ifndef HYDRINO_MOTOR
#define HYDRINO_MOTOR

bool isMotorOn = false;
bool loopAround = false;
unsigned long lastMotorLoopAt = 0;

void startMotor(){
  digitalWrite(MOTOR_ON, HIGH);
  isMotorOn = true;
  digitalWrite(LED_BUILTIN, HIGH);
  #if DEBUG
    Serial.println(F("call startMotor() completed"));
  #endif
}

void stopMotor(){
  digitalWrite(MOTOR_ON, LOW);
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
      currentCycleStop = getOffCycleTime(currentCycleStart);
    } else {
      startMotor();
      currentCycleStart = lastMotorLoopAt;
      currentCycleStop = getOnCycleTime(currentCycleStart);
    }
    loopAround = (currentCycleStop < currentCycleStart);
  }
  #if DEBUG
    else {
      Serial.println(F("\t...not ready."));
    }
  #endif

  #if DEBUG
    /* In the SLEEP_MODE_IDLE, UART interrupts (from serial) wakes the system
     * this means the Arduino never gets a chance to actually sleep.
     * We use the blocking delay() instead to emulate sleep when in debug mode.
     */
    Serial.println(F("done. Sleeping...*note: debug mode uses delay()"));
    delay(4090);
  #else
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    sleep_disable();
  #endif
}

#endif
