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

// HYDRINO_CONFIG
#endif
