//https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino
//https://www.circuitsathome.com/mcu/rotary-encoder-interrupt-service-routine-for-avr-micros

#include <EEPROM.h>           //https://www.arduino.cc/en/Reference/EEPROM
#include <TimeLord.h>         //https://github.com/probonopd/TimeLord
#include <HT1632.h>           //https://github.com/gauravmm/HT1632-for-Arduino
#include <font_7X5.h>         //https://github.com/gauravmm/HT1632-for-Arduino
#include <font_5x4.h>         //https://github.com/gauravmm/HT1632-for-Arduino
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Time.h>             //https://github.com/PaulStoffregen/Time
#include <TimeLib.h>          //https://github.com/PaulStoffregen/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire

const boolean debugbit = false;

const byte pulsePin = 9;
const byte pulsePin2 = 10;
const byte stopPin = 7;
const byte relayPin = 8;

const byte SQWPin = 1;

//LED Screen pins
const byte pinDATA = 19;
const byte pinWR = 20;
const byte pinCS1 = 18;

//EEPROM Vars
const byte DST = 100;
const byte BrightStor = 101;
const byte MaxHour = 102;
const byte MaxMinute = 103;
const byte EESunFollow = 104;
const byte SunOn = 105;

unsigned long targetMillis = 0;
const unsigned long interval = 1757;

long duty = 0;
const int steps = 1024;

//alarm stuff
// SET EARLIEST SUNRISE LIMITS - 1 hour before latest?
volatile byte latestHour;
volatile byte latestMin;
volatile byte alarmHour;
volatile byte alarmMinute;
volatile boolean alarmed = false;
volatile boolean tripped = false;
time_t alarmStart;

//timelord stuff
const float latitude = 49;
const float longitude = -119;
volatile boolean gotSunrise = false;

//brightness var
volatile unsigned int brightLevel = 60000;

//rotary encoder
#define ENC_WR  PORTB //encoder port write  
#define ENC_RD  PINB  //encoder port read
#define ENC_A 14 // Bent cap pin to 14
#define ENC_B 16
const byte switchPin = 0; // for rotary switch
volatile int encVal = 0;
volatile uint8_t old_AB = 0;
volatile const int8_t enc_states [] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; //encoder lookup table
volatile unsigned long last_interrupt_time = 0;
volatile unsigned long interrupt_time = 0;

//menu vars
const byte none = 0;
const byte menu = 1;
const byte doSunrise = 11;
const byte setBright = 12;
const byte sunFollow = 13;
const byte sunHour = 14;
const byte sunMinute = 15;
const byte setClk = 20;
const byte cHour = 21;
const byte cMinute = 22;
const byte cSecond = 23;
const byte cDay = 24;
const byte cMonth = 25;
const byte cYear = 26;
volatile int EEPROMVar;
volatile boolean inMenu = 0;
volatile int timeVar;
volatile byte knobVar = 0;
volatile byte selectIndex = 0;
char* selectText [] = { "Rise ? ", "SetLux?", "Follow?", "Rise HR?", "RiseMin", "Set Clk" };
char* selectClock [] = { "Set Hr", "Set Min", "Set Sec", "Set Day", "Set Mth", "Set Year" };

