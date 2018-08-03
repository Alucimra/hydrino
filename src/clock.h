#include <config.h>

#ifndef HYDRINO_CLOCK
#define HYDRINO_CLOCK

struct rtc_dt {
  uint8_t year:7;
  uint8_t month:4;
  uint8_t dayOfMonth:5;
  uint8_t hour:4;
  uint8_t minute:6;
  uint8_t second:6;
  // dayOfWeek isn't too useful as a separate field, and it pushes this struct
  // into 32+8 territory. But it's part of ds3231 data...so it'll stay for now.
  // Note: It's not guaranteed to exist. check hasDayOfWeek
  // Note: We 0-index this value, but the DS3231 is 1-indexed.
  uint8_t dayOfWeek:3;
  bool hasDayOfWeek;
  //uint8_t extra:4;
};

struct rtc_bytes {
  uint8_t one;
  uint8_t two;
  uint8_t three;
  uint8_t four;
  uint8_t extra;
};

union rtc_datetime {
  struct rtc_dt datetime;
  struct rtc_bytes bytes;
};

typedef union rtc_datetime rtc_datetime_t;


// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val) { return( (val/10*16) + (val%10) ); }

// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val) { return( (val/16*10) + (val%16) ); }

#ifndef DS3231_ID
  void setTime(rtc_datetime_t *newTime){}

  rtc_datetime_t getTime(){
    rtc_datetime_t blank;
    return blank;
  }
#else
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
#endif

#endif
