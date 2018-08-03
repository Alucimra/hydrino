#include <config.h>
#include <clock.h>

#ifndef HYDRINO_DEBUG_SETTINGS
#define HYDRINO_DEBUG_SETTINGS

void printPins(){
  Serial.print(F(":: GPIO pins:> {\"MOTOR_ON\": "));
  Serial.print(MOTOR_ON);
  Serial.print(F(", \"BATTERY_SENSE\": "));
  Serial.print(BATTERY_SENSE);
  Serial.print(F("}"));
  Serial.println();

  Serial.print(F(":: I2C Addresses i2c:> {"));

  #ifdef DRIVE_ID
  Serial.print(F("\"DRIVE_ID\": \"0x"));
  Serial.print(DRIVE_ID, HEX);
  Serial.print(F("\", "));
  #endif

  #ifdef DS3231_ID
  Serial.print(F("\"DS3231_ID\": \"0x"));
  Serial.print(DS3231_ID, HEX);
  Serial.print(F("\", "));
  #endif

  #ifdef DS3231_TEMPERATURE_MSB
  Serial.print(F("\"DS3231_TEMPERATURE_MSB\": \"0x"));
  Serial.print(DS3231_TEMPERATURE_MSB, HEX);
  Serial.print(F("\", "));
  #endif

  #ifdef DS3231_TEMPERATURE_LSB
  Serial.print(F("\"DS3231_TEMPERATURE_LSB\": \"0x"));
  Serial.print(DS3231_TEMPERATURE_LSB, HEX);
  Serial.print(F("\", "));
  #endif

  Serial.print(F("\"\":0}"));
  Serial.println();
}

void printBatterySettings(){
  Serial.print(F(":: Battery Levels and cycle times battery:> {\"CYCLE_ON_TIME\": "));
  Serial.print(CYCLE_ON_TIME);
  Serial.print(F(", \"CYCLE_OFF_MULT\": "));
  Serial.print(CYCLE_OFF_MULT);
  Serial.print(F(", \"CYCLE_FLEX_THRESHOLD\": "));
  Serial.print(CYCLE_FLEX_THRESHOLD);
  Serial.print(F(", \"BATTERY_FLEX\": "));
  Serial.print(BATTERY_FLEX);
  Serial.print(F(", \"BATTERY_OVERCHARGE\": "));
  Serial.print(BATTERY_OVERCHARGE);
  Serial.print(F(", \"BATTERY_MAX\": "));
  Serial.print(BATTERY_MAX);
  Serial.print(F(", \"BATTERY_MIN\": "));
  Serial.print(BATTERY_MIN);
  Serial.print(F("}"));
  Serial.println();
}

#if DRIVE_ID
  void printDriveSettings(){
    Serial.print(F(":: Drive Settings drive:> {\"DRIVE_ID\": \"0x"));
    Serial.print(DRIVE_ID, HEX);
    Serial.print(F("\", \"DRIVE_SPACE\": "));
    Serial.print(DRIVE_SPACE);
    Serial.print(F(", \"DRIVE_WRITE_LIMIT\": "));
    Serial.print(DRIVE_WRITE_LIMIT);
    Serial.print(F(", \"DRIVE_MARKER_BYTE\": "));
    Serial.print(DRIVE_MARKER_BYTE);
    Serial.print(F("}"));
    Serial.println();
  }
#else
  void printDriveSettings(){
    Serial.print(F(":: Drive not setup; DRIVE_ID not set drive:> {\"DRIVE_ID\": null}"));
    Serial.println();
  }
#endif

#if DS3231_ID
  void printClockSettings(){
    Serial.print(F(":: Clock (DS3231) ds3231:> {\"DS3231_ID\": \"0x"));
    Serial.print(DS3231_ID, HEX);
    Serial.print(F("\", \"now\": \""));
    rtc_datetime_t now = getTime();
    Serial.print(now.datetime.year);
    Serial.print(F("-"));
    Serial.print(now.datetime.month);
    Serial.print(F("-"));
    Serial.print(now.datetime.dayOfMonth);
    Serial.print(F(" "));
    Serial.print(now.datetime.hour);
    Serial.print(F(":"));
    if(now.datetime.minute < 10){
      Serial.print(0);
    }
    Serial.print(now.datetime.minute);
    Serial.print(F(":"));
    if(now.datetime.second < 10){
      Serial.print(0);
    }
    Serial.print(now.datetime.second);
    Serial.print(F(" UTC "));

    Serial.print(F("\"}"));
    Serial.println();
  }
#else
  void printClockSettings(){
    Serial.print(F(":: Clock not setup; DS3231_ID not set ds3231:> {\"DS3231_ID\": null}"));
    Serial.println();
  }
#endif

#if defined(DS3231_TEMPERATURE_MSB) && defined(DS3231_TEMPERATURE_LSB)
  void printTemperatureSettings(){
    Serial.print(F(":: Temperature from DS3231 ds3231_temp:> {"));

    #ifdef DS3231_TEMPERATURE_MSB
    Serial.print(F("\"DS3231_TEMPERATURE_MSB\": \"0x"));
    Serial.print(DS3231_TEMPERATURE_MSB, HEX);
    Serial.print(F("\", "));
    #endif

    #ifdef DS3231_TEMPERATURE_LSB
    Serial.print(F("\"DS3231_TEMPERATURE_LSB\": \"0x"));
    Serial.print(DS3231_TEMPERATURE_LSB, HEX);
    Serial.print(F("\", "));
    #endif

    // TODO: Add current temperature
    Serial.print(F("\"set\": true}"));
    Serial.println();
  }
#else
  void printTemperatureSettings(){
    Serial.print(F(":: Temperature not setup ds3231_temp:> {"));

    #ifdef DS3231_TEMPERATURE_MSB
    Serial.print(F("\"DS3231_TEMPERATURE_MSB\": \"0x"));
    Serial.print(DS3231_TEMPERATURE_MSB, HEX);
    Serial.print(F("\", "));
    #endif

    #ifdef DS3231_TEMPERATURE_LSB
    Serial.print(F("\"DS3231_TEMPERATURE_LSB\": \"0x"));
    Serial.print(DS3231_TEMPERATURE_LSB, HEX);
    Serial.print(F("\", "));
    #endif

    Serial.print(F("\"set\": false}"));
    Serial.println();
  }
#endif

void printAllSettings(){
  printPins();
  printBatterySettings();
  printDriveSettings();
  printClockSettings();
  printTemperatureSettings();
}

#endif