void setup(void) {

  if (debugbit) {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Start!");

    EEPROM.update( BrightStor, 234 );
    EEPROM.update( MaxHour, 8 );
    EEPROM.update( MaxMinute, 30 );
    EEPROM.update( EESunFollow, 1 );
    EEPROM.update( SunOn, 1 );

    alarmHour = 8;
    alarmMinute = 30;
  }

  pinMode(pinCS1, OUTPUT);
  pinMode(pinWR, OUTPUT);
  pinMode(pinDATA, OUTPUT);

  HT1632.begin(pinCS1, pinWR, pinDATA);
  HT1632.clear();
  HT1632.render();
  HT1632.setBrightness(1);
  toggleBlink(false);

  pinMode(stopPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(stopPin), stop, FALLING);
  pinMode(pulsePin, OUTPUT);
  pinMode(pulsePin2, OUTPUT);
  pinMode(relayPin, OUTPUT);

  RTC.squareWave(SQWAVE_1_HZ);
  //RTC.squareWave(SQWAVE_NONE);

  setTime(RTC.get());

  pinMode(SQWPin, INPUT_PULLUP);

  // SET 16 bit PWM
  DDRB |= (1 << 6) | (1 << 5);
  ICR1 = 0xFFFF;
  TCCR1A = 0b10101010;
  TCCR1B = 0b00011001;

  // Set brightness
  setBrightness(brightLevel);

  ShowClock();

  latestHour = EEPROM.read( MaxHour );
  latestMin = EEPROM.read( MaxMinute );

  if ( EEPROM.read( EESunFollow ) == 0 ) { // not following the actual sunrise
    alarmHour = latestHour;
    alarmMinute = latestMin;
  }
  else {
    getSunrise();
  }

  RTC.setAlarm(ALM2_MATCH_HOURS, 0, alarmMinute, alarmHour, 1); //Alarm for sunrise
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_2, false);

  CheckDST();

  digitalWrite(relayPin, HIGH);

  attachInterrupt(digitalPinToInterrupt(SQWPin), timeUpdate, FALLING);

  //rotary encoder
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);

  if (!(debugbit)) {
    //encoder switch interrupt
    attachInterrupt( digitalPinToInterrupt( switchPin ), knobPress, FALLING );
    pinMode(switchPin, INPUT_PULLUP);
  }

  //add interrups for rotary gray code, pins 14 & 16
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT3) | (1 << PCINT2);

  sei();

}

void loop(void) {

  if ( alarmed )
  {
    alarmed = false;

    if ( minute() % 5 == 0 && second() == 0 )
    {
      setTime(RTC.get());
    }

    if ( RTC.alarm(ALARM_2) )
    {
      tripped = false;
      gotSunrise = false;

      if ( EEPROM.read( SunOn ) == 1 ) {

        alarmStart = now();

        //DO SUNRISE
        digitalWrite(relayPin, HIGH);

        static unsigned int z;

        for (z = 1; z <= steps; z++)
        {
          if ( alarmed == true && minute() % 5 == 0 && second() == 0 )
          {
            setTime(RTC.get());
            alarmed = false;
          }

          if (z < 82)
          {
            duty = (long) (65535.0 - (((z * 100.0 / 1024.0) / 902.3) * 65535.0));
          }
          else
          {
            duty = (long) (65535.0 - (pow(((z * 100.0 / 1024.0 + 16.0) / 116.0), 3.0) * 65535.0));
          }

          setBrightness(duty);

          targetMillis = ( (unsigned long) ( millis() + interval ) );

          do {} while ( targetMillis > millis() );

          if (tripped == true)
          {
            break;
          }
        }
      }
    }
  }

  if ( ( readBright() != brightLevel ) && ( now() - alarmStart > 9000 ) ) //Dim after two hours @ full brightness [OVER 9000 SECONDS!]
  {
    setBrightness(brightLevel);
  }

  if ( ( hour() > 0 || hour () < 4 ) && ( month() == 3 || month() == 11 ) && minute() == 0 && second() == 0 ) {
    CheckDST();
  }

  if ( hour() > 12 && gotSunrise == false ) {
    getSunrise();
  }

}

ISR(PCINT0_vect)
{

  old_AB <<= 2; //remember previous state
  old_AB |= ( ENC_RD >> 2 & 0x03 );
  encVal += (enc_states[( old_AB & 0x0f )]);

  /* post "Navigation forward/reverse" event */
  if ( encVal > 3 || encVal < -3 ) {
    if ( inMenu == 0  ) {
      //Serial.println("Not In Menu");
      encVal = 0;
      return;
    }
    if ( knobVar == menu || knobVar == setClk ) { //on main menu
      selectIndex = ( selectIndex + constrain( encVal, -1, 1 ) ) % 6;
      if ( selectIndex > 5 ) {
        selectIndex = 5;
      }
      encVal = 0;
    }
    else {
      encVal = constrain( encVal, -1, 1 );
      KnobUpdate();
      encVal = 0;
    }
  }
}

