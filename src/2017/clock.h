#include <config.h>

#ifndef HYDRINO_CLOCK
#define HYDRINO_CLOCK

// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val) { return( (val/10*16) + (val%10) ); }

// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val) { return( (val/16*10) + (val%16) ); }

void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year) {
  power_twi_enable();
  delay(1000);
  // sets time and date data to DS3231
  Wire.beginTransmission(CLOCK_ID);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
  power_twi_disable();
}

void readTime(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year) {
  power_twi_enable();
  delay(1000);
  Wire.beginTransmission(CLOCK_ID);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(CLOCK_ID, 7);
  // request seven uint8_ts of data from DS3231 starting from register 00h
  unsigned long currentTime = millis();
  uint8_t lastAt = 0;
  while(lastAt < 7 && (millis() - currentTime) < 30000){
    if(Wire.available() > 0){
      if(lastAt == 0){ *second = bcdToDec(Wire.read() & 0x7f); }
      if(lastAt == 1){ *minute = bcdToDec(Wire.read()); }
      if(lastAt == 2){ *hour = bcdToDec(Wire.read() & 0x3f); }
      if(lastAt == 3){ *dayOfWeek = bcdToDec(Wire.read()); }
      if(lastAt == 4){ *dayOfMonth = bcdToDec(Wire.read()); }
      if(lastAt == 5){ *month = bcdToDec(Wire.read()); }
      if(lastAt == 6){ *year = bcdToDec(Wire.read()); }
      lastAt++;
    }
  }
  power_twi_disable();
}

/* This is the structure of the decimal date vs bitwise arranged as a union
 * TODO: Consider switching code to use the union instead? Won't need conversion function calls.
 */
/*
union BitwiseDate {
  uint32_t decimal;
  struct {
    uint8_t year:6;
    uint8_t month:4;
    uint8_t dayOfMonth:5;
    uint8_t hour:6;
    uint8_t minute:6;
    uint8_t second:6;
  };
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
  };
};
*/

// NOTE: bisewiseDate signature is different from the rest: there's no dayOfWeek!
uint32_t dateToBitwiseDate(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfMonth, uint8_t month, uint8_t year){
  uint32_t bitwiseDate = 0;
  year %= 100;

  bitwiseDate += ((uint32_t)year << 26);
  bitwiseDate += ((uint32_t)month << 22);
  bitwiseDate += ((uint32_t)dayOfMonth << 17);
  bitwiseDate += ((uint32_t)hour << 12);
  bitwiseDate += ((uint32_t)minute << 6);
  bitwiseDate += (uint32_t)second;

  return bitwiseDate;
}

void bitwiseDateToDate(uint32_t bitwiseDate, uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year){
  *second = (uint8_t)(bitwiseDate & 63);
  bitwiseDate >>= 6;
  *minute = (uint8_t)(bitwiseDate & 63);
  bitwiseDate >>= 6;
  *hour = (uint8_t)(bitwiseDate & 31);
  bitwiseDate >>= 5;
  *dayOfMonth = (uint8_t)(bitwiseDate & 31);
  bitwiseDate >>= 5;
  *month = (uint8_t)(bitwiseDate & 15);
  bitwiseDate >>= 4;
  *year = (uint8_t)(bitwiseDate & 63);
}

void splitBitwiseDate(uint32_t bitwiseDate, uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d){
  *a = (uint8_t)(bitwiseDate & 255);
  bitwiseDate >>= 8;
  *b = (uint8_t)(bitwiseDate & 255);
  bitwiseDate >>= 8;
  *c = (uint8_t)(bitwiseDate & 255);
  bitwiseDate >>= 8;
  *d = (uint8_t)(bitwiseDate & 255);
}

uint32_t joinBitwiseDate(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
  uint32_t bitwiseDate = 0;

  bitwiseDate += d;
  bitwiseDate <<= 8;
  bitwiseDate += c;
  bitwiseDate <<= 8;
  bitwiseDate += b;
  bitwiseDate <<= 8;
  bitwiseDate += a;

  return bitwiseDate;
}


#endif
