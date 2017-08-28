#include <config.h>
#include <chipdata.h>

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


// cycle_time should be tweaked so that the battery should be around 3.0v
// by the time the sun starts shining (and recharging the battery).
// This will be affected by total sunlight (seasonal) and the battery, so will
// likely need seasonal updates.
// There's the expectation that odd cycle_time (except 1) means an off cycle.
// This expectation is used for graphing. See tools/chart.html
void actionLoop(){
  while(sleep_for > 0){ sleep_mode(); }

  uint8_t cycle_time = 1;
  int power = 0;

  // Current draw of motors affects voltage readings by 0.01-0.05v
  // It's not big, but I want to see if the analogRead numbers are more stable
  // with the voltage being read with motors off. Also lets us remove all the
  // stopMotor() calls inside the cycles, hopefully saves a few bytes
  stopMotor(motorA);
  stopMotor(motorB);
  delay(500);

  digitalWrite(POWER_ACTIVATE, HIGH);
  delay(300);
  power = analogRead(POWER_CHECK);
  delay(200);
  digitalWrite(POWER_ACTIVATE, LOW);

  if(power > SOLAR + TOLERANCE){
    // Power is higher than the solar panel, which means battery is over-charged.
    // In actual use, this should only happen when a new fully charged battery
    // was put in to replace the old battery. We use up power to bring it back down.
    runMotor(motorA, STRONG);
    runMotor(motorB, STRONG);
    cycle_time = 6;
  } else if(power > FULL + TOLERANCE){
    if(cycle == 0){
      cycle_time = 3;
    } else if(cycle == 1){
      runMotor(motorA, STRONG);
      cycle_time = 4;
    } else if(cycle == 2){
      cycle_time = 7;
    } else if(cycle == 3){
      runMotor(motorB, STRONG);
      cycle_time = 4;
    }
  } else if(power > CHARGED + TOLERANCE){
    if(cycle == 0 || cycle == 2){
      cycle_time = 5;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else if(power > NOMINAL - TOLERANCE){
    if(cycle == 0 || cycle == 2){
      cycle_time = 9;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else if(power > DRAINED - TOLERANCE){
    if(cycle == 0 || cycle == 2){
      cycle_time = 19;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else if(power > CUTOFF){
    if(cycle == 0 || cycle == 2){
      cycle_time = 59;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else {
    cycle_time = 21;
  }
  saveCycle(power / 4, cycle_time);

  cycle = (cycle + 1) % 4;
  sleep_for = cycle_time * CYCLE_LENGTH;
  // sleep will be handled on next iteration of this loop
}

#endif
