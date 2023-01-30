#ifndef MAIN_H
#define MAIN_H

const int WETPIN = 25;
const int FANPIN = 22;
const int DUMPPIN = 33;
const int HIGHLEVELPIN = 14;
const int LOWLEVELPIN = 12;

const int PWM_FREQ = 5000;
const int PWM_RES = 12;

// CANBus
const int CTX = 16; // TX2
const int CRX = 17; // RX2

void serialRX();

void RX_CAN();
void receive(); 

#endif //MAIN_H