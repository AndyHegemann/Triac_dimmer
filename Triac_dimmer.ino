//small change for testing

/* 
Written by Andy Hegemann with code by Scott Driscoll, public domain - share, copy, etc. No warranty
12/21/2021

Youtube for Scott Driscoll code: https://youtu.be/DzqvI6oM0-c

Requires TimerOne and LowPower Libraries
Requires zerocross to be on interupt 0 which is pin 2 for an Arduino Nano

This code was made as a soft start for LED lights that are on a smart timer, it slowly ramps the brightness up
according to the ramp_time using a generic optoisolated triac module

This requires the arduino be powered by the smart timer, for safety I also power the LEDs with the smart timer
*/


#include <TimerOne.h>
#include <LowPower.h>

volatile unsigned long dim = 8200;  // Default Dimming level, 8100 is off, 500 is brightest setting
unsigned long freqStep = 1000000;

int triac_pin = 9;                  // Triac connected to digital pin 9
bool triac_on = false;

float brightness_setpoint = 0.0;

unsigned long last_run = 0;
float inc_delay;
float ramp_time = 180;  // ramp up time in seconds


void setup() {
  pinMode(triac_pin, OUTPUT);                              // Set triac control pin as output
  attachInterrupt(0, zero_cross_detect, RISING);   // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection

  Timer1.initialize(freqStep);
  Timer1.attachInterrupt(dim_check,freqStep);

  inc_delay = ((ramp_time / 1000) * 1000) - 1 ;     // (ramp time in seconds / 1000 setpoints) * 1000 milliseconds per second minus processor overhead
}


void loop() {

  if(millis() - last_run > inc_delay){  //every delay period increase brightness 
    last_run = millis();
    brightness_setpoint = brightness_setpoint + 0.1;
    dim = linearize(brightness_setpoint); // convert to a dim value needed by dimmer
  }

  if(brightness_setpoint >= 100){
    halt();
  }
}


void halt() {  //remove all interupts and then set the triac to always be on and then do nothing
  Timer1.detachInterrupt();
  detachInterrupt(digitalPinToInterrupt(2));
  digitalWrite(triac_pin, HIGH);
  delay(500);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  // while(1){}   //use if you don't want to use the lowpower library 
}


// This gets called when the AC signal cross zero and sets a delay for when the triac should be turned on thru Timer1.
// The longer the delay, the dimmer the LED as the lower percentage of each cycle the triac is held on.
void zero_cross_detect() {
   Timer1.setPeriod(dim);
} 


// Timer1 triggered
void dim_check() {     
  if (!triac_on) {
      if (dim < 7999) {
        digitalWrite(triac_pin, HIGH);
      }  
      triac_on = true;   
      Timer1.setPeriod(8000 - dim);     // call this function again to turn off the triac signal near the end of the cycle. 
      // the total cycle is about 8000us, so I just subtract my dim from 7500 to get the off time.
  }
  else {
    digitalWrite(triac_pin, LOW); 
    triac_on = false; 
    Timer1.setPeriod(1000000);
  }   
}


// takes an input 0-100 and creates an output that makes the LED behave more linearly according to how humans perceive the light.
// smaller return is brighter
// 8100 is off, 500 is brightest setting
float linearize(float i) {
  unsigned long output = 8100; // effectively off
  if (i <= 0.09) {
    return 8000;
  }
  if (i < 10.0) {
    return mapf(i, .09, 9.99, 7900, 7500);
    
  }
  else if (i < 50.0) {
   return mapf(i, 10, 50.0, 7500, 6000);
  }
  else if (i< 90.0) {
    return mapf(i, 50, 90.0, 6000, 3000);
  }
  else if (i<98.0) {
    return mapf(i, 90, 98.0, 3000, 500.0);
  }
  else 
    return 500.0;
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
