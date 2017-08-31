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
  // 8/30/17 500ms delay doesn't do anything, readings are still bouncing.
  // stopMotor() calls inside the cycles, hopefully saves a few bytes
  stopMotor(motorA);
  stopMotor(motorB);
  delay(1000);

  digitalWrite(POWER_ACTIVATE, HIGH);
  delay(500);
  power = analogRead(POWER_CHECK);
  delay(100);
  digitalWrite(POWER_ACTIVATE, LOW);

  if(power > SOLAR + TOLERANCE){
    // AB6 AB6 AB6 AB6 1.00
    /**
     * NOTE: There is no battery protection circuit! We're over-charging!
     * This is pretty bad because if this happens early enough in the day, the
     * battery is going to be over-charged. These motors might not be enough to
     * use up all the solar power on a really bright day.
     *
     * Motors are rated at 3v 150mA or about 0.45W, both running should use ~0.9W
     * TODO: How much power do the motors draw here? (Real check).
     */
    runMotor(motorA, STRONG);
    runMotor(motorB, STRONG);
    cycle_time = 6;
  } else if(power > FULL + TOLERANCE){
    // *5 A4 *5 B4 0.44
    /**
     * NOTE: This is where most of the charging happens. A decent amount of the
     * off cycle is required for proper re-charging...but have to be careful
     * not to over charge because we don't have any protection!
     * This is why we're using LiFePO4 batteries, because they can handle the
     * added abuse of a little over-charging here and there.
     */
    if(cycle == 0 || cycle == 2){
      cycle_time = 5;
    } else if(cycle == 1){
      runMotor(motorA, STRONG);
      cycle_time = 4;
    } else if(cycle == 3){
      runMotor(motorB, STRONG);
      cycle_time = 4;
    }
  } else if(power > CHARGED + TOLERANCE){
    // *3 a2 *3 b2 40
    if(cycle == 0 || cycle == 2){
      cycle_time = 5;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
      cycle_time = 2;
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
      cycle_time = 2;
    }
  } else if(power > NOMINAL - TOLERANCE){
    // *3 a *3 b 25
    /**
     * NOTE: At this point (3.2v), we should still have a good chunk of the real
     * battery power (70-80% on a LiFePO4). Most of the night should be here.
     */
    if(cycle == 0 || cycle == 2){
      cycle_time = 3;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else if(power > DRAINED - TOLERANCE){
    // *15 a *15 b 6
    /**
     * NOTE: Voltage starts to drop sharply starting here; there's less power
     * than it looks.
     */
    if(cycle == 0 || cycle == 2){
      cycle_time = 15;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else if(power > CUTOFF){
    // *25 a *25 b 4
    /**
     * NOTE: There is almost no power left. Ideally this is where it will be in
     * the early morning, before the sun comes up. Used up most of the power
     * and leaves room for the battery to charge.
     */
    if(cycle == 0 || cycle == 2){
      cycle_time = 25;
    } else if(cycle == 1){
      runMotor(motorA, WEAK);
    } else if(cycle == 3){
      runMotor(motorB, WEAK);
    }
  } else {
    // *31 *31 *31 *31 0
    /**
     * NOTE: There's (almost) no power left. We need to conserve energy.
     * The chip stops working properly at 2.7-2.9v. The power led and clocks run
     * but it doesn't really work like it should, sometimes stalls!
     * It's operating out of spec when it reaches here. Should avoid.
     */
    cycle_time = 31;
  }
  saveCycle(power / 4, cycle_time);

  cycle = (cycle + 1) % 4;
  sleep_for = cycle_time * CYCLE_LENGTH;
  // sleep will be handled on next iteration of this loop
}

#endif
