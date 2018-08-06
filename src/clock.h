#include <config.h>

#ifndef HYDRINO_CLOCK
#define HYDRINO_CLOCK

struct rtc_dt {
  uint8_t year:7;
  uint8_t month:4;
  uint8_t dayOfMonth:5;
  uint8_t hour:5;
  uint8_t minute:6;
  uint8_t second:6;
  // 33 bits at this point.
  // dayOfWeek isn't too useful as a separate field, and it pushes this struct
  // into 5 byte territory. But it's part of ds3231 data...so it'll stay for now.
  // Note: It's not guaranteed to exist. check hasDayOfWeek
  // Note: We 0-index this value, but the DS3231 is 1-indexed.
  uint8_t dayOfWeek:3;
  bool hasDayOfWeek;
  //uint8_t extra:3;
};

union rtc_datetime {
  struct rtc_dt datetime;
  uint8_t bytes[5];
};

typedef union rtc_datetime rtc_datetime_t;

union rtc_temp {
  int temp;
  uint8_t bytes[2];
};

typedef union rtc_temp rtc_temp_t;

// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val) { return( (val/10*16) + (val%10) ); }

// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val) { return( (val/16*10) + (val%16) ); }

int32_t epochDays(uint8_t dayOfMonth, uint8_t month, uint8_t year){
  year -= (month <= 2);
  int16_t era = (year >= 0 ? year : year - 399) / 400;
  uint16_t yoe = (year - era * 400); // 0..399
  uint16_t doy = ((uint16_t)153*(month + (month > 2 ? -3 : 9)) + 2)/5 + dayOfMonth-1; // 0..365
  uint32_t doe = yoe * 365 + yoe/4 - yoe/100 + doy; // 0..146096
  // Note: remove the 719468 to set epoch to 2000-03-01 instead of 1970-01-01
  return ((int32_t)era * 146097 + doe - 719468);
}

// Note: For this to work properly, the time needs to be set as UTC
// because the RTC does not track timezone (and daylight savings)
uint32_t dateToUnixTimestamp(rtc_datetime_t *date){
  uint32_t unixtime = (uint32_t)86400 * epochDays(date->datetime.dayOfMonth, date->datetime.month, 2000+date->datetime.year);
  unixtime += 3600 * date->datetime.hour;
  unixtime += 60 * date->datetime.minute;
  unixtime += date->datetime.second;

  return unixtime;
}

#ifndef DS3231_ID
  void setTime(rtc_datetime_t *newTime){}

  rtc_datetime_t getTime(){
    rtc_datetime_t blank;
    return blank;
  }

  rtc_temp_t getTemperature(){
    rtc_temp_t blank;
    return blank;
  }
#else
  // Temperature Sensor (DS3231)
  #define DS3231_TEMPERATURE_MSB 0x11
  #define DS3231_TEMPERATURE_LSB 0X12

  void setTime(rtc_datetime_t *newTime) {
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_ID);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(newTime->datetime.second)); // set seconds
    Wire.write(decToBcd(newTime->datetime.minute)); // set minutes
    Wire.write(decToBcd(newTime->datetime.hour)); // set hours
    Wire.write(decToBcd(newTime->datetime.dayOfWeek+1)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(newTime->datetime.dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(newTime->datetime.month)); // set month (1-12)
    Wire.write(decToBcd(newTime->datetime.year)); // set year (0 to 99)
    Wire.endTransmission();
  }

  rtc_datetime_t getTime() {
    Wire.beginTransmission(DS3231_ID);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_ID, 7);
    // request seven uint8_ts of data from DS3231 starting from register 00h

    rtc_datetime_t currentTime;

    uint32_t start = millis();
    uint8_t lastAt = 0;

    while(lastAt < 7 && (millis() - start) < 30000){
      if(Wire.available() > 0){
        if(lastAt == 0){ currentTime.datetime.second = bcdToDec(Wire.read() & 0x7f); }
        if(lastAt == 1){ currentTime.datetime.minute = bcdToDec(Wire.read()); }
        if(lastAt == 2){ currentTime.datetime.hour = bcdToDec(Wire.read() & 0x3f); }
        if(lastAt == 3){ currentTime.datetime.dayOfWeek = bcdToDec(Wire.read()) - 1; }
        if(lastAt == 4){ currentTime.datetime.dayOfMonth = bcdToDec(Wire.read()); }
        if(lastAt == 5){ currentTime.datetime.month = bcdToDec(Wire.read()); }
        if(lastAt == 6){ currentTime.datetime.year = bcdToDec(Wire.read()); }
        lastAt++;
      }
    }

    return currentTime;
  }

  rtc_temp_t getTemperature(){
    rtc_temp_t temp;

    Wire.beginTransmission(DS3231_ID);
    Wire.write(DS3231_TEMPERATURE_MSB);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_ID, 2);

    temp.bytes[1] = Wire.read();
    temp.bytes[0] = Wire.read();

    return temp;
  }

#endif

#endif