void stop() {
  static unsigned long stop_last_interrupt_time = 0;
  unsigned long stop_interrupt_time = millis();

  // If interrupts come faster than 250ms, assume it's a bounce and ignore
  if (stop_interrupt_time - stop_last_interrupt_time > 250)
  {
    if ( inMenu ) {
      if ( knobVar == cYear || knobVar == cMonth || knobVar == cDay || knobVar == cSecond || knobVar == cMinute || knobVar == cHour ) {
        knobVar = setClk;
      }
      else if ( knobVar == doSunrise || knobVar == setBright || knobVar == sunFollow || knobVar == sunHour || knobVar == sunMinute || knobVar == setClk) {
        if ( knobVar == setClk ) {
          selectIndex = 5;
        }
        knobVar = menu;
      }
      else {
        knobVar = none;
        inMenu = 0;
      }
    }
    else {
      if ( readBright() == brightLevel ) //if complete or has been tripped previously
      {
        digitalWrite( relayPin, !digitalRead(relayPin) );
        if ( digitalRead(relayPin) == LOW )
        {
          HT1632.clear();
          HT1632.render();
        }
        else
        {
          ShowClock();
        }
      }
      else
      {
        tripped = true;
        setBrightness( brightLevel );
        HT1632.setBrightness(1);
      }
    }
  }
  stop_last_interrupt_time = stop_interrupt_time;
}

void timeUpdate() {
  if ( digitalRead(relayPin) == HIGH ) //Only show clock when lamp on
  {
    ShowClock();
  }
  alarmed = true;
}

