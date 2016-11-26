#include <Arduino.h>
#include <Wire.h>
#include  <TimerOne.h>

// measurment constants
const float refVoltage = 4.44;
const float currentConstant1 = 0.1;// V:A
const float currentConstant2 = 0.066;// V:A
const float currentVoltageConstant1 = refVoltage/(1024*currentConstant1);
const float currentVoltageConstant2 = refVoltage/(1024*currentConstant2);

// Pins in use


void setup() {
  Serial.begin(9600);
}

void loop() {

}
