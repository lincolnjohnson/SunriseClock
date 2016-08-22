# Arduino SunriseClock v2.0 #
https://github.com/lincolnjohnson/SunriseClock
ReadMe file  
Lincoln Johnson August 2016

![CC BY-SA](http://mirrors.creativecommons.org/presskit/buttons/80x15/png/by-sa.png)

---

![Crochet Cthulu for scale](https://github.com/lincolnjohnson/SunriseClock/blob/master/Clock.png)

## Introduction ##

An LED sunrise clock using an Arduino Pro Micro, 5M 5630 LED strip (300 LEDs/strip), HT1632C 8X32 LED module, DS3231 RTC Module, and rotary encoder with built-in switch.

I've tried a few of the commercially available sunrise clocks, and none of them seem to go bright enough to really help me wake up. So I went with "bigger is better," and maybe a little too far. The LED strip pulls ~72W of power at full brightness, and at 8.5% is sufficient as a standalone light to read or work by.

Should provide ~10,000 lumens at full brightness. Compare that to off-the-shelf sunrise clocks, which generally provide ~300-800 lumens... It's bright enough to hurt my eyes from across the room, and definitely gets me out of bed. I didn't build in functionality to limit the brightness or change the sunrise length, but these would be reasonably trivial to implement if desired.

The clock can be set to natually shift along with the natural sunrise if desired so that you wake up with the real sunrise during the summer months when it shifts earlier.

I've included a fritzing layout and Eagle files for PCB printing. The PCB is designed to be split into two boards, one for the arduino, one for the rotary encoder and switch to be mounted near the top of the case I'm designing.

Functionality included:

- 16-bit PWM control of LED lamp brightness
- Sunrise brightness uses an exponential function to ramp up brightness in a way that appears linear to the human eye
- User defined Sunrise (alarm) start time, ramps up over 30 minutes
- Choose to have sunrise follow the natural sunrise time or not (with the latest time being the above defined time)
- Automatic dimming after two hours at full brightness
- User-adjustable brightness of light when outside of sunrise period
- Set RTC clock using rotary encoder
- Automatic RTC adjustment for DST

Essentially, once initial program is loaded onto clock it never needs to be re-connected unless you want to update the baseline functionality. You can do everything you should need to do through the various menus.

I made some minor adjustments to a couple of the included libraries or used non-standard versions, so they're included in this repository as well so as to make your life easier if you want to compile/build this. Everything in the includes folder is not my own original work. The other included files listed such as EEPROM and Wire are standard arduino versions.

There are dual signal wires to the transistor controlling the LEDs because it seemed that one signal wire wasn't allowing the system to open fully. Not sure why, but doubling up appears to have fixed it - at full duty cycle it's just as bright as a direct connection to power.

The Pro Micro tends to be cranky with the Arduino IDE sometimes, especially if you are trying to load code to it for the first time, or switching between sketches. I find that jumpering the RST and GND pins, pressing upload on the IDE, and then disconnecting the jumper as soon as the IDE finishes compiling and outputs the size of the files used fixes this problem. Subsequent uploads with the same sketch should work fine. Not sure if this is a problem related to my use of cheap ebay clones.

I strongly suggest you use a high-quality power supply, as when cheap ebay ones start to die they tend to send out significant voltage spikes that fry things... such as your Arduino.

"Arduino SunriseClock v2.0" by Lincoln Johnson is licensed under CC BY-SA 4.0.
Included libraries are licensed under the licenses provided by their respective creators.

## Usage notes ##

- Run InitialSetup.ino on your Arduino board to initialize EEPROM values and set the time on your RTC [alternatively, these can all be set through the configuration menu]
- Update latitude and longitude values for your location in SunriseClock.ino
- Run SunriseClock.ino on your Arduino board

Pushbutton used to:

- Cancel sunrise if you wake up before it completes
- Toggle clock and light on or off when outside sunrise period and not in configuration menu
- Go back one level in configuration menu, or exit config if on top level

Rotary encoder used to:

- Cycle between configuration menu options or option settings
- Pushbutton used to enter config menu or select menu option displayed

## Electronics BOM ##

- 1x Arduino Pro Micro (Nano also tested and works for base functionality, changes would need to be made to schematic)
- 1x 80W+ power supply (PMT-12V100W1AA used)
- 1x 5m 5630 LED strip
- 1x RTC Clock (ebay DS3231 clock module board used)
- 1x 8x32 LED display board with HT1632C controller
- 1x Pushbutton switch (1825910-6 used)
- 1x Rotary encoder with pushbutton switch (EN12-HS22AF20 used)
- 2x 0.1uF capacitor (K104K10X7RF5UH5 used)
- 1x Diode (1N4004 used)
- 1x PNP transistor (KSB772YSTU used) OR 1x n-channel mosfet (FQU13N06L used)
- 1x NPN transistor (KSD882YSTU used)
- 3x 2.2k resistors
- 1x 12V relay (ALKS321 used)
- 2x 11 pin female headers to enable easy replacement of arduino in case of failure
- 3x 6 pin headers to connect 3208 matrix and switches board
- 1x 5 pin header to connect RTC
- 1x 2 pin header to connect LED strip
- 1x screw terminal header for power connection to power supply
- Various hookup/connection wires
- Breadboard if no PCB

## Materials used for physical build ##

- 1x 5" to 4" ducting reducer, spray painted matte black
- 1x 4" ducting connector
- 1x 4" ducting cap
- 1x 5" DIA 9" height acrylic canister with white tissue paper as shade
- 1x AC power entry connector (701W-15/31 or similar)
- 1x Aluminum knob for rotary encoder

**TODO: finish designing the damn case**

## Fritzing Board Layout ##

This is a little messy, but it should get you most of the way there.

![A fritzing breadboard layout](https://github.com/lincolnjohnson/SunriseClock/blob/master/Fritzing.png)

## Eagle board PCB layout ##

I haven't tested this board yet as it hasn't shown up from the fab. It's probably wrong/may cause a black hole to form and destroy the planet. In fact, I think I made a mistake on it, but won't be sure until the boards show up. Use at your own risk.

This will need additional wires soldered on the LED power ciruit to handle the ~72W power drain. Also a 2.2k resistor soldered between pin 9 and position 3 on the PNP resistor spot if you're using a mosfet.

![An eagle PCB](https://github.com/lincolnjohnson/SunriseClock/blob/master/Eagle.png)

## SunriseClock Functions ##

###ISR(PCINT0_vect)
#####Description
Reads the inputs from the rotary encoder gray code to determine if an increment or decrement is desired

###void stop()
#####Description
Called by interrupt, reads the inputs from pushbutton with software debouncing
#####Syntax
`stop();`
#####Parameters
None.
#####Returns
Nothing.

###void timeUpdate()
#####Description
Called by interrupt generated by 1HZ square wave from RTC - calls showClock() if lamp is on
#####Syntax
`timeUpdate();`
#####Parameters
None.
#####Returns
Nothing.

###void ShowClock()
#####Description
Updates clock display based on current time
#####Syntax
`ShowClock();`
#####Parameters
None.
#####Returns
Nothing.

###bool IsDST()
#####Description
Checks if current time is in DST or not
#####Syntax
`IsDST();`
#####Parameters
None.
#####Returns
0 if currently not in DST, 1 if in DST

###void CheckDST()
#####Description
Checks if currently in DST, updates relevant EEPROM values if there has been a change, updates time on RTC
#####Syntax
`CheckDST();`
#####Parameters
None.
#####Returns
Nothing.

###void setBrightness(unsigned int set)
#####Description
Updates clock brightness
#####Syntax
`setBrightness(60000);`
#####Parameters
*(unsigned int)* set - 65536 is off, 0 is full brightness
#####Returns
Nothing.

###unsigned int readBright()
#####Description
Reads current brightness setting
#####Syntax
`readBright();`
#####Parameters
None.
#####Returns
Current brightness level *(unsigned int)*

###void getSunrise()
#####Description
Calculates sunrise time for next day, updates RTC alarm as necessary
#####Syntax
`getSunrise();`
#####Parameters
None.
#####Returns
Nothing.

###void toggleBlink(boolean blinky)
#####Description
Toggles 250ms blink cycle on LED screen
#####Syntax
`toggleBlink(true);`
#####Parameters
*(boolean)* blinky
#####Returns
Nothing.

###void ClockMenu(String text)
#####Description
Updates LED screen (for when in configuration menu)
#####Syntax
`ClockMenu("Set Clk");`
#####Parameters
*(String)* text
#####Returns
Nothing.

###void knobPress()
#####Description
Controls functionality of rotary encoder switch. Enters menu if currently in clock mode, otherwise selects current menu item shown on LED. Includes software debounce.
#####Syntax
`knobPress();`
#####Parameters
None.
#####Returns
Nothing.

###void KnobSelect()
#####Description
Updates LCD screen for menu navigation
#####Syntax
`KnobSelect();`
#####Parameters
None.
#####Returns
Nothing.

###void KnobUpdate()
#####Description
Called by encoder ISR. Controls update of all config variables, EEPROM updates as necessary.
#####Syntax
`KnobUpdate();`
#####Parameters
None.
#####Returns
Nothing
