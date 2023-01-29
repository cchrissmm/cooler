#ifndef COOLER_H_
#define COOLER_H_

#include <Arduino.h>
#include <CAN.h>

class cooler
{
public:
    cooler(int wetPin, int fanPin, int dumpPin, int fillSensPin);
    void task1000ms();        // where the timers run from
    void coolOn();            // turns on the cool function
    void coolOff();           // turns off the cool function
    void fanOn(int fanSPeed); // turns on the fan at the speed desired
    void fanOff();            // turns off the fan

private:
    void dump(); // dump the water
    void levelCheck();
    void fill(); // fill the water

private:
    int wetPin, fanPin, dumpPin, fillSensPin; // pins
    int coolerRunning;                        // flag to tell if the cooler is running
};

#endif