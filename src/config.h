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
#define HAS_DRIVE false
#define DRIVE_ID 0x50
#define SAVE_TO_DRIVE_AT 1000
#define DRIVE_SPACE 32768
#define DRIVE_WRITE_LIMIT 25


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


// Voltage Levels
// WARNING: Assumes internal reference voltage is 1.0v. See docs/voltage levels.txt
// aRead = (battery_voltage / 4) / (1.0 / 1023)
// battery_voltage = aRead * 4 * (1.0 / 1023)
// Real life read checks:
// 225*4 = 900 = ~4.1v (8/26/17) vref = 1.165
const int OVERCHARGE = 800; // >3.6v
const int SOLAR = 790; // ~3.6v
const int FULL = 746; // 3.4v
const int CHARGED = 720; // 3.3v
const int NOMINAL = 702; // 3.2v
const int DRAINED = 680; // 3.1v
const int CUTOFF = 660; // 3.0v
const int TOLERANCE = 26; // 13=0.05v


// we have 1024 bytes of EEPROM
// marker at position 0 and 1 tells us where we are, 2 is for extra storage
unsigned int logPos = 3; // first two bytes is reserved for logPointer
unsigned int logStart = 3;

bool isDebugging = false; // default debugging mode

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
