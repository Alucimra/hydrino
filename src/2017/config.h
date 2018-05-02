#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <EEPROM.h>
#include <power.h>

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

#define LOG_POWER true

// motor power Levels
const uint8_t MAX = 255; // 1023/1023
const uint8_t STRONG = 150; // 140 = 90-100mA 560/1023
const uint8_t WEAK = 135; // 120 = 75-90mA 480/1023, if too low, motor stalls!
const uint8_t DISABLED = 0;
const int MOTOR_START_WAIT = 1000;

bool motorArunning = false;
bool motorBrunning = false;
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
//const int SOLAR = 800;      // 3.6v
//const int FULL = 787;       // 3.4v
//const int CHARGED = 764;    // 3.3v
//const int NOMINAL = 741;    // 3.2v
//const int DRAINED = 717;    // 3.1v
//const int CUTOFF = 680;     // 3.0v
const int TOLERANCE = 23;   // 12=0.05v 23 = 0.1
const float VOLTAGE_MULTIPLIER = 4.0 * (9.75/2.45) * (1.11/1023);

/**
 * NOTE: There is no battery protection circuit! We're over-charging!
 * This is pretty bad because if this happens early enough in the day, the
 * battery is going to be over-charged. These motors might not be enough to
 * use up all the solar power on a really bright day.
 *
 * Motors are rated at 3v 150mA or about 0.45W, both running should use ~0.9W
 * TODO: How much power do the motors draw here? (Real check).
 */
cycle_timing SOLAR = generateCycleTiming(
  800,
  0x6, ON, ON,
  0x6, ON, ON,
  0x6, ON, ON,
  0x6, ON, ON
);
/**
 * NOTE: This is where most of the charging happens. A decent amount of the
 * off cycle is required for proper re-charging...but have to be careful
 * not to over charge because we don't have any protection!
 * This is why we're using LiFePO4 batteries, because they can handle the
 * added abuse of a little over-charging here and there.
 */
cycle_timing FULL = generateCycleTiming(
  787,
  0x5, OFF, OFF,
  0x4, ON, OFF,
  0x5, OFF, OFF,
  0x4, OFF, ON
);
cycle_timing CHARGED = generateCycleTiming(
  764,
  0x3, OFF, OFF,
  0x2, ON, OFF,
  0x3, OFF, OFF,
  0x2, OFF, ON
);
/**
 * NOTE: At this point (3.2v), we should still have a good chunk of the real
 * battery power (70-80% on a LiFePO4). Most of the night should be here.
 */
cycle_timing NOMINAL = generateCycleTiming(
  741,
  0x3, OFF, OFF,
  0x1, ON, OFF,
  0x3, OFF, OFF,
  0x1, OFF, ON
);
/**
 * NOTE: Voltage starts to drop sharply starting here; there's less power
 * than it looks.
 */
cycle_timing DRAINED = generateCycleTiming(
  690,
  0x15, OFF, OFF,
  0x1, ON, OFF,
  0x15, OFF, OFF,
  0x1, OFF, ON
);

cycle_timing *timings[] = {&SOLAR, &FULL, &CHARGED, &NOMINAL, &DRAINED};
cycle_timing *cycleTime = &DRAINED;
volatile cycle currentCycle;

/**
 * NOTE: There's (almost) no power left after drained. We need to conserve energy.
 * The chip stops working properly at 2.7-2.9v. The power led and clocks run
 * but it doesn't really work like it should, sometimes stalls!
 * It's operating out of spec when it reaches here. Should avoid.
 */

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
// Using CYCLE_LENGTH lets us set cycle_time numbers as multiples of a time
unsigned long CYCLE_LENGTH = 60000;

#endif
