This is a refactored and updated version of the code in the https://github.com/kiyoshigawa/oMIDItone github repo intended to simplify and make faster the existing code base while simultaneously switching to PlatformIO.

It is code written to run a 6-head MIDI controlled Otamatone synth, including lighting and mouth opening/closing servo animations. The project is running on a Teensy 3.2, using the arduino framework through PlatformIO, and is using the following libraries in addition to the libraries that I have written myself specifically for this project:
https://github.com/adafruit/Adafruit_NeoPixel 
https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library (Modified to work with i2c_t3.h)
https://github.com/nox771/i2c_t3
https://github.com/pedvide/ADC


The updated version is reworked to make handling MIDI it's own MIDIController class, as well as to simplify the oMIDItone object class to operate based only on desired frequencies instead of handling MIDI notes, pitch bends, etc.

This readme information will be lagging behind the actual development of the code base until it is nearly done and working properly, so look for more up-to-date comments in the source files for the time being. Once the project is working properly and unlikely to ahve significant changes, I will add a better writeup of everything here.

All my code is being released under the GPLV3 unless otherwise noted in the included third party libraries that the project is built on.