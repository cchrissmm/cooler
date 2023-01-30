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

State current_state = State::vanOn; //default state is van on, such that a dump triggers when van is turned on

void cooler::task1000ms() {
//the state machine will be here running in loop

switch(current_state) {
    case State::idle: // system idle
        cooler::fanOff();
        cooler::wetOff();
        cooler::dumpOff();
        cooler::fillOff();
        break;

    case State::vanOn: // van on
        if(cooler::dumpOn()) { // if dump cycle is complete
            current_state = State::idle;
        }
        break;

    case State::startUp: // start up cooler

        if(cooler::dumpOn()){  
            if(cooler::fillOn) {
                if(cooler::wetOn()) {
                    if (wetTimer > WET_CYCLE_TIME) {
                        cooler::fanOn(255);
                    }
                    current_state = State::runCool;
                }
            }
        }
        break;

    case State::runCool: // run cooler in cool mode
        cooler::wetOn();
        cooler::fanOn(255);
        cooler::fillOn;
        current_state = State::runCool;
        break;

    case State::runFanOnly:
        //run cooler in fan only mode
        break;

    case State::shutDown: // shut down cooler
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

int cooler::fanOn(int speed) {
    analogWrite(fanPin, speed);
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
