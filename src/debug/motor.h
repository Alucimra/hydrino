#include <config.h>
#include <motor.h>

#ifndef HYDRINO_DEBUG_MOTOR
#define HYDRINO_DEBUG_MOTOR

void debugMotorStop(){
  if(isMotorOn){
    stopMotor();
  }
}

void debugMotorStart(){
  if(!isMotorOn){
    startMotor();
  }
}

void debugMotorStatus(){
  Serial.println(F(":) Motor status"));
  Serial.print(F("\tisMotorOn = "));
  Serial.println(isMotorOn);
  Serial.print(F("\tonCycleCount = "));
  Serial.println(onCycleCount);
  Serial.print(F("\tlastMotorLoopAt = "));
  Serial.println(lastMotorLoopAt);

}


#endif
