#include <cooler.h>
#include <Arduino.h>
#include <CAN.h>
#include <Timer.h>

Timer tDumpCycle;

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
    cooler::dumpCheck();  // check if the water needs to be dumped
}

void cooler::coolOn()
{                              // turns on the cool function
    coolerRunning = 1;         // set the run flag
    cooler::fanOn(255);         // start the fan
    analogWrite(wetPin, 255); // start the wet pump
    status = "cooling on";
}

void cooler::coolOff()
{                      // turns off the cool function
    coolerRunning = 0; // reset the run flag
    cooler::dump();     // dump the water
    dumpRequested = 1;  // set the dump flag
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
    if ((digitalRead(fillSensPin) == 0) && coolerRunning) // if the fill sensor is low and the cooler is running
    { 
        // send can message to the fill pump run
    }
    else
    {
        // send can message to the fill pump to stop
    }
}

void cooler::dumpCheck()
{ // dump the water
if(dumpRequested)
{
        analogWrite(dumpPin, 255); // start the dump pump
        tDumpCycle.start();        // start the timer
        if(tDumpCycle.hasPassed(DUMP_CYCLE_TIME))
        {
            analogWrite(dumpPin, 0);   // stop the dump pump
            tDumpCycle.stop();         // stop the timer
            dumpRequested = 0;         // reset the flag
        }
}
    analogWrite(dumpPin, 0);   // stop the dump pump
}

void cooler::wetCheck()
{
    if (wetRequested)
    {
        analogWrite(wetPin, 255); // start the wet pump
        tWetCycle.start();        // start the timer
        if (tWetCycle.hasPassed(WET_CYCLE_TIME))
        {
            analogWrite(wetPin, 0);   // stop the wet pump
            tWetCycle.stop();         // stop the timer
            wetRequested = 0;         // reset the flag
        }
    }
    analogWrite(wetPin, 0);   // stop the wet pump
}

String cooler::getStatus() {
    return status;
}