#include <config.h>
#include <chipdata.h>
#include <power.h>

#ifndef HYDRINO_MOTOR
#define HYDRINO_MOTOR

void runMotor(bool useMotorA, uint8_t power){
  if(useMotorA && motorArunning){
    // motor A - running - change the power level
    analogWrite(MOTOR_A, power);
  } else if(!useMotorA && motorBrunning){
    // motor B - running - change the power Level
    analogWrite(MOTOR_B, power);
  } else if(useMotorA && !motorArunning){
    analogWrite(MOTOR_A, MAX);
    delay(MOTOR_START_WAIT);
    analogWrite(MOTOR_A, power);
    motorArunning = true;
  } else if(!useMotorA && !motorBrunning){
    analogWrite(MOTOR_B, MAX);
    delay(MOTOR_START_WAIT);
    analogWrite(MOTOR_B, power);
    motorBrunning = true;
  }
}

void stopMotor(bool useMotorA){
  if(useMotorA){
    digitalWrite(MOTOR_A, LOW);
    motorArunning = false;
  } else {
    digitalWrite(MOTOR_B, LOW);
    motorBrunning = false;
  }
}

void powerCheck(){
  int power = 0;

  digitalWrite(POWER_ACTIVATE, HIGH);
  delay(500);
  power = analogRead(POWER_CHECK);
  delay(100);
  digitalWrite(POWER_ACTIVATE, LOW);

  currentCycle.power = power;
}

// cycle_time should be tweaked so that the battery should be around 3.0v
// by the time the sun starts shining (and recharging the battery).
// This will be affected by total sunlight (seasonal) and the battery, so will
// likely need seasonal updates.
// There's the expectation that odd cycle_time (except 1) means an off cycle.
// This expectation is used for graphing. See tools/chart.html
void actionLoop(){
  unsigned long long now = millis();
  while(now < currentCycle.sleep_until || (currentCycle.sleep_until < currentCycle.sleep_start && now > currentCycle.sleep_start))
  {
    sleep_mode();
  }

  stopMotor(motorA);
  stopMotor(motorB);

  if(currentCycle.power == 0){
    powerCheck();

    for(uint8_t i=0; i < (sizeof(timings)/sizeof(timings[0])); i++){
      if(currentCycle.power > timings[i]->power+TOLERANCE){
        cycleTime = timings[i];
        break;
      }
    }
  }

  currentCycle.sleep_start = millis();
  currentCycle.sleep_until = cycleTime.sleep_start + (cycleTime.)

  currentCycle.cycle = (currentCycle.cycle + 1) % 4;

  #if LOG_POWER
//  saveCycle();
  #endif

  // if we looped around, reset power so it will redo the timing selection
  if(currentCycle.cycle == 0){ currentCycle.power = 0; }

  // sleep will be handled on next iteration of this loop
}

#endif
