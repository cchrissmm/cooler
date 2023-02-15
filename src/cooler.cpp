#include <cooler.h>
#include <Arduino.h>
#include <CAN.h>
#include <Timer.h>
#include <main.h>

Timer tDumpCycle;

cooler::cooler(int wetPin, int fanPin, int dumpPin, int highLevelPin, int lowLevelPin)
{
    this->wetPin = wetPin;
    this->fanPin = fanPin;
    this->dumpPin = dumpPin;
    this->highLevelPin = highLevelPin;
    this->lowLevelPin = lowLevelPin;
}

enum class State
{
    idle,       // doing nothing
    vanOn,      // turning on van causes a dump cycle
    startUp,    // starting up cooler. dump() then fill(), then run().
    runCool,    // running in cool mode. run() with fill() whenever required
    runFanOnly, // running in fan only mode
    shutDown,   // shutting down from cool mode. wetOff(), dump(), fanOff()
};

State current_state = State::vanOn; // default state is van on, such that a dump triggers when van is turned on

///***PUBLIC FUNCTIONS*********************************************************///

void cooler::runCooler()
{
    //current_mode = Mode::cool;
    current_state = State::startUp; // start up cooler
    Serial.println("Current cooler mode is cool");
    Serial.println("low level pin is: " + String(touchRead(lowLevelPin)));
    Serial.println("high level pin is: " + String(touchRead(highLevelPin)));
}

void cooler::setFanSpeed(int fanSpeed)
{
    this->fanSpeed = fanSpeed;
    Serial.println("Cooler fan set to " + String(fanSpeed));

    // if the state is idle then go into fan only mode, else we assume its already running, so just change the speed
    if (current_state == State::idle)
    {
    current_state = State::runFanOnly; // run cooler in fan only mode
    }
   
    
}

void cooler::stopCooler()
{
    //current_mode = Mode::off;

    if (current_state == State::runCool)
    {
        current_state = State::shutDown; // shut down cooler
    }
    else
    {
        current_state = State::idle; // go straight to idle
    }
    Serial.println("Current cooler mode is off");
}

///***TICKER AND STATE MACHINES*********************************************************///
void cooler::task1000ms()
{
    Serial.println("low level pin is: " + String(touchRead(lowLevelPin)));
    Serial.println("high level pin is: " + String(touchRead(highLevelPin)));
    
    // the state machine will be here running in loop
    switch (current_state)
    {
    case State::idle: // system idle
        Serial.println("Current cooler state is idle");
        cooler::fanOff();
        cooler::wetOff();
        cooler::dumpOff();
        cooler::fillOff();
        break;

    case State::vanOn: // van on
        Serial.println("Current cooler state is vanOn");
        if (cooler::dumpOn())
        { // if dump cycle is complete
            current_state = State::idle;
        }
        break;

    case State::startUp: // start up cooler
        Serial.println("Current cooler state is startUp");
        Serial.println("dump pump done state is: " + String(StartupDumpDone));

        if (cooler::dumpOn()) // if dump cycle is complete or already done
        {
            StartupDumpDone = 1;
            Serial.println("dump done, request fill cycle...");
            
            if (cooler::fillOn())
            {
                Serial.println("fill done, request wet cycle...");
                if (cooler::wetOn())
                {
                    wetTimer += 1;
                    Serial.println("pre wet running...");
                    if (wetTimer > WET_CYCLE_TIME)
                    {
                        cooler::fanOn();
                        wetTimer = 0;
                        current_state = State::runCool;
                        Serial.println("request runCool");
                        StartupDumpDone = 0;
                    }
                }
            }
        }
        break;

    case State::runCool: // run cooler in cool mode
        Serial.println("Current cooler state is runCool");
        cooler::wetOn();
        cooler::fanOn();
        cooler::fillOn();
        current_state = State::runCool;
        break;

    case State::runFanOnly:
        // run cooler in fan only mode
        Serial.println("Current cooler state is runFanOnly");
        cooler::wetOff();
        cooler::fanOn();
        cooler::fillOff();
        cooler::dumpOff();
        break;

    case State::shutDown: // shut down cooler
        Serial.println("Current cooler state is shutDown");
        cooler::fillOff();
        cooler::wetOff();
        if (cooler::dumpOn())
        {
            fanPostRunTime += 1;
            Serial.println("fan post running...");
            if (fanPostRunTime > POSTRUN_FAN_TIME) {
                cooler::fanOff();
                fanPostRunTime = 0;
                current_state = State::idle;
            }
        }

        break;

    default:
        // do nothing
        break;
    }
}

//***COOLER SUBROUTINES*********************************************************///

int cooler::wetOn()
{
    Serial.println("wet pump on");
    analogWrite(wetPin, 255);
    return 1;
}

int cooler::wetOff()
{
    Serial.println("wet pump off");
    analogWrite(wetPin, 0);
    return 1;
}

int cooler::fanOn()
{
    analogWrite(fanPin, fanSpeed);
    Serial.println("fan on at " + String(fanSpeed));
    return 1;
}

int cooler::fanOff()
{
    Serial.println("fan off");
    analogWrite(fanPin, 0);
    return 1;
}

int cooler::dumpOn()
{
    if (touchRead(lowLevelPin) < LEVELPIN_THRESHOLD && StartupDumpDone == 0)
    {
        analogWrite(dumpPin, 255);
        Serial.println("dump pump triggered at " + String(touchRead(lowLevelPin)));
        return 0;
    }
    else
    {
        analogWrite(dumpPin, 0);
        Serial.println("dump pump idle at " + String(touchRead(lowLevelPin)));
        return 1;
    }
    return 0;
}

int cooler::dumpOff()
{
    Serial.println("dump pump off");
    analogWrite(dumpPin, 0);
    StartupDumpDone = 0;
    return 1;
}

int cooler::fillOn()
{
    if (touchRead(highLevelPin) > LEVELPIN_THRESHOLD)
    {
        Serial.println("fill level low, fill pump running at " + String(touchRead(highLevelPin)));
        //  CAN.beginPacket(0x20, 2);
        //  CAN.write(1);
        //  CAN.write(0);
        //  CAN.endPacket();
        return 0;
    }
    else
    {
        Serial.println("fill level ok, fill pump idle at " + String(touchRead(highLevelPin)));
        // CAN.beginPacket(0x20, 2);
        // CAN.write(0);
        // CAN.write(1);
        // CAN.endPacket();
        return 1;
    }
    return 0;
}

int cooler::fillOff()
{
    Serial.println("fillpump off");
    //  can message to stop fill pump
    // CAN.beginPacket(0x20, 2);
    // CAN.write(0);
    // CAN.write(1);
    // CAN.endPacket();
    return 1;
}