void ShowClock() {
  byte Ta = (hour() / 10);
  byte Tb = (hour() % 10);
  byte Tc = (minute() / 10);
  byte Td = (minute() % 10);
  byte Te = (second() / 10);
  byte Tf = (second() % 10);
  char num[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  char Th[1] = {num[Ta]};
  char Th2[1] = {num[Tb]};
  char Tm[1] = {num[Tc]};
  char Tm2[1] = {num[Td]};
  char Ts[1] = {num[Te]};
  char Ts2[1] = {num[Tf]};
  char showhour[9] = {Th[0], Th2[0], ':', Tm[0], Tm2[0], ':', Ts[0], Ts2[0]};
  HT1632.drawText(showhour, 0, 0, FONT_7X5, FONT_7X5_END, FONT_7X5_HEIGHT);
  HT1632.render();
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

void CheckDST() {
  if ( EEPROM.read(DST) == 0 && IsDST() == 1 )
  {
    EEPROM.write(DST, 1);
    detachInterrupt(digitalPinToInterrupt(SQWPin));
    setTime(RTC.get());
    setTime( hour() + 1, minute(), second(), day(), month(), year() );
    RTC.set( now() );
    attachInterrupt(digitalPinToInterrupt(SQWPin), timeUpdate, FALLING);
  }

  if ( EEPROM.read(DST) == 1 && IsDST() == 0 )
  {
    EEPROM.write(DST, 2);
    detachInterrupt(digitalPinToInterrupt(SQWPin));
    setTime(RTC.get());
    setTime( hour() - 1, minute(), second(), day(), month(), year() );
    RTC.set(now());
    attachInterrupt(digitalPinToInterrupt(SQWPin), timeUpdate, FALLING);
  }

  if ( EEPROM.read(DST) == 2 && day() > 8 )
  {
    EEPROM.write(DST, 0);
  }
}

void setBrightness(unsigned int set) {
  OCR1A = set;
  OCR1B = set;
}

unsigned int readBright() {
  return OCR1A;
}

void getSunrise() {

  latestHour = EEPROM.read( MaxHour );
  latestMin = EEPROM.read( MaxMinute );

  if ( EEPROM.read( EESunFollow ) == 0 ) { // not following the actual sunrise
    alarmHour = latestHour;
    alarmMinute = latestMin;
  } else { 
    TimeLord Sun;
    Sun.TimeZone( ( -8 + IsDST() ) * 60  );
    Sun.Position( latitude, longitude );
    time_t next = ( now() + ( ( 36.0 - hour() ) * 60 * 60 ) ); //noon tomorrow
    byte tmo[] = { 0, 0, 12, (byte) day(next), (byte) month(next), 0 };
    Sun.SunRise(tmo);
    //Serial.print("Sunrise: ");
    //Serial.print((int) tmo[tl_hour]);
    //Serial.print(":");
    //Serial.println((int) tmo[tl_minute]);
  
    if ( (int) tmo[tl_hour] < latestHour ) {
      alarmHour = (int) tmo[tl_hour];
      alarmMinute = (int) tmo[tl_minute];
    }
  
    if ( (int) tmo[tl_hour] == latestHour ) {
      alarmHour = (int) tmo[tl_hour];
  
      if ( (int) tmo[tl_minute] <= latestMin ) {
        alarmMinute = (int) tmo[tl_minute];
      }
      else {
        alarmMinute = latestMin;
      }
    }
  
    if ( (int) tmo[tl_hour] > latestHour ) {
      alarmHour = latestHour;
      alarmMinute = latestMin;
    }

  }

  RTC.setAlarm(ALM2_MATCH_HOURS, 0, alarmMinute, alarmHour, 1); //Alarm for sunrise
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_2, false);

  gotSunrise = true;

}

void toggleBlink(boolean blinky) {
  if (blinky) {
    HT1632.sendCommand(HT1632_CMD_BLON);
  }
  else {
    HT1632.sendCommand(HT1632_CMD_BLOFF);
  }
}

void ClockMenu(String text) {
  char charBuf[10];
  text.toCharArray(charBuf, 10);
  HT1632.clear();
  HT1632.drawText(charBuf, 0, 1, FONT_5X4, FONT_5X4_END, FONT_5X4_HEIGHT, 1);
  HT1632.render();
}

void knobPress() {
  // Yes, press it precious
  interrupt_time = millis();

  // If interrupts come faster than 250ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 250)
  {
    if ( inMenu == 0  ) {
      inMenu = 1;
      KnobSelect();
    }

    //in clock menu
    if ( knobVar == setClk )
    {
      switch ( selectIndex ) {
        case 0:
          knobVar = cHour;
          break;
        case 1:
          knobVar = cMinute;
          break;
        case 2:
          knobVar = cSecond;
          break;
        case 3:
          knobVar = cDay;
          break;
        case 4:
          knobVar = cMonth;
          break;
        case 5:
          knobVar = cYear;
          break;
        default:
          break;
      }
    }
    //in main menu
    if ( knobVar == menu )
    {
      switch ( selectIndex ) {
        case 0:
          knobVar = doSunrise;
          break;
        case 1:
          knobVar = setBright;
          break;
        case 2:
          knobVar = sunFollow;
          break;
        case 3:
          knobVar = sunHour;
          break;
        case 4:
          knobVar = sunMinute;
          break;
        case 5:
          selectIndex = 0;
          knobVar = setClk;
          break;
        default:
          break;
      }
    }
    last_interrupt_time = interrupt_time;
  }
}

void KnobSelect() {
  
  last_interrupt_time = interrupt_time;

  sei();
  detachInterrupt(digitalPinToInterrupt(SQWPin)); // disable clock update
  toggleBlink(true);

  setTime(RTC.get());

  knobVar = menu;
  do {
    String outputVar = "test";
    if ( knobVar == menu ) {
      ClockMenu(selectText[selectIndex]);
    }
    if ( knobVar == setClk ) {
      ClockMenu(selectClock[selectIndex]);
    }
    if ( knobVar == doSunrise ) {
      EEPROMVar = EEPROM.read( SunOn );
      if ( EEPROMVar == 0 ) {
        ClockMenu( "Rise? N" );
      }
      else {
        ClockMenu( "Rise? Y" );
      }
    }
    if ( knobVar == setBright ) {
      EEPROMVar = ( 256 - EEPROM.read( BrightStor ) );
      outputVar = "Lux: ";
      outputVar = outputVar + String(EEPROMVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == sunFollow ) {
      EEPROMVar = EEPROM.read( EESunFollow );
      if ( EEPROMVar == 0 ) {
        ClockMenu( "No Foll" );
      }
      else {
        ClockMenu( "Yes Foll" );
      }
    }
    if ( knobVar == sunHour ) {
      EEPROMVar = EEPROM.read( MaxHour );
      outputVar = "Hour: ";
      outputVar = outputVar + String(EEPROMVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == sunMinute ) {
      EEPROMVar = EEPROM.read( MaxMinute );
      outputVar = "Min: ";
      outputVar = outputVar + String(EEPROMVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cHour ) {
      timeVar = hour();
      outputVar = "Hour: " ;
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cMinute ) {
      timeVar = minute();
      outputVar = "Min: ";
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cSecond ) {
      timeVar = second();
      outputVar = "Sec: ";
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cDay ) {
      timeVar = day();
      outputVar = "Day: ";
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cMonth ) {
      timeVar = month();
      outputVar = "Mth: ";
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
    if ( knobVar == cYear ) {
      timeVar = year();
      outputVar = "Yr: ";
      outputVar = outputVar + String(timeVar);
      ClockMenu( outputVar );
    }
  } while ( inMenu );

  knobVar = 0;
  selectIndex = 0;

  toggleBlink(false);

  getSunrise();

  RTC.set( now() );

  
  if ( digitalRead(relayPin) == LOW )
  {
    HT1632.clear();
    HT1632.render();
  }
  else
  {
    ShowClock();
  }
  
  attachInterrupt(digitalPinToInterrupt(SQWPin), timeUpdate, FALLING);
}

void KnobUpdate() {
  // To adjust RTC clock time/date
  // setTime(HH, MM, SS, DD, M, YYYY);
  if ( knobVar == cHour ) {
    setTime( now() + (long) encVal * 60.0 * 60.0);
  }

  if ( knobVar == cMinute ) {
    setTime( hour(), minute() + encVal, second(), day(), month(), year() );
  }

  if ( knobVar == cSecond ) {
    setTime( now() + encVal );
  }

  if ( knobVar == cDay ) {
    setTime( now() + (long) encVal * 24.0 * 60.0 * 60.0 );
  }

  if ( knobVar == cMonth ) {
    byte setVal;
    if ( month () + encVal < 1 ) {
      setVal = 12;
    }
    else if ( month () + encVal > 12 ) {
      setVal = 1;
    }
    else {
      setVal = month() + encVal;
    }
    setTime(hour(), minute(), second(), day(), setVal, year());
  }

  if ( knobVar == cYear ) {
    setTime(hour(), minute(), second(), day(), month(), year() + encVal);
  }

  // Set sunrise latest hour (eeprom)
  if ( knobVar == sunHour ) {
    alarmHour = ( EEPROM.read( MaxHour ) + encVal ) % 24;
    if ( alarmHour > 23 ) {
      alarmHour = 23;
    }
    EEPROM.write( MaxHour, alarmHour );
  }

  // Set sunrise latest minute (eeprom)
  if ( knobVar == sunMinute ) {
    alarmMinute = ( EEPROM.read( MaxMinute ) + encVal ) % 60;
    if ( alarmHour > 59 ) {
      alarmHour = 59;
    }
    EEPROM.write( MaxMinute, alarmMinute );
  }

  // Set sunrise follow or not (eeprom)

  if ( knobVar == sunFollow ) {
    EEPROMVar = EEPROM.read( EESunFollow );

    if ( EEPROMVar == 0 ) {
      EEPROM.write( EESunFollow, 1 );
    }
    else {
      EEPROM.write( EESunFollow, 0 );
    }
  }

  // Set sunrise on or off (eeprom)
  if ( knobVar == doSunrise ) {
    EEPROMVar = EEPROM.read( SunOn );

    if ( EEPROMVar == 0 ) {
      EEPROM.write( SunOn, 1 );
    }
    else {
      EEPROM.write( SunOn, 0 );
    }
  }

  // Adjust light brightLevel (eeprom)
  if ( knobVar == setBright ) {
    EEPROMVar = EEPROM.read( BrightStor );

    if ( EEPROMVar + ( -encVal ) < 255 ) {
      brightLevel = ( EEPROMVar + ( -encVal ) ) * 256 + 96;
      EEPROM.write( BrightStor, EEPROMVar + ( -encVal ) );
    }
    else {
      brightLevel = 65376;
      EEPROM.write( BrightStor, 255 );
    }

    setBrightness(brightLevel);

  }
}