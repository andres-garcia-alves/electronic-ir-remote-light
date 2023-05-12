/*
  Electronic IR Remote Light v1.2

  Created by: Andres Garcia Alves <andres.garcia.alves@gmail.com>
  
  Version 1.0, 2023.05.07 - Initial release.
  Version 1.1, 2023.05.08 - Adding use of sleep mode + interrupts to reduce power consumption.
  Version 1.2, 2023.05.11 - Rearrange assigned pins to improve PCB design.

  This source code is licensed under GPL v3.0  
*/

#include <IRremote.h>
#include <LowPower.h>

#define DEBUG           false
#define DEBOUNCE_DELAY  50

#define IR_SENSOR       2
#define LED_ON          A0
#define RELAY           A5

// codes depends on actual remote control
#define REMOTE_BTN_1    4151
#define REMOTE_BTN_2    6199

// variables & constants declaration
IRrecv irReceiver(IR_SENSOR);
decode_results results;
bool dataAvailable;

// interruption handler
void irInterruptHandler() { }


void setup() {

  #if DEBUG
  Serial.begin(9600);
  #endif

  pinMode(IR_SENSOR, INPUT_PULLUP);
  pinMode(LED_ON, OUTPUT);
  pinMode(RELAY, OUTPUT);

  irReceiver.enableIRIn();

  // enable external interrupts on IR-sensor pin
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR), irInterruptHandler, FALLING); // RISING, FALLING
}

void loop() {

  // enable enter in power-down state
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);

  // check remote control pressed button (twice)
  checkRemoteControl();
  checkRemoteControl();

  delay(20);
}

void checkRemoteControl() {

  delay(50);
  dataAvailable = irReceiver.decode(&results);

  if (dataAvailable) {

    irReceiver.blink13(true);

    #if DEBUG
    Serial.print("Code received. ");
    Serial.print("Type " + (String)results.decode_type + ", ");
    Serial.println("Value " + (String)results.value + " (decimal).");
    delay(20);
    #endif

    // enable/disable relay
    if (results.value == REMOTE_BTN_1 || results.value == REMOTE_BTN_2) {
      digitalWrite(RELAY, !digitalRead(RELAY));
      digitalWrite(LED_ON, !digitalRead(LED_ON));
      delay(500);
    }
  }

  irReceiver.resume();
  delay(500);
}
