#include <cooler.h>
#include <Arduino.h>
#include <CAN.h>
#include <Timer.h>

Timer timer_DumpCycle;

cooler::cooler(int wetPin, int fanPin, int dumpPin, int fillSensPin)
{
    this->wetPin = wetPin;
    this->fanPin = fanPin;
    this->dumpPin = dumpPin;
    this->fillSensPin = fillSensPin;
}

void cooler::task1000ms()
{ // where the timers run from
    cooler::levelCheck(); //monitor the level of the water
}

void cooler::coolOn()
{                              // turns on the cool function
    coolerRunning = 1;         // set the run flag
    cooler::fanOn(255);         // start the fan
    analogWrite(wetPin, 255); // start the wet pump
}

void cooler::coolOff()
{                      // turns off the cool function
    coolerRunning = 0; // reset the run flag
    cooler::dump();     // dump the water
}

void cooler::fanOn(int fanSpeed)
{                                   // turns on the fan
    analogWrite(fanPin, fanSpeed); // start the fan
}

void cooler::fanOff()
{                            // turns off the fan
    analogWrite(fanPin, 0); // stop the fan
}

void cooler::levelCheck()
{
    if ((digitalRead(fillSensPin) == 0) && coolerRunning)
    { // if the fill sensor is low
        // send can message to the fill pump run
    }
    else
    {
        // send can message to the fill pump to stop
    }
}

void cooler::dump()
{ // dump the water
    analogWrite(dumpPin, 255); // start the dump pump
    
    analogWrite(dumpPin, 0);   // stop the dump pump
}