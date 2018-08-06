#include <config.h>
#include <cycling.h>

#ifndef HYDRINO_MOTOR
#define HYDRINO_MOTOR

bool isMotorOn = false;
bool loopAround = false;
unsigned long lastMotorLoopAt = 0;
uint8_t onCycleCount = 0;

void startMotor(){
  if(currentBatteryCharge() > BATTERY_MIN){
    #if MOTOR_ON_FLIP
    digitalWrite(MOTOR_ON, LOW);
    #else
    digitalWrite(MOTOR_ON, HIGH);
    #endif
  }
  #if DEBUG_DEEP
  else {
    Serial.println(F("\tnot really turning motor on, current charge less than min"));
  }
  #endif

  isMotorOn = true;
  digitalWrite(LED_BUILTIN, HIGH);

  #if DEBUG_DEEP
    Serial.println(F("\tstartMotor() completed"));
  #endif
}

void stopMotor(){
  #if MOTOR_ON_FLIP
  digitalWrite(MOTOR_ON, HIGH);
  #else
  digitalWrite(MOTOR_ON, LOW);
  #endif
  isMotorOn = false;
  digitalWrite(LED_BUILTIN, LOW);
  #if DEBUG_DEEP
    Serial.println(F("\tstopMotor() completed"));
  #endif
}

void motorLoop(){
  lastMotorLoopAt = millis();

  #if DEBUG_DEEP
    Serial.println(F("inside motorLoop()"));
    Serial.print(F("\tlastMotorLoopAt = "));
    Serial.println(lastMotorLoopAt);
    Serial.print(F("\tloopAround = "));
    Serial.println(loopAround);
    Serial.print(F("\tcurrentCycleStart = "));
    Serial.println(currentCycleStart);
    Serial.print(F("\tcurrentCycleStop = "));
    Serial.println(currentCycleStop);
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
      onCycleCount++;
    }
    loopAround = (currentCycleStop < currentCycleStart);
  }
  #if DEBUG_DEEP
    else {
      Serial.println(F("\t...not ready."));
    }
  #endif

  #if DEBUG_DEEP
    /* In the SLEEP_MODE_IDLE, UART interrupts (from serial) wakes the system
     * this means the Arduino never gets a chance to actually sleep.
     * We use the blocking delay() instead to emulate sleep when in debug mode.
     */
    Serial.println(F("\tdone."));
    Serial.println(F("Sleeping...*note: debug mode uses delay()"));
    Serial.println();
    delay(4090);
  #else
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    sleep_disable();
  #endif
}

#endif
