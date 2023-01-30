#include <cooler.h>
#include <Arduino.h>
#include <CAN.h>
#include <Timer.h>

Timer tDumpCycle;

cooler::cooler(int wetPin, int fanPin, int dumpPin, int highLevelPin, int lowLevelPin)
{
    this->wetPin = wetPin;
    this->fanPin = fanPin;
    this->dumpPin = dumpPin;
    this->highLevelPin = highLevelPin;
    this ->lowLevelPin = lowLevelPin;
}

enum class State
{
    idle, //doing nothing
    vanOn, //turning on van causes a dump cycle
    startUp, //starting up cooler. dump() then fill(), then run().
    runCool, //running in cool mode. run() with fill() whenever required
    runFanOnly, //running in fan only mode
    shutDown, //shutting down from cool mode. wetOff(), dump(), fanOff() 
};

enum class Mode
{
    cool, //cool mode
    fanOnly, //fan only mode
    off, //off mode
};

State current_state = State::vanOn; //default state is van on, such that a dump triggers when van is turned on
Mode current_mode = Mode::off; //default mode is off

///***PUBLIC FUNCTIONS*********************************************************///

void cooler::runCooler()
{
    current_mode = Mode::cool;
}

void cooler::setFanSpeed(int fanSpeed)
{
    this->fanSpeed = fanSpeed;
    current_mode = Mode::fanOnly;
}

void cooler::stopCooler()
{
    current_mode = Mode::off;
}

///***TICKER AND STATE MACHINES*********************************************************///
void cooler::task1000ms() {
switch(current_mode) {
    case Mode::cool:
        current_state = State::startUp; //start up cooler
        Serial.print("Current cooler mode is cool");
        break;

    case Mode::fanOnly:
        if(current_state == State::runCool) {
            current_state = State::shutDown; //shut down cooler
        }
        if(current_state == State::shutDown) {
            //do nothing and wait
        }
        else {
            current_state = State::runFanOnly; //run cooler in fan only mode
        }
        Serial.print("Current cooler mode is fanOnly");
        break;

    case Mode::off:
        if (current_state == State::runCool)
        {
            current_state = State::shutDown; // shut down cooler
        }
        if (current_state == State::shutDown)
        {
            // wait for it to shut down
        }
        else
        {
            current_state = State::idle; // then idle
        }
        Serial.print("Current cooler mode is off");
        break;

    default:
        //do nothing
        break;
}

//the state machine will be here running in loop
switch(current_state) {
    case State::idle: // system idle
        Serial.print("Current cooler state is idle");
        cooler::fanOff();
        cooler::wetOff();
        cooler::dumpOff();
        cooler::fillOff();
        break;

    case State::vanOn: // van on
        Serial.print("Current cooler state is vanOn");
        if(cooler::dumpOn()) { // if dump cycle is complete
            current_state = State::idle;
        }
        break;

    case State::startUp: // start up cooler
        Serial.print("Current cooler state is startUp");
        if(cooler::dumpOn()){  
            if(cooler::fillOn()) {
                if(cooler::wetOn()) {
                    if (wetTimer > WET_CYCLE_TIME) {
                        cooler::fanOn();
                    }
                    current_state = State::runCool;
                }
            }
        }
        break;

    case State::runCool: // run cooler in cool mode
        Serial.print("Current cooler state is runCool");
        cooler::wetOn();
        cooler::fanOn();
        cooler::fillOn();
        current_state = State::runCool;
        break;

    case State::runFanOnly:
        //run cooler in fan only mode
        Serial.print("Current cooler state is runFanOnly");
        cooler::wetOff();
        cooler::fanOn();
        cooler::fillOff();
        cooler::dumpOff();
        break;

    case State::shutDown: // shut down cooler
        Serial.print("Current cooler state is shutDown");
        cooler::fillOff();
        cooler::wetOff();
        if(cooler::dumpOn()) {
            cooler::fanOff();
            current_state = State::idle;
        }
        
        break;

    default:
        //do nothing
        break;
}

}

//***COOLER SUBROUTINES*********************************************************///

int cooler::wetOn() {
    digitalWrite(wetPin, HIGH);
    wetTimer += 1;
    return 1;
}

int cooler::wetOff() {
    digitalWrite(wetPin, LOW);
    wetTimer = 0;
    return 1;
}

int cooler::fanOn() {
    analogWrite(fanPin, fanSpeed);
    return 1;
}

int cooler::fanOff() {
    analogWrite(fanPin, 0);
    return 1;
}

int cooler::dumpOn() {      
    if(digitalRead(lowLevelPin) == 1) {
    digitalWrite(dumpPin, HIGH);
    return 0;
    }
    else {
        return 1;
    }
    return 0;
}

int cooler::dumpOff() {
    digitalWrite(dumpPin, LOW);
    return 1;
}

int cooler::fillOn() {
    if(digitalRead(highLevelPin) == 0) {
    //can message to start fill pump
        CAN.beginPacket(0x20, 2);
        CAN.write(1);
        CAN.write(0);
        CAN.endPacket();
        return 0;
    }
    else {
        // can message to stop fill pump
        CAN.beginPacket(0x20, 2);
        CAN.write(0);
        CAN.write(1);
        CAN.endPacket();
        return 1;
    }
    return 0;
}

int cooler::fillOff() {
    //  can message to stop fill pump
    CAN.beginPacket(0x20, 2);
    CAN.write(0);
    CAN.write(1);
    CAN.endPacket();
    return 1;
}
