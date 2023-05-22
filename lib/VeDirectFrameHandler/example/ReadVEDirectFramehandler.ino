/*************************************************************************************
 ReadVeDirectFrameHandler

 Uses VeDirectFrameHandler library

 This example and library tested with NodeMCU 1.0 using Software Serial.
 If using with a platform containing 2 harware UART's, use those, not SoftwareSerial.
 Tested with Victron BMV712.

 VEDirect Device:
   pin 1 - gnd
   pin 2 - RX
   pin 3 - TX
   pin 4 - power

 History:
   2020.05.05 - 0.3 - initial release

**************************************************************************************/

#include "Arduino.h"
#include <SoftwareSerial.h>
#include "VeDirectFrameHandler.h"

VeDirectFrameHandler myve;

// SoftwareSerial
#define rxPin D7                            // RX using Software Serial so we can use the hardware UART to check the ouput
#define txPin D8                            // TX Not used
SoftwareSerial veSerial(rxPin, txPin);         

void setup() {
	Serial.begin(115200);                   // output serial port
    veSerial.begin(19200);                  // input serial port (VE device)
    veSerial.flush();
    Serial.println("DEBUG-setup");
}

void loop() {    
    ReadVEData();
    EverySecond();
}

void ReadVEData() {
    while ( veSerial.available() ) {
        myve.rxData(veSerial.read());
    }
    yield();
}

void EverySecond() {
    static unsigned long prev_millis;

    if (millis() - prev_millis > 1000) {
        PrintData();
        prev_millis = millis();
    }
}

void PrintData() {
    for ( int i = 0; i < myve.veEnd; i++ ) {
    Serial.print(myve.veName[i]);
    Serial.print("= ");
    Serial.println(myve.veValue[i]);    
    }
}
