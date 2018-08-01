#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Wire.h>
//#include <EEPROM.h>

#ifndef HYDRINO_CONFIG
#define HYDRINO_CONFIG

// Pins
#define MOTOR_ON 3
#define BATTERY_SENSE A0
//#define CHARGE_ON 9
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
const uint32_t CYCLE_ON_TIME = 30000;
const uint8_t CYCLE_OFF_MULT = 13;
const uint16_t BATTERY_OVERCHARGE = 1010;
const uint16_t BATTERY_MAX = 1000;
const uint16_t BATTERY_FLEX = 980;
const uint16_t BATTERY_MIN = 940;
const uint8_t CYCLE_FLEX_THRESHOLD = 5;

// HYDRINO_CONFIG
#endif
