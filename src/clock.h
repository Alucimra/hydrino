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
  Wire.write(decToBcd(month)); // set month (1-12)
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

uint32_t dateToUnixTimestamp(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfMonth, uint8_t month, uint8_t year){
  year -= m <= 2;
  int16_t era = (year >= 0 ? year : year - 399) / 400;
  uint16_t yoe = (year - era * 400); // 0..399
  uint16_t doy = ((uint16_t)153*(month + (month > 2 ? -3 : 9)) + 2)/5 + dayOfMonth-1; // 0..365
  uint32_t doe = yoe * 365 + yoe/4 - yoe/100 + doy; // 0..146096
  // Note: remove the 719468 to set epoch to 2000-03-01 instead of 1970-01-01
  return (int32_t)era * 146097 + doe - 719468);
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

/*
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
*/

#endif
