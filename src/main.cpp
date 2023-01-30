#include <Arduino.h>
#include <CAN.h>
#include <main.h>
#include <SimpleTimer.h>
#include <cooler.h>

SimpleTimer Task_1000ms(1000);
cooler cooler01(WETPIN, FANPIN, DUMPPIN, HIGHLEVELPIN, LOWLEVELPIN);

void setup() {
  Serial.begin(115200); // initialise USB serial
  CAN.setPins(CRX, CTX);
  // start the CAN bus at 500 kbps
  if (!CAN.begin(125E3))
  {
    Serial.println("Starting CAN failed!");
    while (1)
      ;
  }
  CAN.loopback(); // sent packets are also received
  Serial.println("CAN Connected");

  
  ledcAttachPin(FANPIN, 0);
  ledcSetup(0, PWM_FREQ, PWM_RES);
  pinMode(WETPIN, OUTPUT);
  pinMode(DUMPPIN, OUTPUT);
  pinMode(HIGHLEVELPIN, INPUT); // may need pullup
  pinMode(LOWLEVELPIN, INPUT); // may need pullup

  Serial.println("Setup done");
}

void loop() {
  if (Task_1000ms.isReady())
  {
    Task_1000ms.reset();
    serialRX();
    cooler01.task1000ms();
  }
}

void serialRX()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');

    if (str.startsWith("COOLON"))
    {
      Serial.println("recieved: " + str);
      cooler01.runCooler();
    }
    if (str.startsWith("COOLOFF"))
    {
      Serial.println("recieved: " + str);
      cooler01.stopCooler();
    }
    if (str.startsWith("FANLOW"))
    {
      Serial.println("recieved: " + str);
      cooler01.setFanSpeed(125);
    }
    if (str.startsWith("FANHIGH"))
    {
      Serial.println("recieved: " + str);
      cooler01.setFanSpeed(255);
    }
  }
}
