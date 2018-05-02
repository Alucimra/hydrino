#include <config.h>
#include <chipdata.h>
#include <motor.h>
#include <debug.h>

ISR(TIMER1_OVF_vect){
  // we woke up (used 1 sleep cycle)
  currentCycle.sleep_cycles++;
}

void startup(){
  // First few readings after changing analogReference is unreliable, dump them
  // TODO: These delays are probably unnecessary
  analogRead(POWER_CHECK);
  delay(500);
  analogRead(POWER_CHECK);
  delay(300);
  analogRead(POWER_CHECK);
  delay(200);
  analogRead(POWER_CHECK);
  // end analogReference setup

  loadDrivePosition();
  saveReserveData();

  if(digitalRead(DEBUG_PIN) == LOW){
    isDebugging = true;
    Serial.begin(9600);
    delay(500);
    Serial.print(F(":) Debugging activated."));
    Serial.println();
  }

  // remove the pullup, conserve some power (hopefully)
  pinMode(DEBUG_PIN, INPUT);
}

void setup(){
  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);
  pinMode(POWER_CHECK, INPUT);
  pinMode(POWER_ACTIVATE, OUTPUT);
  pinMode(DEBUG_PIN, INPUT_PULLUP);
  analogReference(INTERNAL);
  analogRead(POWER_CHECK); // read and drop

  // Disable I2C and SPI since we're not using it.
  // TODO: disable additional timers? (0 and 2 look like prime targets)
  // but are they needed for analog ouput? (since it's pwm)
  set_sleep_mode(SLEEP_MODE_IDLE);
  power_twi_disable();
  power_spi_disable();
  // timers setup
  // http://donalmorrissey.blogspot.com/2011/11/sleeping-arduino-part-4-wake-up-via.html
  TCCR1A = 0x00;
  TCNT1 = 0x0000;
  TCCR1B = 0x05; // 4.09s
  TIMSK1 = 0x01; // enable interrupt

  startup();
}

void loop(){
  if(isDebugging){
    debugLoop();
  } else {
    actionLoop();
  }
}
