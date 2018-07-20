#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <EEPROM.h>

#ifndef HYDRINO_CONFIG
#define HYDRINO_CONFIG

// Pins
#define MOTOR_ON 3
#define BATTERY_CHECK 2
#define BATTERY_SENSE A3
#define CHARGE_ON 9
//#define DEBUG_PIN

#define DEBUG false

/* Cycle time calculations
 * CYCLE_ON_TIME is the amount of time (microseconds) that the motor is on
 * CYCLE_OFF_MULT is the multiplier of CYCLE_ON_TIME where the motor is off
 * A full on-off cycle will then be ((1+CYCLE_OFF_MULT) * CYCLE_ON_TIME) long
 * For example, with CYCLE_ON_TIME = 30000 and CYCLE_OFF_MULT = 3, the motor
 * will be on for 30 seconds, and off for 90 seconds,
 * ie. it's on for 30 seconds every 2 minutes.
 */
const unsigned long CYCLE_ON_TIME = 30000;
const unsigned long CYCLE_OFF_MULT = 9;

// HYDRINO_CONFIG
#endif
