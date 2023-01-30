#ifndef COOLER_H_
#define COOLER_H_

#include <Arduino.h>
#include <CAN.h>

class cooler
{
public:
    cooler(int wetPin, int fanPin, int dumpPin, int highLevelPin, int lowLevelPin);
    void task1000ms();  // ticker
    void coolerMode(String mode);  // sets the cooler to the desired mode (cool, fan, off)

private:
    int fanOn(int fanSpeed), fanOff(), dumpOn(), dumpOff(), wetOn(), wetOff(), fillOn(), fillOff();


private:
    int wetPin, fanPin, dumpPin, highLevelPin, lowLevelPin; // pins              
    int fanSpeed;                             // speed of the fan
    int WET_CYCLE_TIME = 30;               // duration of the wet cycle in ms before the fans start in seconds
    int POSTRUN_FAN_TIME = 30;             // duration of the fan run after the cooler has been turned off
    int wetTimer;                             // timer for the wet cycle
    
                           
};

#endif