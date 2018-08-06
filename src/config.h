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
// set to true this variable flips it (on is low, off is high)
#define MOTOR_ON_FLIP true

#define DEBUG true

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

const uint16_t BATTERY_OVERCHARGE = 1010; //3.258v -> 6.516
const uint16_t BATTERY_MAX = 1000; // 3.23v -> 6.46
const uint16_t BATTERY_FLEX = 985; // 990=6.38 985= 6.35v 980 = 3.16v -> 6.32
const uint16_t BATTERY_MIN = 930; // 3.0v -> 6.0
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
  const uint32_t LOG_EVERY = 30000;
  const uint16_t LOG_MARKER_START = 560;
  const uint16_t LOG_MARKER_STOP = 568;
#else
  const uint32_t LOG_EVERY = 3600000;
  const uint16_t LOG_MARKER_START = 569;
  const uint16_t LOG_MARKER_STOP = 1023;
#endif














// HYDRINO_CONFIG
#endif
