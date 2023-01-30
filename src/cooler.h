#ifndef COOLER_H_
#define COOLER_H_

#include <Arduino.h>
#include <CAN.h>

class cooler
{
public:
    cooler(int wetPin, int fanPin, int dumpPin, int highLevelPin, int lowLevelPin);
    void task1000ms();  // ticker
    void runCooler();   // run the cooler
    void stopCooler();  // stop the cooler
    void setFanSpeed(int fanSpeed); // set the fan speed

private:
    int fanOn(), fanOff(), dumpOn(), dumpOff(), wetOn(), wetOff(), fillOn(), fillOff();


private:
    int wetPin, fanPin, dumpPin, highLevelPin, lowLevelPin; // pins              
    int fanSpeed = 255;                             // speed of the fan
    int WET_CYCLE_TIME = 30;               // duration of the wet cycle in ms before the fans start in seconds
    int POSTRUN_FAN_TIME = 30;             // duration of the fan run after the cooler has been turned off
    int wetTimer;                             // timer for the wet cycle
    
                           
};

#endif