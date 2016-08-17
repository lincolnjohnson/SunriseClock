/*----------------------------------------------------------------------*
 * Modified version of SetSetial.ino from Jack Christensen's DS3231     *
 * library. Also initializes EEPROM values to those required for        *
 * sunrise clock. Initial Comment from SetSerial.ino included below.    *
 * Modify values of alarmHour and alarmMinute as desired.               *
 * Lincoln Johnson, August 2016                                         *
 *----------------------------------------------------------------------*
 * Display the date and time from a DS3231 or DS3232 RTC every second.  *
 * Display the temperature once per minute. (The DS3231 does a          *
 * temperature conversion once every 64 seconds. This is also the       *
 * default for the DS3232.)                                             *
 *                                                                      *
 * Set the date and time by entering the following on the Arduino       *
 * serial monitor:                                                      *
 *    year,month,day,hour,minute,second,                                *
 *                                                                      *
 * Where                                                                *
 *    year can be two or four digits,                                   *
 *    month is 1-12,                                                    *
 *    day is 1-31,                                                      *
 *    hour is 0-23, and                                                 *
 *    minute and second are 0-59.                                       *
 *                                                                      *
 * Entering the final comma delimiter (after "second") will avoid a     *
 * one-second timeout and will allow the RTC to be set more accurately. *
 *                                                                      *
 * No validity checking is done, invalid values or incomplete syntax    *
 * in the input will result in an incorrect RTC setting.                *
 *                                                                      *
 * Jack Christensen 08Aug2013                                           *
 *                                                                      *
 * Tested with Arduino 1.0.5, Arduino Uno, DS3231/Chronodot, DS3232.    *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/ 
 
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
#include <EEPROM.h>           //https://www.arduino.cc/en/Reference/EEPROM

//EEPROM Vars
const byte DST = 100;
const byte BrightStor = 101;
const byte MaxHour = 102;
const byte MaxMinute = 103;
const byte EESunFollow = 104;
const byte SunOn = 105;

const byte alarmHour = 8;
const byte alarmMinute = 30;

void setup(void)
{
    Serial.begin(115200);
    
    //setSyncProvider() causes the Time library to synchronize with the
    //external RTC by calling RTC.get() every five minutes by default.
    setSyncProvider(RTC.get);
    Serial << F("RTC Sync");
    if (timeStatus() != timeSet) Serial << F(" FAIL!");
    Serial << endl;

    //updates EEPROM values
    EEPROM.update( BrightStor, 234 ); //sets initial brightness PWM at 60000
    EEPROM.update( MaxHour, alarmHour ); //sets alarm hour as defined above
    EEPROM.update( MaxMinute, alarmMinute ); //sets alarm minute as defined above
    EEPROM.update( EESunFollow, 0 ); //sets alarm to not follow natural sunrise
    EEPROM.update( SunOn, 1 ); //sets sunrise alarm to ON
    CheckDST(); //sets if current time is in DST
}

void loop(void)
{
    static time_t tLast;
    time_t t;
    tmElements_t tm;

    //check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
    if (Serial.available() >= 12) {
        //note that the tmElements_t Year member is an offset from 1970,
        //but the RTC wants the last two digits of the calendar year.
        //use the convenience macros from Time.h to do the conversions.
        int y = Serial.parseInt();
        if (y >= 100 && y < 1000)
            Serial << F("Error: Year must be two digits or four digits!") << endl;
        else {
            if (y >= 1000)
                tm.Year = CalendarYrToTm(y);
            else    //(y < 100)
                tm.Year = y2kYearToTm(y);
            tm.Month = Serial.parseInt();
            tm.Day = Serial.parseInt();
            tm.Hour = Serial.parseInt();
            tm.Minute = Serial.parseInt();
            tm.Second = Serial.parseInt();
            t = makeTime(tm);
            RTC.set(t);        //use the time_t value to ensure correct weekday is set
            setTime(t);        
            Serial << F("RTC set to: ");
            printDateTime(t);
            Serial << endl;
            //dump any extraneous input
            while (Serial.available() > 0) Serial.read();
        }
    }
    
    t = now();
    if (t != tLast) {
        tLast = t;
        printDateTime(t);
        if (second(t) == 0) {
            float c = RTC.temperature() / 4.;
            float f = c * 9. / 5. + 32.;
            Serial << F("  ") << c << F(" C  ") << f << F(" F");
        }
        Serial << endl;
    }
}

//print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

//print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

//print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

//Print an integer in "00" format (with leading zero),
//followed by a delimiter character to Serial.
//Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}

void CheckDST() {
  if ( EEPROM.read(DST) == 0 && IsDST() == 1 )
  {
    EEPROM.write(DST, 1);
    setTime(RTC.get());
    setTime( hour() + 1, minute(), second(), day(), month(), year() );
    RTC.set( now() );
  }

  if ( EEPROM.read(DST) == 1 && IsDST() == 0 )
  {
    EEPROM.write(DST, 2);
    setTime(RTC.get());
    setTime( hour() - 1, minute(), second(), day(), month(), year() );
    RTC.set(now());
  }

  if ( EEPROM.read(DST) == 2 && day() > 8 )
  {
    EEPROM.write(DST, 0);
  }
}

bool IsDST() {
  //January, february, and december are out.
  if (month() < 3 || month() > 11) {
    return false;
  }
  //April to October are in
  if (month() > 3 && month() < 11) {
    return true;
  }
  //march
  if (month() == 3)
  {
    if (day() > 14) {
      return true;
    }
    //before second sunday is out
    if (day() - weekday() < 7) {
      return false;
    }
    //before 2am on second sunday is out
    if (weekday() == 1 && hour() < 2) {
      return false;
    }
    //otherwise in
    {
      return true;
    }
  }
  else //november
  {
    //before first sunday is in
    if (day() - weekday() < 0) {
      return true;
    }
    //after first sunday is out
    if (day() - weekday() > 7) {
      return false;
    }
    //before 2am on second sunday
    if (weekday() == 1 && hour() < 2 && day() - weekday() < 7) {
      return true;
    }
    //otherwise out
    {
      return false;
    }
  }
}