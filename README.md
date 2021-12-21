# Triac_dimmer
Soft starter for LED lights

Based off code by Scott Driscoll at [https://github.com/curiousinventor/AC-triac-arduino-led-dimmer](https://github.com/curiousinventor/AC-triac-arduino-led-dimmer)

Requires the TimerOne library at [https://github.com/PaulStoffregen/TimerOne](https://github.com/PaulStoffregen/TimerOne)

Uses a generic optoisolated triac module to ramp up the brightness of the load (leds in my case) starting from when the microcontroller powers on

I use a smart outlet switch to power on both the microcontroller and the load for redundant safety

Currently set up with zerocross to be on interupt 0 which is pin 2 for an Arduino Nano and triac control on pin 9. Although it should be usable on any interupt and pin that doesn't interfere with TimerOne

Public domain - share, copy, etc. No warranty
