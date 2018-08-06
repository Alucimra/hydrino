#ifndef HYDRINO_CONFIG
#define HYDRINO_CONFIG
#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <EEPROM.h>


// Pins
#define MOTOR_ON 3
#define BATTERY_SENSE A0
//#define CHARGE_ON 9
//#define DEBUG_PIN

// flip the on-off status of motors (default is on is high, off is low)
// setting this variable to true flips it (on is low, off is high)
#define MOTOR_ON_FLIP true

#define DEBUG false
#if DEBUG
  #define DEBUG_DEEP true
  #define DEBUG_DRIVE true
#endif

/* Cycle time calculations
 * CYCLE_ON_TIME is the amount of time (microseconds) that the motor is on
 * CYCLE_OFF_MULT is the multiplier of CYCLE_ON_TIME where the motor is off
 * A full on-off cycle will then be ((1+CYCLE_OFF_MULT) * CYCLE_ON_TIME) long
 * For example, with CYCLE_ON_TIME = 30000 and CYCLE_OFF_MULT = 3, the motor
 * will be on for 30 seconds, and off for 90 seconds,
 * ie. it's on for 30 seconds every 2 minutes.
 */

#if DEBUG
  const uint32_t CYCLE_ON_TIME = 5000;
  const uint8_t CYCLE_OFF_MULT = 5;
#else
  const uint32_t CYCLE_ON_TIME = 30000;
  const uint8_t CYCLE_OFF_MULT = 13;
#endif

/* I started off using open-circuit numbers, but the battery voltage drops by
 * a few points once it's hooked to the arduino...so These numbers below are
 * adjusted for that slight drop.
 *
 * Open-cell (not plugged in):
 *   overcharge: 1010 (3.258 / 6.516)
 *   max: 1000 (3.23 / 6.46)
 *   flex: 985 (990=6.38, 985=6.36, 980=6.32)
 *   min: 930 (2.999 / 6.0)
 *
 * (multiply by 0.0032258 for divided or 0.0064516 for full)
 */
const uint16_t BATTERY_OVERCHARGE = 1000;
const uint16_t BATTERY_MAX = 990;
const uint16_t BATTERY_FLEX = 975;
const uint16_t BATTERY_MIN = 920;
const uint8_t CYCLE_FLEX_THRESHOLD = 3;


// Drive info (AT24C32)
#define DRIVE_ID 0x57
const uint16_t DRIVE_SPACE = 4096; //32768 bits;
const uint8_t DRIVE_WRITE_LIMIT = 0x30;
const uint8_t DRIVE_MARKER_BYTE = 0x4;


// RTC info (DS3231)
#define DS3231_ID 0x68


// Logs
#define LOG_ENABLE true
#if DEBUG
  const uint32_t LOG_EVERY = 60000;
  #if DEBUG_DRIVE
  const uint16_t LOG_MARKER_START = 560;
  const uint16_t LOG_MARKER_STOP = 568;
  #else
  const uint16_t LOG_MARKER_START = 569;
  const uint16_t LOG_MARKER_STOP = 1023;
  #endif
#else
  const uint32_t LOG_EVERY = 3600000;
  const uint16_t LOG_MARKER_START = 569;
  const uint16_t LOG_MARKER_STOP = 1023;
#endif














// HYDRINO_CONFIG
#endif
