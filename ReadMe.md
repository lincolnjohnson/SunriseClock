# Arduino SunriseClock v3.2 #
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

I've included a schematic and Eagle files for PCB printing. The PCB is designed to be split into two boards, one for the arduino, one for the rotary encoder and switch to be mounted near the top of the case I've designed. Case plans are in the Drawings folder.

Functionality included:

- 16-bit PWM control of LED lamp brightness
- Sunrise brightness uses an exponential function to ramp up brightness in a way that appears linear to the human eye
- User defined Sunrise (alarm) start time, ramps up over 30 minutes
- Choose to have sunrise follow the natural sunrise time or not (with the latest time being the above defined time)
- Automatic dimming after two hours at full brightness
- User-adjustable brightness of light when outside of sunrise period
- Set RTC clock using rotary encoder
- Automatic RTC adjustment for DST
- TODO: ADD WEEKEND AUTOMATIC ADJUSTMENT FOR EXTRA SLEEP TIME

Essentially, once initial program is loaded onto clock it never needs to be re-connected unless you want to update the baseline functionality. You can do everything you should need to do through the various menus.

I made some minor adjustments to a couple of the included libraries or used non-standard versions, so they're included in this repository as well so as to make your life easier if you want to compile/build this. Everything in the includes folder is not my own original work. The other included files listed such as EEPROM and Wire are standard arduino versions.

There are dual signal wires to the MOSFET controlling the LEDs because it seemed that one signal wire wasn't allowing the system to open fully when using a transistor. Not sure why, but doubling up appears to have fixed it - at full duty cycle it's just as bright as a direct connection to power, especially with the first MOSFET switching the source voltage to drive the second one. Now that I've switched to using a MOSFET, one is fine but two signals doesn't hurt so I left it. If you need the extra pin for something else you can always remove one of 9/10 and update the code appropriately.

The Pro Micro tends to be cranky with the Arduino IDE sometimes, especially if you are trying to load code to it for the first time, or switching between sketches. I find that jumpering the RST and GND pins, pressing upload on the IDE, and then disconnecting the jumper as soon as the IDE finishes compiling and outputs the size of the files used fixes this problem. Subsequent uploads with the same sketch should work fine. Not sure if this is a problem related to my use of cheap ebay clones.

I strongly suggest you use a high-quality power supply, as when cheap ebay ones start to die they tend to send out significant voltage spikes that fry things... such as your Arduino.

"Arduino SunriseClock v3.2" by Lincoln Johnson is licensed under CC BY-SA 4.0.
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
- 1x 80W+ power supply (PMT-12V100W1AA)
- 1x 5m 5630 LED strip
- 1x RTC Clock (ebay DS3231 clock module board)
- 1x 8x32 LED display board with HT1632C controller
- 1x 12v->5V DC converter (BP5293-50)
- 1x Pushbutton switch (1825910-6)
- 1x Rotary encoder with pushbutton switch (EN12-HS22AF20)
- 2x 0.1uF capacitor (K104K10X7RF5UH5)
- 2x n-channel mosfet (FQU13N06L)
- 2x 100ohm resistors
- 2x 100kohm resistors
- 2x 12 pin female headers to enable easy replacement of arduino in case of failure (PPTC121LFBN-RC)
- 3x 6 pin headers to connect 3208 matrix and switches board (1x 801-87-006-20-001101 right angle for 3208 & 2x 310-87-106-41-001101 for switches)
- 1x 5 pin header to connect RTC (310-87-105-41-001101)
- 1x 2 pin header to connect LED strip (310-87-102-41-001101)
- 1x screw terminal header for power connection to power supply (1935161)
- Various hookup/connection wires
- Breadboard if no PCB

## Materials used for physical build ##

- 1x 5" to 4" ducting reducer, spray painted matte black
- 1x 4" ducting connector
- 1x 4" ducting cap
- 1x 5" DIA 9" height acrylic canister with white tissue paper as shade
- 1x AC power entry connector (701W-X2/02 or similar)
- 1x Aluminum knob for rotary encoder (Ebay)
- 1x Pushbutton cap for switch (TAGCHR)
- 4.5 ft of 6" wide hardwood for case

## Main Board Schematic ##

This is the main board without the rotary encoder and button on it. If you want it on one board, remove the TOSWITCHES headers and connect them as required.

![Main board schematic](https://github.com/lincolnjohnson/SunriseClock/blob/master/MainSch.png)

## Eagle Main Board PCB ##

I haven't tested this board yet as it hasn't shown up from the fab. It's probably wrong/may cause a black hole to form and destroy the planet. Use at your own risk. Board size: 1.5464 sqin.

This will need additional physical wires soldered on the LED power ciruit to handle the ~72W power drain if you use the set trace length.

![Main eagle PCB](https://github.com/lincolnjohnson/SunriseClock/blob/master/EagleMain.png)

## Switches Board Schematic ##

This is the board with the rotary encoder and button on it. If you want it on one board, remove the TOSWITCHES headers and connect them as required.

![Main board schematic](https://github.com/lincolnjohnson/SunriseClock/blob/master/SwitchSch.png)

## Eagle Switches Board PCB ##

I haven't tested this board yet as it hasn't shown up from the fab. It's probably wrong/may cause a black hole to form and destroy the planet. Use at your own risk. Board size: 1.1135 sqin.

![Main eagle PCB](https://github.com/lincolnjohnson/SunriseClock/blob/master/EagleSwitch.png)

## Case Design ##

Complete drawings follow below. Sides not shown as they are solid. PDF with full instructions for construction in Drawings folder.

![Clock case](https://github.com/lincolnjohnson/SunriseClock/blob/master/Assembly.png)

###Completed case

![Clock case](https://github.com/lincolnjohnson/SunriseClock/blob/master/Drawings/completed.png)

###Base drawings

![Base drawings](https://github.com/lincolnjohnson/SunriseClock/blob/master/Drawings/back.png)

###Front drawings

![Front drawings](https://github.com/lincolnjohnson/SunriseClock/blob/master/Drawings/front.png)

###Top drawings

![Top drawings](https://github.com/lincolnjohnson/SunriseClock/blob/master/Drawings/top.png)

###Back drawings

![Back drawings](https://github.com/lincolnjohnson/SunriseClock/blob/master/Drawings/back.png)

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

###void toggleLamp(boolean on)
#####Description
Toggles lamp on or off
#####Syntax
`toggleLamp(0);`
#####Parameters
*(boolean)* on - 0 or false is off, 1 or true is on
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
