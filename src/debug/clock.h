#include <config.h>
#include <clock.h>
//#include <time.h>
// TODO: include time.h for strftime and unixtime to rtc_datetime conversion
// NOTE: avr libc's epoch is on 1-1-2000

#ifndef HYDRINO_DEBUG_CLOCK
#define HYDRINO_DEBUG_CLOCK

void displayTemperature(rtc_temp_t *temp){
  Serial.print( ((temp->temp >> 6) * 25) / 100 );
  Serial.print(F("."));
  Serial.print( ((temp->temp >> 6) * 25) % 100 );
  Serial.print(F(" C"));
}

void displayShortTime(rtc_datetime_t *time){
  Serial.print(time->datetime.year, DEC);
  Serial.print(F("-"));
  Serial.print(time->datetime.month, DEC);
  Serial.print(F("-"));
  Serial.print(time->datetime.dayOfMonth, DEC);
  Serial.print(F(" "));

  // send it to the serial monitor
  Serial.print(time->datetime.hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(F(":"));
  if (time->datetime.minute < 10) { Serial.print(F("0")); }
  Serial.print(time->datetime.minute, DEC);
  Serial.print(F(":"));
  if (time->datetime.second < 10) { Serial.print(F("0")); }
  Serial.print(time->datetime.second, DEC);
}

void displayTime(rtc_datetime_t *time) {
  Serial.print(F("Weekday (0=Sun, 6=Sat): "));
  Serial.print(time->datetime.dayOfWeek);
  Serial.print(F(" "));
  displayShortTime(time);
  Serial.println();
  Serial.print(F(":: unixtime:> "));
  Serial.print(dateToUnixTimestamp(time));
  Serial.println();
}

  #ifndef DS3231_ID

    void readTime(){
      Serial.print(F(":( Clock disabled or unavailable, can't get time"));
      Serial.println();
    }

    void writeTime(){
      Serial.print(F(":( Clock disabled or unavailable, can't set time"));
      Serial.println();
    }

    void readTemperature(){
      Serial.print(F(":( Clock disabled or unavailable, can't read temperature"));
      Serial.println();
    }

  #else
    void readTime(){
      rtc_datetime_t now = getTime();
      Serial.print(F(":) Reading the time: "));
      displayTime(&now);
      Serial.println();
    }

    /**
     * There's a little problem with this. Serial baud of 9600 is a little slow and
     * we can't quite pin down the exact timing of the seconds.
     * We can get close by setting a time ahead, and waiting until a second before
     * to press y and start the save. But it could still be about ~1-2s off
     * Can't trust Serial printing either, because there's still the delay.
     */
    void writeTime(){
      rtc_datetime_t newTime;
      Serial.setTimeout(60000);
      // give up to 60 seconds to enter the input
      Serial.println(F(":) Note: Time is assumed to be UTC for easy unixtime conversion"));

      Serial.print(F(":) Year (0 to 99)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.year = (uint8_t)Serial.parseInt() % 100;
      Serial.print(newTime.datetime.year);
      Serial.println();

      Serial.print(F(":) Month (1 to 12)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.month = (uint8_t)Serial.parseInt() % 13;
      Serial.print(newTime.datetime.month);
      Serial.println();

      Serial.print(F(":) Day of Month (1 to 31)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.dayOfMonth = (uint8_t)Serial.parseInt() % 32;
      Serial.print(newTime.datetime.dayOfMonth);
      Serial.println();

      Serial.print(F(":) Day of Week (0 to 6, Sun to Sat)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.dayOfWeek = (uint8_t)Serial.parseInt() % 8;
      Serial.print(newTime.datetime.dayOfWeek);
      Serial.println();

      Serial.print(F(":) Hour (0 to 23)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.hour = (uint8_t)(Serial.parseInt() % 24);
      Serial.print(newTime.datetime.hour);
      Serial.println();

      Serial.print(F(":) Minute (0 to 59)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.minute = (uint8_t)Serial.parseInt() % 60;
      Serial.print(newTime.datetime.minute);
      Serial.println();

      Serial.print(F(":) Second (0 to 59)"));
      Serial.println();
      Serial.print(F(":] "));
      newTime.datetime.second = (uint8_t)Serial.parseInt() % 60;
      Serial.print(newTime.datetime.second);
      Serial.println();
      Serial.setTimeout(2000);

      uint32_t start = millis();
      // clear the serial buffer in case they typed anything
      while(Serial.available() > 0){ Serial.read(); }

      Serial.print(F(":) Setting time to ("));
      displayTime(&newTime);
      Serial.print(F("). Press y to continue, or anything else to cancel."));
      Serial.println();
      Serial.print(F(":] "));
      // wait for serial to be available to read and write
      while(Serial.available() <= 0){}
      uint8_t accept = Serial.read();
      Serial.print(accept);
      if(accept == 'y'){
        Serial.print(F(":) Setting the time...please wait..."));

        setTime(&newTime);
        uint32_t done = millis();
        delay(500);

        Serial.print(F("Done! Took "));
        Serial.print(done - start);
        Serial.print(F("ms behind the wire"));
        Serial.println();
        readTime();
      } else {
        Serial.print(F(":( Canceled by user."));
        Serial.println();
      }
    }

    void readTemperature(){
      rtc_temp_t now = getTemperature();
      Serial.print(F(":) Reading the temperature: "));
      displayTemperature(&now);
      Serial.println();
    }
  #endif
#endif
