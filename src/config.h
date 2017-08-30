#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <EEPROM.h>

#ifndef HYDRINO_CONFIG
#define HYDRINO_CONFIG

// Pins
#define MOTOR_A A1
#define MOTOR_B A3
#define POWER_CHECK A2
#define POWER_ACTIVATE 11
#define DEBUG_PIN 9

// TODO: Since we'll be getting a DS1307 module with an included AT24C32, we
// should implement the code to read the time

// Drive info (AT24C32)
// TODO: We could use this HAS_DRIVE preprocessor to skip function definitions
// because we won't be calling them anyway.
#define HAS_DRIVE true
#define DRIVE_ID 0x57
#define DRIVE_SPACE 32768
#define DRIVE_WRITE_LIMIT 0x30
#define DRIVE_MARKER_BYTE 0x4

// RTC info (DS3231)
#define HAS_CLOCK true
#define CLOCK_ID 0x68


// motor power Levels
const uint8_t MAX = 255; // 1023/1023
const uint8_t STRONG = 150; // 140 = 90-100mA 560/1023
const uint8_t WEAK = 135; // 120 = 75-90mA 480/1023, if too low, motor stalls!
const uint8_t OFF = 0;
const int MOTOR_START_WAIT = 1000;

bool motorArunning = false;
bool motorBrunning = false;
uint8_t cycle = 0;
const bool motorA = true;
const bool motorB = false;

/**
 * Voltage Levels
 * WARNING: Assumes internal reference voltage is 1.0v. See docs/voltage levels.txt
 * divisor = fullResistance / potResistance
 * aRead = (battery_voltage / divisor) / (vref / 1023)
 * batteryVoltage = aRead * divisor * (vref / 1023)
 * vref = (1023 * battery_voltage) / (divisor * aRead)
 * multiplier = 4 * divisor * (vref / 1023)
 *
 * Real life read checks:
 * 191*4 = 764 = ~3.33 (8/27/17) vref = 1.12v
 * 190*4 = 760 = ~3.28 (8/30/17) vref = 1.10v
 *
 * Changes:
 * vref = 1.11 (8/30/17)
 * divisor = 9.75 / 2.45 = ~3.9795918367 (8/30/17)
 * multiplier = ~0.01727213 (below, 8/30/17)
 */
const int OVERCHARGE = 810; // between 3.6v, 3.7
const int SOLAR = 800;      // 3.6v
const int FULL = 787;       // 3.4v
const int CHARGED = 764;    // 3.3v
const int NOMINAL = 741;    // 3.2v
const int DRAINED = 717;    // 3.1v
const int CUTOFF = 680;     // 3.0v
const int TOLERANCE = 23;   // 12=0.05v 23 = 0.1
const float VOLTAGE_MULTIPLIER = 4.0 * (9.75/2.45) * (1.11/1023);

/**
 * We have 1024 bytes of EEPROM.
 * The first 4 bytes (0-3) is reserved for start time (bitwiseDate)
 * The 5th byte (4) is the marker for where we are in the drive.
 * The 6th byte (5) is reserved, maybe for when we have a larger drive?
 */
unsigned int logStart = 6;
unsigned int logPos = logStart;
uint8_t drivePos = 0;

// default debugging mode is off
bool isDebugging = false;

// timers for sleep and stuff
volatile char sleep_for = 0;
//const unsigned int sleep_cycle = 4090; // TCCR1B = 0x05; = 4.09s
// when debugging, we can decrease the CYCLE_LENGTH so we don't have to wait
// sleep time is based on sleep cycles, each of which is (currently) ~4.09s
// which makes 15 cycles roughly 61.35 seconds (default: 14 at 4.09s 16MHz)
// or 7 at 8.18s 8MHz
// Using CYCLE_LENGTH lets us set numbers in the loop as minutes
unsigned long CYCLE_LENGTH = 7;

#endif
